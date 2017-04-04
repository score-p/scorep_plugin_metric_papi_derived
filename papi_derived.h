#ifndef PAPI_DREIVED_H
#define PAPI_DREIVED_H

#include <scorep/SCOREP_MetricPlugins.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

#define PAPI_CALL( CALL ) \
    do { \
        int ret = PAPI_ ## CALL; \
        if ( ret != PAPI_OK ) \
        { \
            fprintf( stderr, "PAPI_error %d: %s\n", ret, PAPI_strerror( ret ) ); \
            abort(); \
        } \
    } while ( 0 )

enum CounterDefs
{
    CYCLES       = 0,
    INSTRUCTIONS = 1,

    NUM_COUNTERS
};

static char**    counter        = NULL;

#define BROADWELL( FAMILY, MODEL ) \
    ( FAMILY == 0X6 && ( MODEL == 0X3D || MODEL == 0X47 || MODEL == 0X4F || MODEL == 0X56 ) )

static char* counter_broadwell[ NUM_COUNTERS ] = {
    [ CYCLES ]       = "CPU_CLK_THREAD_UNHALTED:THREAD_P",
    [ INSTRUCTIONS ] = "INST_RETIRED:ANY_P",
};

#define HASWELL( FAMILY, MODEL ) \
    ( FAMILY == 0X6 && ( MODEL == 0X3C || MODEL == 0X3F || MODEL == 0X45 || MODEL == 0X46 ) )

static char* counter_haswell[ NUM_COUNTERS ] = {
    [ CYCLES ]       = "CPU_CLK_THREAD_UNHALTED:THREAD_P",
    [ INSTRUCTIONS ] = "INST_RETIRED:ANY_P",
};

#define IVY_BRIDGE( FAMILY, MODEL ) \
    ( FAMILY == 0X6 && ( MODEL == 0X3A || MODEL == 0X3E ) )

static char* counter_ivy_bridge[ NUM_COUNTERS ] = {};

#define SANDY_BRIDGE( FAMILY, MODEL ) \
    ( FAMILY == 0X6 && ( MODEL == 0X2A || MODEL == 0X2D ) )

static char* counter_sandy_bridge[ NUM_COUNTERS ] = {};


typedef union
{
    uint64_t u64;
    int64_t  i64;
    double   dbl;
} ValueType_t;


typedef struct event_set_t EventSet_t;
typedef void (* AddEventFnPtr_t)( EventSet_t*,
                                  int );
struct event_set_t
{
    ssize_t         set_size;
    ssize_t         index_map[ NUM_COUNTERS ];
    int             papi_set;

    AddEventFnPtr_t add_event;
};

#endif /* end of include guard: PAPI_DREIVED_H */
