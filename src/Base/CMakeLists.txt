set(BOT_LIB_NAME Base)

add_library(${BOT_LIB_NAME} STATIC
          ProcMon.cpp 
)


target_link_libraries(${BOT_LIB_NAME})

