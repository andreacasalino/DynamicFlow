set(PROJECT_SHORTNAME "DynamicFlow")

MakeLibrary(${PROJECT_SHORTNAME} header)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_SHORTNAME}
PUBLIC
	${CMAKE_THREAD_LIBS_INIT}     
PRIVATE
	OpenMP::OpenMP_CXX
)

option(DYNAMIC_FLOW_ENABLE_JSON_EXPORT "Enable export to json for DynamicFlow" OFF)
if(DYNAMIC_FLOW_ENABLE_JSON_EXPORT)
	FECTH_JSON_LIB()
	target_link_libraries(${PROJECT_SHORTNAME} PUBLIC nlohmann_json::nlohmann_json)
	target_compile_definitions(${PROJECT_SHORTNAME} PUBLIC DYNAMIC_FLOW_ENABLE_JSON_EXPORT)
endif()
