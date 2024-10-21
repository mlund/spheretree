set(BOT_LIB_NAME BBox)
add_subdirectory(MVBB)
add_subdirectory(qHull)

file(GLOB_RECURSE LibSource ./*.cpp)
list(FILTER LibSource EXCLUDE REGEX ".*MVBB/.*")
list(FILTER LibSource EXCLUDE REGEX ".*qHull/.*")

add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)


target_link_libraries(${BOT_LIB_NAME} qHull MVBB)

