set(BOT_LIB_NAME Storage)

add_library(${BOT_LIB_NAME} STATIC
        Array.h Array2D.h FreeArray.h LinkedList.h Queue.h Storage.h kTree.h null.cpp
)


target_link_libraries(${BOT_LIB_NAME})

