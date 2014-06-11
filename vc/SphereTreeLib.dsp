# Microsoft Developer Studio Project File - Name="SphereTreeLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SphereTreeLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SphereTreeLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SphereTreeLib.mak" CFG="SphereTreeLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SphereTreeLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SphereTreeLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SphereTreeLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Build/Release/Lib"
# PROP Intermediate_Dir "../Build/Release/Lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1809 /d "NDEBUG"
# ADD RSC /l 0x1809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SphereTreeLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Build/Debug/Lib"
# PROP Intermediate_Dir "../Build/Debug/Lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gm /GX /Zi /Ob2 /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x1809 /d "_DEBUG"
# ADD RSC /l 0x1809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SphereTreeLib - Win32 Release"
# Name "SphereTreeLib - Win32 Debug"
# Begin Group "API"

# PROP Default_Filter ""
# Begin Group "MeshSample"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\DomainSampler.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\DomainSampler.h
# End Source File
# Begin Source File

SOURCE=..\src\API\MSBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\MSBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\MSGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\MSGrid.h
# End Source File
# Begin Source File

SOURCE=..\src\API\MSRelaxation.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\MSRelaxation.h
# End Source File
# End Group
# Begin Group "Hubbard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\MergeHubbard.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\MergeHubbard.h
# End Source File
# Begin Source File

SOURCE=..\src\API\STGHubbard.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\STGHubbard.h
# End Source File
# End Group
# Begin Group "Redundency Eliminator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\REBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\REBase.h
# End Source File
# Begin Source File

SOURCE=..\src\Api\REDiscard.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\REDiscard.h
# End Source File
# Begin Source File

SOURCE=..\src\API\RELargest.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\RELargest.h
# End Source File
# Begin Source File

SOURCE=..\src\Api\RELargestLM.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\RELargestLM.h
# End Source File
# Begin Source File

SOURCE=..\src\API\REMaxElim.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\REMaxElim.h
# End Source File
# Begin Source File

SOURCE=..\src\Api\RESelect.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\RESelect.h
# End Source File
# End Group
# Begin Group "Sphere Evaluator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\SEBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SEConvex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SEConvex.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SEPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SEPoint.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SESphPt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SESphPt.h
# End Source File
# End Group
# Begin Group "Sphere Fitter"

# PROP Default_Filter ""
# Begin Group "SFWhite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\SFWhite\ball.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\ball.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\complex.c
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\complex.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\nrutil.c
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\nrutil.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\svd.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite\svd.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\API\SFBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFMinErr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFMinErr.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFRitter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFRitter.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SFWhite.h
# End Source File
# End Group
# Begin Group "Sphere Reducer/Generator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\SRBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRBurst.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRBurst.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRComposite.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRComposite.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRExpand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRExpand.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRGrid.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRMerge.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRMerge.h
# End Source File
# Begin Source File

SOURCE=..\src\Api\SRQuickBurst.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\SRQuickBurst.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRSpawn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRSpawn.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRStandOff.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRStandOff.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SRVoronoi.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SRVoronoi.h
# End Source File
# End Group
# Begin Group "Sphere Sampler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\SSBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SSIsohedron.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SSIsohedron.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SSRings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SSRings.h
# End Source File
# End Group
# Begin Group "Sphere Tree Generator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Api\Hybrid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\Hybrid.h
# End Source File
# Begin Source File

SOURCE=..\src\API\STGBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\STGGeneric.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\STGGeneric.h
# End Source File
# Begin Source File

SOURCE=..\src\API\STGOctree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\STGOctree.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SurfaceDiv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SurfaceDiv.h
# End Source File
# End Group
# Begin Group "Optimiser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\SOBalance.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SOBalance.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SOBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SOBase.h
# End Source File
# Begin Source File

SOURCE=..\src\Api\SOPerSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Api\SOPerSphere.h
# End Source File
# Begin Source File

