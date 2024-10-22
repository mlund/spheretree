set(BOT_LIB_NAME Surface)

add_library(${BOT_LIB_NAME} STATIC
         Surface.cpp  SpacialHash.cpp  Internal.cpp  Voxel.cpp  SurfaceTester.cpp VolInt.cpp  SurfaceRep.cpp   InternalGem.cpp  OBJLoader.cpp
)


target_link_libraries(${BOT_LIB_NAME} Geometry Storage Base)

