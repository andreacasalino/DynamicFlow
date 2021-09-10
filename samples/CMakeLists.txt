function(MAKE_SAMPLE NAME)
	add_executable(${NAME} ${NAME}.cpp)

	target_link_libraries(${NAME}
	PUBLIC
		DynamicFlow
	)
	
	install(TARGETS ${NAME})
endfunction()

file(GLOB SAMPLES [RELATIVE path] *.cpp)
foreach(sample ${SAMPLES})
	get_filename_component(SAMPLE_NAME_EXTENSION ${sample} NAME [CACHE])
	get_filename_component(SAMPLE_NAME ${SAMPLE_NAME_EXTENSION} NAME_WE [CACHE])
	message(STATUS "Creating sample named: ${SAMPLE_NAME}")
	MAKE_SAMPLE(${SAMPLE_NAME})
endforeach()