SOURCE=..\src\API\SOSimplex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\SOSimplex.h
# End Source File
# End Group
# Begin Group "Voronoi Fix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\API\VFAdaptive.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\VFAdaptive.h
# End Source File
# Begin Source File

SOURCE=..\src\API\VFBase.h
# End Source File
# Begin Source File

SOURCE=..\src\API\VFGapCross.cpp
# End Source File
# Begin Source File

SOURCE=..\src\API\VFGapCross.h
# End Source File
# End Group
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Base\Defs.h
# End Source File
# Begin Source File

SOURCE=..\src\Base\ProcMon.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Base\Types.h
# End Source File
# End Group
# Begin Group "BBox"

# PROP Default_Filter ""
# Begin Group "MVBB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\BBox\MVBB\gdiam.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\Mvbb\gdiam.h
# End Source File
# End Group
# Begin Group "qHull"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\BBox\qHull\geom.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\geom.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\geom2.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\global.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\mem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\mem.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\merge.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\merge.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\poly.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\poly.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\poly2.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\qhull.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\qhull.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\qhull_a.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\set.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\set.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\stat.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\stat.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\user.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\qHull\user.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\BBox\BBox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\BBox.h
# End Source File
# Begin Source File

SOURCE=..\src\BBox\eigen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BBox\eigen.h
# End Source File
# End Group
# Begin Group "Exceptions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Exceptions\CallStack.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\CallStack.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\CheckAlways.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\CheckDebug.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\CheckMemory.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\ErrorLog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\ErrorLog.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\ErrorsInc.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\Exception.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\Exception.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\MemoryException.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\MemoryException.h
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\ValidPointer.h
# End Source File
# End Group
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Group "pcube"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Geometry\pcube\fpcube.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\pcube\pcube.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\pcube\pcube.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\pcube\vec.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\Geometry\Angles.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Circle.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Circle.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\CubeTri.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\CubeTri.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Geometry.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Line2D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Line2D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\LineSeg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\LineSeg.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Plane.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Plane.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Point2D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Point2D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Point3D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Point3D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Ray.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Ray.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\RayBox.c
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\RayBox.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Sphere.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Sphere.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Transform2D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Transform2D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Transform3D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Transform3D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\TriTri.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\TriTri.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Vector2D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Vector2D.h
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Vector3D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Geometry\Vector3D.h
# End Source File
# End Group
# Begin Group "LinearAlgebra"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\LinearAlgebra\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\src\LinearAlgebra\Matrix.h
# End Source File
# End Group
# Begin Group "MedialAxis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\MedialAxis\Clusters.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\Clusters.h
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\MedialTester.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\MedialTester.h
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\Voronoi3D.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\Voronoi3D.h
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\VoronoiFace.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MedialAxis\VoronoiFace.h
# End Source File
# End Group
# Begin Group "MinMax"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\MinMax\Simplex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MinMax\Simplex.h
# End Source File
# End Group
# Begin Group "SphereTree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\SphereTree\SphereTree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SphereTree\SphereTree.h
# End Source File
# End Group
# Begin Group "Storage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Storage\Array.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\Array2D.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\FreeArray.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\kTree.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\LinkedList.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\Queue.h
# End Source File
# Begin Source File

SOURCE=..\src\Storage\Storage.h
# End Source File
# End Group
# Begin Group "Surface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Surface\Internal.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\Internal.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\InternalGem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\InternalGem.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\OBJLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\OBJLoader.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SpacialHash.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SpacialHash.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\Surface.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\Surface.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SurfaceRep.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SurfaceRep.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SurfaceTester.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\SurfaceTester.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\VolInt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\VolInt.h
# End Source File
# Begin Source File

SOURCE=..\src\Surface\Voxel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Surface\Voxel.h
# End Source File
# End Group
# Begin Group "Export"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Export\POV.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Export\POV.h
# End Source File
# Begin Source File

SOURCE=..\src\Export\VRML.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Export\VRML.h
# End Source File
# End Group
# End Target
# End Project
