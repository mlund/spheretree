cmake_minimum_required(VERSION 3.10)
project(spheretree-dist VERSION 1.0 LANGUAGES CXX C)

# Set the required C/C++ standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-int-in-bool-context -Wsign-compare")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()


SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -fPIC -DNDEBUG")

# Include all subdirectories that are part of the project
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

# Add the static library equivalent to libSTG.la
add_library(STG STATIC
        src/null.c
)

# Link dependent libraries to libSTG
target_link_libraries(STG
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

# Define the list of programs to be built
set(PROGRAMS
        makeTreeMedial
        makeTreeHubbard
        makeTreeOctree
        makeTreeGrid
        makeTreeSpawn
)

# Add each program and its respective source files
foreach(PROGRAM ${PROGRAMS})
    if(${PROGRAM} STREQUAL "makeTreeMedial")
        add_executable(${PROGRAM}
                src/makeTreeMedial.cpp
                src/DecodeParam.cpp
                src/DecodeParam.h
                src/EvalTree.cpp
                src/EvalTree.h
                src/VerifyModel.cpp
                src/VerifyModel.h
        )
    elseif(${PROGRAM} STREQUAL "makeTreeHubbard")
        add_executable(${PROGRAM}
                src/makeTreeHubbard.cpp
                src/VerifyModel.cpp
                src/VerifyModel.h
                src/DecodeParam.cpp
                src/DecodeParam.h
        )
    elseif(${PROGRAM} STREQUAL "makeTreeOctree")
        add_executable(${PROGRAM}
                src/makeTreeOctree.cpp
                src/DecodeParam.cpp
                src/DecodeParam.h
        )
    elseif(${PROGRAM} STREQUAL "makeTreeGrid")
        add_executable(${PROGRAM}
                src/makeTreeGrid.cpp
                src/VerifyModel.cpp
                src/VerifyModel.h
                src/EvalTree.cpp
                src/EvalTree.h
                src/DecodeParam.cpp
                src/DecodeParam.h
        )
    elseif(${PROGRAM} STREQUAL "makeTreeSpawn")
        add_executable(${PROGRAM}
                src/makeTreeSpawn.cpp
                src/VerifyModel.cpp
                src/VerifyModel.h
                src/EvalTree.cpp
                src/EvalTree.h
                src/DecodeParam.cpp
                src/DecodeParam.h
        )
    endif()

    # Link the libSTG static library to each executable
    target_link_libraries(${PROGRAM} STG)
endforeach()

# Installation rules for executables
install(TARGETS ${PROGRAMS} RUNTIME DESTINATION bin)
