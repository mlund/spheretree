set(BOT_LIB_NAME SphereTree)

add_library(${BOT_LIB_NAME} STATIC
        SphereTree.cpp
)


target_link_libraries(${BOT_LIB_NAME} Geometry Storage)

