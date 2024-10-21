cmake_minimum_required(VERSION 3.16)

# Define project
project(spheretree-dist VERSION 1.0 LANGUAGES C CXX)

# Set C/C++ standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define options
option(ENABLE_WARNINGS "Enable project warnings" ON)
option(ENABLE_SANITIZER "Enable address and undefined behavior sanitizer" OFF)

# Set up modules path and include necessary scripts from cmake_template
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include standard project settings
include(StandardProjectSettings)

# Add subdirectories
add_subdirectory(src/API)
add_subdirectory(src/Base)
add_subdirectory(src/BBox)
add_subdirectory(src/Exceptions)
add_subdirectory(src/Geometry)
add_subdirectory(src/LinearAlgebra)
add_subdirectory(src/MedialAxis)
add_subdirectory(src/MinMax)
add_subdirectory(src/SphereTree)
add_subdirectory(src/Storage)
add_subdirectory(src/Surface)
add_subdirectory(src/Export)

# Create STG library
add_library(STG STATIC src/null.c)

# Include compiler warnings module
include(CompilerWarnings)
if(ENABLE_WARNINGS)
    myproject_set_project_warnings(STG OFF "" "" "" "")
endif()

# Link dependent libraries to STG
set(LIBS_TO_LINK
    API
    Base
    BBox
    Exceptions
    Geometry
    Matrix
    MedialAxis
    MinMax
    SphereTree
    Storage
    Surface
    Export
)
target_link_libraries(STG PUBLIC ${LIBS_TO_LINK})


# Enable sanitizers
if(ENABLE_SANITIZER)
    include(Sanitizers)
    add_sanitizers(STG)
endif()

# Set common properties for executables
function(add_custom_executable EXEC_NAME)
    set(SOURCES)
    foreach(_ARG IN LISTS ARGN)
        list(APPEND SOURCES ${_ARG})
    endforeach()
    add_executable(${EXEC_NAME} ${SOURCES})
    target_link_libraries(${EXEC_NAME} PRIVATE STG)
    if(ENABLE_WARNINGS)
        myproject_set_project_warnings(${EXEC_NAME} OFF "" "" "" "")
    endif()
    if(ENABLE_SANITIZER)
        add_sanitizers(${EXEC_NAME})
    endif()
endfunction()

# Add executables
add_custom_executable(makeTreeMedial
    "src/makeTreeMedial.cpp"
    "src/DecodeParam.cpp"
    "src/EvalTree.cpp"
    "src/VerifyModel.cpp"
)

add_custom_executable(makeTreeHubbard
    "src/makeTreeHubbard.cpp"
    "src/DecodeParam.cpp"
    "src/VerifyModel.cpp"
)

add_custom_executable(makeTreeOctree
    "src/makeTreeOctree.cpp"
    "src/DecodeParam.cpp"
)

add_custom_executable(makeTreeGrid
    "src/makeTreeGrid.cpp"
    "src/DecodeParam.cpp"
    "src/VerifyModel.cpp"
    "src/EvalTree.cpp"
)

add_custom_executable(makeTreeSpawn
    "src/makeTreeSpawn.cpp"
    "src/DecodeParam.cpp"
    "src/VerifyModel.cpp"
    "src/EvalTree.cpp"
)

# Include project-specific and external dependencies
include(GNUInstallDirs)
install(TARGETS STG makeTreeMedial makeTreeHubbard makeTreeOctree makeTreeGrid makeTreeSpawn
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

