set(BOT_LIB_NAME pcube)
add_library(${BOT_LIB_NAME} STATIC
        pcube.cpp fpcube.cpp
)

target_link_libraries(${BOT_LIB_NAME})

