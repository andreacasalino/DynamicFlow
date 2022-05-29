cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

option(LIB_OPT "Compile shared libraries (ON) or static (OFF)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set macro-directory and find scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# include cmake custom functions
set(WITH_SOURCE_TREE ON)
include(GroupSources)
include(AutoCollect)
include(MakeLibrary)
include(GCov)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

add_subdirectory(src)

option(BUILD_DYN_FLOW_SAMPLES "Build the samples" ON)
if(BUILD_DYN_FLOW_SAMPLES)
    add_subdirectory(samples)
endif()

option(BUILD_DYN_FLOW_TESTS "" OFF)
if(BUILD_DYN_FLOW_TESTS)
    add_subdirectory(tests)
endif()

