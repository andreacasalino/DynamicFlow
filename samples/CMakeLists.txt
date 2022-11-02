include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/make-sample.cmake)

set(SAMPLES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# add_subdirectory(Sample-README) # only used to produce the documentation

add_subdirectory(Sample-01-big-vector-samples)
add_subdirectory(Sample-02-long-text-process)
add_subdirectory(Sample-03-concurrent-update-read)
add_subdirectory(Sample-04-collisions-detection)
