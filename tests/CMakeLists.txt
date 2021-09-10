if (WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (WIN32)
add_subdirectory(GoogleTests)

function(MAKE_TEST NAME)
	add_executable(${NAME} ${NAME}.cpp)

	target_link_libraries(${NAME}
	PUBLIC
		gtest
		DynamicFlow
	)

	# target_compile_definitions(${NAME} 
	# PUBLIC
		# -D TEST_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/"
		# # -D SAMPLE_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/../../Samples/"
	# )

	install(TARGETS ${NAME})
endfunction()

file(GLOB TESTS [RELATIVE path] ./*.cpp)
foreach(test ${TESTS})
	get_filename_component(TEST_NAME_EXTENSION ${test} NAME [CACHE])
	get_filename_component(TEST_NAME ${TEST_NAME_EXTENSION} NAME_WE [CACHE])
	message(STATUS "Creating test named: ${TEST_NAME}")
	MAKE_TEST(${TEST_NAME})
endforeach()
