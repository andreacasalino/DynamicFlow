cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

option(LIB_OPT "Compile shared libraries (ON) or static (OFF)" OFF)
option(BUILD_SAMPLES "Build the samples" ON)
option(BUILD_TESTS "" OFF)

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

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

project(Flow-Core)
add_subdirectory(src)

# if(BUILD_SAMPLES)
	# project(Flow-Samples)
    # add_subdirectory(samples)
# endif()

if(BUILD_TESTS)
	project(Flow-Tests)
    add_subdirectory(tests)
endif()

