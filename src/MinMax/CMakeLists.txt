set(BOT_LIB_NAME MinMax)

add_library(${BOT_LIB_NAME} STATIC
        Simplex.cpp
)


target_link_libraries(${BOT_LIB_NAME})

