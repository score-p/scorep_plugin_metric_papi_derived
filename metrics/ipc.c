#include "metrics.h"

static __thread int64_t previous_values[ 2 ];

static void
initialize( EventSet_t* eventSet )
{
    long long values[ NUM_COUNTERS ];

    int event_set_state;
    PAPI_CALL( state( eventSet->papi_set, &event_set_state ) );
    if ( event_set_state & PAPI_RUNNING )
    {
        PAPI_CALL( stop( eventSet->papi_set, values ) );
    }

    int counters[ 2 ] = { INSTRUCTIONS, CYCLES };
    for ( int i = 0; i < 2; ++i )
    {
        eventSet->add_event( eventSet, counters[ i ] );
    }

    PAPI_CALL( start( eventSet->papi_set ) );
    PAPI_CALL( read( eventSet->papi_set, values ) );

    previous_values[ 0 ] = values[ eventSet->index_map[ INSTRUCTIONS ] ];
    previous_values[ 1 ] = values[ eventSet->index_map[ CYCLES ] ];
}

static bool
get_value( EventSet_t* eventSet, uint64_t* value )
{
    long long values[ NUM_COUNTERS ] = { 0 };
    PAPI_CALL( read( eventSet->papi_set, values ) );

    int64_t val_instructions = values[ eventSet->index_map[ INSTRUCTIONS ] ];
    int64_t val_cycles       = values[ eventSet->index_map[ CYCLES ] ];

    int64_t instructions = val_instructions - previous_values[ 0 ];
    int64_t cycles       = val_cycles - previous_values[ 1 ];

    if ( instructions <= 0
         || cycles <= 0 )
    {
        return false;
    }

    previous_values[ 0 ] = val_instructions;
    previous_values[ 1 ] = val_cycles;

    ValueType_t ret;
    ret.dbl = instructions / ( double )cycles;
    *value  = ret.u64;

    return true;
}

Metric_t papi_derived_ipc =
{
    .properties      = {
        .name        = "IPC",
        .description = "The average number of instructions executed for each clock cycle.",
        .unit        = "1/s",
        .mode        = SCOREP_METRIC_MODE_ABSOLUTE_LAST,
        .value_type  = SCOREP_METRIC_VALUE_DOUBLE,
        .base        = SCOREP_METRIC_BASE_DECIMAL,
        .exponent    = 0
    },
    .initialize = &initialize,
    .get_value  = &get_value
};
