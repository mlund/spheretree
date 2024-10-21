set(BOT_LIB_NAME Storage)
file(GLOB_RECURSE LibSource ./*.cpp ./*h )
add_library(${BOT_LIB_NAME} STATIC
        ${LibSource}
)


target_link_libraries(${BOT_LIB_NAME})

