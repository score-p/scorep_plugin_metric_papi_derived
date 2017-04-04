# PAPI derived
This is a synchronous plugin metric for measuring derived PAPI performance counters.

## Compilation and Installation

### Prerequisites

To compile this plugin, you need:

* CMake

* PAPI (`5.2+`)

* Score-P

### Building

1. Create a build directory

        mkdir build
        cd build

2. Invoke CMake

    Specify the Score-P and PAPI directory if it is not in the default path with
    `-DSCOREP_DIR=<PATH>` respectivly `-DPAPI_INC=<PATH>`. The plugin will
    use alternatively the environment variables `SCOREP_DIR` and  `PAPI_INC`, e.g.

        cmake .. -DSCOREP_DIR=/opt/scorep -DPAPI_INC=/opt/papi/inc

3. Invoke make

        make

4. Copy it to a location listed in `LD_LIBRARY_PATH` or add current path to `LD_LIBRARY_PATH` with

        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`

## Usage

To use this plugin, add it to the `SCOREP_METRIC_PLUGINS` environment variable, e.g.:

    export SCOREP_METRIC_PLUGINS="papiDerived"

This plugin defines custom counters derived from papi native ones. To use them, simply set the
`SCOREP_METRIC_PAPIDERIVED` environment variable, e.g.

    export SCOREP_METRIC_PAPIDERIVED="IPC"
