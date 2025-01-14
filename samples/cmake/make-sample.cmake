function(MAKE_SAMPLE)
	get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

	set(COMPLETE_NAME DynamicFlow-${SAMPLE_NAME})

	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
		file(GLOB SAMPLE_SRC_FILES 
			${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
			${CMAKE_CURRENT_SOURCE_DIR}/src/*.h 
			${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
		)
		add_executable(${COMPLETE_NAME} ${SAMPLE_SRC_FILES})
	else()
		add_executable(${COMPLETE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
	endif()

	target_link_libraries(${COMPLETE_NAME} PUBLIC 
		DynamicFlow
		DynamicFlow-Sample-utils
	)

	target_include_directories(${COMPLETE_NAME} PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src/
	)

	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
		target_compile_definitions(${COMPLETE_NAME}
		PUBLIC
			-DASSET_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets"
		)
	endif()

	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/script)
		target_compile_definitions(${COMPLETE_NAME}
		PUBLIC
			-DSCRIPT_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/script"
		)
	endif()
endfunction()
