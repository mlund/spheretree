# Microsoft Developer Studio Project File - Name="SphereTree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SphereTree - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SphereTree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SphereTree.mak" CFG="SphereTree - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SphereTree - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SphereTree - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SphereTree - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Build/Release/"
# PROP Intermediate_Dir "../Build/Release/App"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /GX /Ot /Oa /Ow /Oi /Oy /Ob2 /I "c:\devel\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /libpath:"c:\devel\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SphereTree - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Build/Debug/"
# PROP Intermediate_Dir "../Build/Debug/APP"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /Zi /Od /Ob2 /I "c:\devel\inc" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DEBUG" /YX"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /debug /debugtype:both /machine:I386 /nodefaultlib:"libcmt" /libpath:"c:\devel\lib"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "SphereTree - Win32 Release"
# Name "SphereTree - Win32 Debug"
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MfcApp\res\idr_coll.ico
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\move.cur
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\OVtool.bmp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\Rotate.cur
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\rotate_3d.cur
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\SphereTree.rc2
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\res\zoom.cur
# End Source File
# End Group
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OpenGL\GLbitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGL\GLbitmap.h
# End Source File
# Begin Source File

SOURCE=.\OpenGL\GLsave.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGL\GLsave.h
# End Source File
# Begin Source File

SOURCE=.\OpenGL\GLsupport.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGL\GLsupport.h
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Image\Formats.cpp
# End Source File
# Begin Source File

SOURCE=.\Image\Formats.h
# End Source File
# Begin Source File

SOURCE=.\Image\ImageRGB.cpp
# End Source File
# Begin Source File

SOURCE=.\Image\ImageRGB.h
# End Source File
# Begin Source File

SOURCE=.\Image\IMsupport.cpp
# End Source File
# Begin Source File

SOURCE=.\Image\IMsupport.h
# End Source File
# End Group
# Begin Group "GL2PS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Gl2ps\gl2ps.cpp
# End Source File
# Begin Source File

SOURCE=.\Gl2ps\gl2ps.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MfcApp\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\CTimer.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\InputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\InputDialog.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\MainFrm.h
# End Source File
# Begin Source File

SOURCE=".\MfcApp\MFC-Util.cpp"
# End Source File
# Begin Source File

SOURCE=".\MfcApp\MFC-Util.h"
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ModelDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ModelDoc.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ModelView.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ModelView.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ObjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ObjectView.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\OutputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\OutputDialog.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ProgressMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\ProgressMeter.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\resource.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\SphereTree.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\SphereTree.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\SphereTree.rc
# End Source File
# Begin Source File

SOURCE=.\MfcApp\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\MfcApp\TraceOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\MfcApp\TraceOutput.h
# End Source File
# End Target
# End Project
