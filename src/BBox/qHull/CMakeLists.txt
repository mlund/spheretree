set(BOT_LIB_NAME qHull)

add_library(${BOT_LIB_NAME} STATIC
        geom.cpp  geom2.cpp global.cpp mem.cpp  merge.cpp  poly.cpp  poly2.cpp qhull.cpp   set.cpp  stat.cpp  user.cpp 
)

target_link_libraries(${BOT_LIB_NAME})
