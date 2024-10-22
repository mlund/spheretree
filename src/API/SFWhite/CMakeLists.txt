set(BOT_LIB_NAME SFWhite)

add_library(${BOT_LIB_NAME} STATIC
        ball.cpp ball.h complex.c complex.h nrutil.c nrutil.h svd.cpp svd.h
)

target_link_libraries(${BOT_LIB_NAME})

