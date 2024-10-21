set(BOT_LIB_NAME Geometry)
file(GLOB_RECURSE LibSource ./*.cpp)
add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)
add_subdirectory(pcube)


target_link_libraries(${BOT_LIB_NAME} pcube Matrix)

