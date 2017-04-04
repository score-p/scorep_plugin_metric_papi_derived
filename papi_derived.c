#include <scorep/SCOREP_MetricPlugins.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

#include <papi.h>

#include "papi_derived.h"
#include "metrics/metrics.h"


static const Metric_t* available_metrics[] =
{
    &papi_derived_ipc,
};


static void
event_set_add_event( EventSet_t* eventSet, int eventCode )
{
    if ( eventSet->index_map[ eventCode ] != -1 )
    {
        return;
    }

    int native = 0;
    PAPI_CALL( event_name_to_code( counter[ eventCode ], &native ) );
    PAPI_CALL( add_event( eventSet->papi_set, native ) );

    eventSet->index_map[ eventCode ] = eventSet->set_size;
    eventSet->set_size++;
}

static __thread int        ignored_requests[ 1 ];
static __thread EventSet_t event_set = {
    .set_size  = 0,
    .papi_set  = PAPI_NULL,
    .add_event = &event_set_add_event
};


int32_t
initialize()
{
    if ( !PAPI_is_initialized() )
    {
        PAPI_library_init( PAPI_VER_CURRENT );
        PAPI_multiplex_init();
    }

    const PAPI_hw_info_t* hwinfo = NULL;
    hwinfo = PAPI_get_hardware_info();

    if ( hwinfo == NULL )
    {
        return 1;
    }

    if ( BROADWELL( hwinfo->cpuid_family, hwinfo->cpuid_model ) )
    {
        counter = counter_broadwell;
    }
    else if ( HASWELL( hwinfo->cpuid_family, hwinfo->cpuid_model ) )
    {
        counter = counter_haswell;
    }
    else
    {
        return 1;
    }

    return 0;
}


static unsigned long int
get_thread_id()
{
    return syscall( SYS_gettid );
}

int32_t
add_counter( char* eventName )
{
    if ( event_set.papi_set == PAPI_NULL )
    {
        PAPI_CALL( thread_init( &get_thread_id ) );

        PAPI_CALL( create_eventset( &event_set.papi_set ) );

        for ( int i = 0; i < NUM_COUNTERS; ++i )
        {
            event_set.index_map[ i ] = -1;
        }
    }

    int num_available_metrics = sizeof( available_metrics ) / sizeof( available_metrics[ 0 ] );
    for ( int i = 0; i < num_available_metrics; ++i )
    {
        if ( strcmp( eventName, available_metrics[ i ]->properties.name ) == 0 )
        {
            available_metrics[ i ]->initialize( &event_set );
            ignored_requests[ i ] = 15;
            return i;
        }
    }

    return -1;
}


SCOREP_Metric_Plugin_MetricProperties*
get_event_info( char* eventName )
{
    SCOREP_Metric_Plugin_MetricProperties* return_values;

    int num_available_metrics = sizeof( available_metrics ) / sizeof( available_metrics[ 0 ] );

    /* If wildcard is specified, add all counters */
    if ( strcmp( eventName, "*" ) == 0 )
    {
        return_values = malloc( ( num_available_metrics + 1 ) * sizeof( SCOREP_Metric_Plugin_MetricProperties ) );
        for ( int i = 0; i < num_available_metrics; i++ )
        {
            return_values[ i ].name        = strdup( available_metrics[ i ]->properties.name );
            return_values[ i ].description = strdup( available_metrics[ i ]->properties.description );
            return_values[ i ].unit        = strdup( available_metrics[ i ]->properties.unit );
            return_values[ i ].mode        = available_metrics[ i ]->properties.mode;
            return_values[ i ].value_type  = available_metrics[ i ]->properties.value_type;
            return_values[ i ].base        = available_metrics[ i ]->properties.base;
            return_values[ i ].exponent    = available_metrics[ i ]->properties.exponent;
        }
        return_values[ num_available_metrics ].name = NULL;
    }
    else
    {
        return_values           = malloc( 2 * sizeof( SCOREP_Metric_Plugin_MetricProperties ) );
        return_values[ 0 ].name = NULL;
        for ( int i = 0; i < num_available_metrics; ++i )
        {
            if ( strcmp( eventName, available_metrics[ i ]->properties.name ) == 0 )
            {
                return_values[ 0 ].name        = strdup( available_metrics[ i ]->properties.name );
                return_values[ 0 ].description = strdup( available_metrics[ i ]->properties.description );
                return_values[ 0 ].unit        = strdup( available_metrics[ i ]->properties.unit );
                return_values[ 0 ].mode        = available_metrics[ i ]->properties.mode;
                return_values[ 0 ].value_type  = available_metrics[ i ]->properties.value_type;
                return_values[ 0 ].base        = available_metrics[ i ]->properties.base;
                return_values[ 0 ].exponent    = available_metrics[ i ]->properties.exponent;
            }
        }
        return_values[ 1 ].name = NULL;
    }

    return return_values;
}

bool
get_optional_value( int32_t   counterIndex,
                    uint64_t* value )
{
    if ( ignored_requests[ counterIndex ] > 0 )
    {
        ignored_requests[ counterIndex ]--;
        return false;
    }

    ignored_requests[ counterIndex ] = 15;

    return available_metrics[ counterIndex ]->get_value( &event_set, value );
}


void
finalize()
{
    PAPI_shutdown();
}


SCOREP_METRIC_PLUGIN_ENTRY( papiDerived )
{
    /* Initialize info data (with zero) */
    SCOREP_Metric_Plugin_Info info;
    memset( &info, 0, sizeof( SCOREP_Metric_Plugin_Info ) );
    /* Set up */
    info.plugin_version     = SCOREP_METRIC_PLUGIN_VERSION;
    info.run_per            = SCOREP_METRIC_PER_THREAD;
    info.sync               = SCOREP_METRIC_SYNC;
    info.initialize         = initialize;
    info.finalize           = finalize;
    info.get_event_info     = get_event_info;
    info.add_counter        = add_counter;
    info.get_optional_value = get_optional_value;
    return info;
}
