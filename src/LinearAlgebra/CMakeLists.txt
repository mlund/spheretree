set(BOT_LIB_NAME Matrix)
add_library(${BOT_LIB_NAME} STATIC
        Matrix.cpp
)


target_link_libraries(${BOT_LIB_NAME})
