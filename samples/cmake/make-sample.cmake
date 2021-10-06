function(MAKE_SAMPLE NAME)
	file(GLOB SAMPLE_SRC_FILES ./src/*.cpp ./src/*.h ./*.cpp)

	add_executable(${NAME} ${SAMPLE_SRC_FILES})

	target_link_libraries(${NAME}
	PUBLIC
		DynamicFlow
	)

	target_include_directories(${NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src/
	)

	install(TARGETS ${NAME})
endfunction()
