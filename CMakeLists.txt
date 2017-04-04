cmake_minimum_required(VERSION 3.1)
project(papiDerived)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/common;${CMAKE_MODULE_PATH}")

find_package(PAPI REQUIRED)
find_package(Scorep REQUIRED)

set(PLUGIN_SOURCE papi_derived.c
                  metrics/ipc.c )

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${PAPI_INCLUDE_DIRS})
include_directories(${SCOREP_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCE})
target_link_libraries(${PROJECT_NAME} ${PAPI_LIB})

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
