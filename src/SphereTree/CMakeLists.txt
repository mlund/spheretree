set(BOT_LIB_NAME SphereTree)
file(GLOB_RECURSE LibSource ./*.cpp)
add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)


target_link_libraries(${BOT_LIB_NAME} Geometry Storage)

