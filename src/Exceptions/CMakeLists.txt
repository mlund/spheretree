set(BOT_LIB_NAME Exceptions)

add_library(${BOT_LIB_NAME} STATIC
        ErrorLog.cpp
        ErrorLog.h
        MemoryException.cpp
        MemoryException.h
        Exception.cpp
        Exception.h
        CheckAlways.h
        CheckDebug.h
        CheckMemory.h
        ValidPointer.h
        ErrorsInc.h
)
set(EXTRA_DIST
        CallStack.cpp
        CallStack.h
)


target_link_libraries(${BOT_LIB_NAME})

