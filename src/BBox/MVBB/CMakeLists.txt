set(BOT_LIB_NAME MVBB)

add_library(${BOT_LIB_NAME} STATIC
        gdiam.cpp
)


target_link_libraries(${BOT_LIB_NAME})

