file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(LIB_NAME DynamicFlow-Sample-utils)

add_library(${LIB_NAME} ${SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
target_compile_definitions(${LIB_NAME} PUBLIC
-DPYTHON_CMD="${Python3_EXECUTABLE}"
-DLOG_DIR="${CMAKE_CURRENT_BINARY_DIR}/log"
-DDEFAULT_SCRIPT="${CMAKE_CURRENT_SOURCE_DIR}/ShowGraph.py"
)
target_link_libraries(${LIB_NAME} PUBLIC DynamicFlow)
