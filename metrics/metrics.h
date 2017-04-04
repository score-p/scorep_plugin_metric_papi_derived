#ifndef METRICS_H
#define METRICS_H

#include <scorep/SCOREP_MetricPlugins.h>
#include <papi_derived.h>

typedef struct metric_t Metric_t;
typedef void (* InitMetricFnPtr_t)( EventSet_t* );
typedef bool (* GetValueFnPtr_t)( EventSet_t*, uint64_t* );
struct metric_t
{
    SCOREP_Metric_Plugin_MetricProperties properties;
    InitMetricFnPtr_t                     initialize;
    GetValueFnPtr_t                       get_value;
};


/*
 * Available Metrics
 */

#define PAPI_DERIVED_METRIC( NAME ) \
    extern Metric_t papi_derived_ ## NAME

PAPI_DERIVED_METRIC( ipc );

#undef PAPI_DERIVED_METRIC

#endif /* end of include guard: METRICS_H */
