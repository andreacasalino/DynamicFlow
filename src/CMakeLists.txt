set(PROJECT_SHORTNAME "DynamicFlow")

MakeLibrary(${PROJECT_SHORTNAME} header)

find_package(Threads)
target_link_libraries(${PROJECT_SHORTNAME}
    PUBLIC
	${CMAKE_THREAD_LIBS_INIT}      
)
