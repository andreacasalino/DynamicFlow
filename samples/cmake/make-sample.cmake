function(MAKE_SAMPLE ADD_PYTHON_VIZ)
	get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

	file(GLOB SAMPLE_SRC_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.h 
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	)
	if(ADD_PYTHON_VIZ)
		list(APPEND SAMPLE_SRC_FILES 
			${SAMPLES_ROOT}/python/RunScript.h
			${SAMPLES_ROOT}/python/RunScript.cpp
		)
	endif()

	set(COMPLETE_NAME DynamicFlow-${SAMPLE_NAME})

	add_executable(${COMPLETE_NAME} ${SAMPLE_SRC_FILES})

	target_link_libraries(${COMPLETE_NAME} PUBLIC DynamicFlow)

	target_include_directories(${COMPLETE_NAME} PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src/
		${SAMPLES_ROOT}/python/
	)

	target_compile_definitions(${COMPLETE_NAME}
	PUBLIC
    	-DSAMPLE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/"
	)

	if(ADD_PYTHON_VIZ)	
		target_compile_definitions(${COMPLETE_NAME}
		PUBLIC
			-DSHOW_GRAPH_PYTHON="${SAMPLES_ROOT}/python/ShowGraph.py"
		)
	endif()

	install(TARGETS ${COMPLETE_NAME})
endfunction()
