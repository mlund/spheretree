set(BOT_LIB_NAME MedialAxis)
add_library(${BOT_LIB_NAME} STATIC
        Clusters.cpp MedialTester.cpp Voronoi3D.cpp VoronoiFace.cpp
)


target_link_libraries(${BOT_LIB_NAME})

