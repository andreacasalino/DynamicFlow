if (WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (WIN32)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(TEST_NAME DynamicFlowTests)

file(GLOB TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${TEST_NAME} ${TEST_SRC})
target_link_libraries(${TEST_NAME}
PUBLIC
	gtest
	gtest_main
	DynamicFlow
)

# target_compile_definitions(${TEST_NAME} 
# PUBLIC
	# -D TEST_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/"
	# # -D SAMPLE_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/../../Samples/"
# )

install(TARGETS ${TEST_NAME})
