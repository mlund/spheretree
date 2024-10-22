set(BOT_LIB_NAME Export)

add_library(${BOT_LIB_NAME} STATIC
        VRML.cpp POV.cpp
)

target_link_libraries(${BOT_LIB_NAME})

