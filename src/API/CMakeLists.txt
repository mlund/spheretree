set(BOT_LIB_NAME API)
add_subdirectory(SFWhite)

file(GLOB_RECURSE LibSource ./*.cpp)
list(FILTER LibSource EXCLUDE REGEX ".*SFWhite/.*")

add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)

target_link_libraries(${BOT_LIB_NAME} SFWhite)
