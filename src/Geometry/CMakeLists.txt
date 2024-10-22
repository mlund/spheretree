set(BOT_LIB_NAME Geometry)
add_library(${BOT_LIB_NAME} STATIC
         Circle.cpp  CubeTri.cpp   Line2D.cpp  LineSeg.cpp  Plane.cpp  Point2D.cpp  Point3D.cpp  Sphere.cpp  Transform2D.cpp  Transform3D.cpp  TriTri.cpp  Vector2D.cpp  Vector3D.cpp  RayBox.c  Ray.cpp 
)
add_subdirectory(pcube)


target_link_libraries(${BOT_LIB_NAME} pcube Matrix)

