set(BOT_LIB_NAME MVBB)
file(GLOB_RECURSE LibSource ./*.cpp)
list(FILTER LibSource EXCLUDE REGEX ".*origbits/.*")
add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)


target_link_libraries(${BOT_LIB_NAME})

