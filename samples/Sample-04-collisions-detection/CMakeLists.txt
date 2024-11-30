MAKE_SAMPLE()

# external library for collision detection
message("fetching gjk-epa")
set(BUILD_GJK_EPA_SAMPLES OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
    gjk_epa
    GIT_REPOSITORY https://github.com/andreacasalino/Flexible-GJK-and-EPA
    GIT_TAG        8166dfb30c3fbaaa22d362651f25210e29a0c715
)
FetchContent_MakeAvailable(gjk_epa)

FECTH_JSON_LIB()

target_link_libraries(DynamicFlow-Sample-04-collisions-detection PUBLIC 
    GJK-EPA
    nlohmann_json::nlohmann_json
)

