; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "spheretree.h"
LastPage=0

ClassCount=10
Class1=CChildFrame
Class2=CInputDialog
Class3=CMainFrame
Class4=CModelDoc
Class5=CModelView
Class6=CObjectView
Class7=COutputDialog
Class8=CProgressMeter
Class9=CSphereTreeApp
Class10=CAboutDlg

ResourceCount=8
Resource1=IDD_INPUT (English (Ireland))
Resource2=IDR_MODELTYPE_SUR
Resource3=IDR_MODELTYPE_3DM
Resource4=IDR_MODELTYPE_OBJ
Resource5=IDR_MAINFRAME
Resource6=IDD_ABOUTBOX
Resource7=IDD_PROGRESS_METER (English (Ireland))
Resource8=IDD_OUTPUTDIALOG (English (Ireland))

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CInputDialog]
Type=0
BaseClass=CDialog
HeaderFile=InputDialog.h
ImplementationFile=InputDialog.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CModelDoc]
Type=0
BaseClass=CDocument
HeaderFile=ModelDoc.h
ImplementationFile=ModelDoc.cpp

[CLS:CModelView]
Type=0
BaseClass=CObjectView
HeaderFile=ModelView.h
ImplementationFile=ModelView.cpp
Filter=C
VirtualFilter=VWC

[CLS:CObjectView]
Type=0
BaseClass=CView
HeaderFile=ObjectView.h
ImplementationFile=ObjectView.cpp

[CLS:COutputDialog]
Type=0
BaseClass=CDialog
HeaderFile=OutputDialog.h
ImplementationFile=OutputDialog.cpp

[CLS:CProgressMeter]
Type=0
BaseClass=CDialog
HeaderFile=ProgressMeter.h
ImplementationFile=ProgressMeter.cpp

[CLS:CSphereTreeApp]
Type=0
BaseClass=CWinApp
HeaderFile=SphereTree.h
ImplementationFile=SphereTree.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=SphereTree.cpp
ImplementationFile=SphereTree.cpp
LastObject=CAboutDlg

[DLG:IDD_INPUT]
Type=1
Class=CInputDialog

[DLG:IDD_OUTPUTDIALOG]
Type=1
Class=COutputDialog

[DLG:IDD_PROGRESS_METER]
Type=1
Class=CProgressMeter

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_VIEW_ROTATEMODE
Command3=ID_VIEW_PANMODE
Command4=ID_VIEW_ZOOMMODE
Command5=ID_MODE_POINT
Command6=ID_VIEW_LIGHTINGANGLE
Command7=ID_VIEW_RESETVIEW
Command8=ID_SPHERETREE_PREV
Command9=ID_SPHERETREE_NEXT
Command10=ID_APP_ABOUT
CommandCount=10

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_VIEW_TOOLBAR
Command5=ID_VIEW_STATUS_BAR
Command6=ID_APP_ABOUT
CommandCount=6

[MNU:IDR_MODELTYPE_SUR]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_MODEL_SAVE
Command4=ID_MODEL_SAVEOBJ
Command5=ID_SPHERES_LOAD
Command6=ID_SPHERES_SAVE
Command7=ID_SPHERES_EXPORT_POV
Command8=ID_SPHERES_EXPORT_VRML
Command9=ID_FILE_SAVEIMAGE
Command10=ID_FILE_SAVEIMAGEPS
Command11=ID_FILE_MRU_FILE1
Command12=ID_APP_EXIT
Command13=ID_VIEW_ROTATEMODE
Command14=ID_VIEW_PANMODE
Command15=ID_VIEW_ZOOMMODE
Command16=ID_VIEW_LIGHTINGANGLE
Command17=ID_MODE_POINT
Command18=ID_VIEW_RESETVIEW
Command19=ID_VIEW_TOOLBAR
Command20=ID_VIEW_STATUS_BAR
Command21=ID_SHOW_MODEL
Command22=ID_SHOW_SAMPLES
Command23=ID_SHOW_SPHERES
Command24=ID_SPHERETREE_NONE
Command25=ID_SPHERETREE_TOP
Command26=ID_SPHERETREE_PREV
Command27=ID_SPHERETREE_NEXT
Command28=ID_GENERATE_SAMPLES
Command29=ID_VALIDATE_MESH
Command30=ID_GENERATE_STATIC_MA
Command31=ID_GENERATE_ADAPTIVE_MA
Command32=ID_GENERATE_IMPROVE_MA
Command33=ID_REDUCE_HUBBARD
Command34=ID_REDUCE_MERGE
Command35=ID_REDUCE_BURST
Command36=ID_REDUCE_EXPAND
Command37=ID_OPTIMISE_PERSPHERE
Command38=ID_OPTIMISE_SIMPLEX
Command39=ID_OPTIMISE_BALANCE
Command40=ID_GENTREE_OCTREE
Command41=ID_GENTREE_HUBBARD
Command42=ID_GENTREE_MERGE
Command43=ID_GENTREE_BURST
Command44=ID_GENTREE_EXPAND
Command45=ID_GENTREE_COMBINED
Command46=ID_GENTREE_SPAWN
Command47=ID_GENTREE_GRID
Command48=ID_USE_CONVEX_TESTER
Command49=ID_EVALUATE_SPHERES
Command50=ID_WINDOW_NEW
Command51=ID_WINDOW_CASCADE
Command52=ID_WINDOW_TILE_HORZ
Command53=ID_WINDOW_ARRANGE
Command54=ID_APP_ABOUT
CommandCount=54

[MNU:IDR_MODELTYPE_3DM]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_MODEL_SAVE
Command4=ID_MODEL_SAVEOBJ
Command5=ID_SPHERES_LOAD
Command6=ID_SPHERES_SAVE
Command7=ID_SPHERES_EXPORT_POV
Command8=ID_SPHERES_EXPORT_VRML
Command9=ID_FILE_SAVEIMAGE
Command10=ID_FILE_SAVEIMAGEPS
Command11=ID_FILE_MRU_FILE1
Command12=ID_APP_EXIT
Command13=ID_VIEW_ROTATEMODE
Command14=ID_VIEW_PANMODE
Command15=ID_VIEW_ZOOMMODE
Command16=ID_VIEW_LIGHTINGANGLE
Command17=ID_MODE_POINT
Command18=ID_VIEW_RESETVIEW
Command19=ID_VIEW_TOOLBAR
Command20=ID_VIEW_STATUS_BAR
Command21=ID_SHOW_MODEL
Command22=ID_SHOW_SAMPLES
Command23=ID_SHOW_SPHERES
Command24=ID_SPHERETREE_NONE
Command25=ID_SPHERETREE_TOP
Command26=ID_SPHERETREE_PREV
Command27=ID_SPHERETREE_NEXT
Command28=ID_GENERATE_SAMPLES
Command29=ID_VALIDATE_MESH
Command30=ID_GENERATE_STATIC_MA
Command31=ID_GENERATE_ADAPTIVE_MA
Command32=ID_GENERATE_IMPROVE_MA
Command33=ID_REDUCE_HUBBARD
Command34=ID_REDUCE_MERGE
Command35=ID_REDUCE_BURST
Command36=ID_REDUCE_EXPAND
Command37=ID_OPTIMISE_PERSPHERE
Command38=ID_OPTIMISE_SIMPLEX
Command39=ID_OPTIMISE_BALANCE
Command40=ID_GENTREE_OCTREE
Command41=ID_GENTREE_HUBBARD
Command42=ID_GENTREE_MERGE
Command43=ID_GENTREE_BURST
Command44=ID_GENTREE_EXPAND
Command45=ID_GENTREE_COMBINED
Command46=ID_GENTREE_SPAWN
Command47=ID_GENTREE_GRID
Command48=ID_USE_CONVEX_TESTER
Command49=ID_EVALUATE_SPHERES
Command50=ID_WINDOW_NEW
Command51=ID_WINDOW_CASCADE
Command52=ID_WINDOW_TILE_HORZ
Command53=ID_WINDOW_ARRANGE
Command54=ID_APP_ABOUT
CommandCount=54

[MNU:IDR_MODELTYPE_OBJ]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_MODEL_SAVE
Command4=ID_MODEL_SAVEOBJ
Command5=ID_SPHERES_LOAD
Command6=ID_SPHERES_SAVE
Command7=ID_SPHERES_EXPORT_POV
Command8=ID_SPHERES_EXPORT_VRML
Command9=ID_FILE_SAVEIMAGE
Command10=ID_FILE_SAVEIMAGEPS
Command11=ID_FILE_MRU_FILE1
Command12=ID_APP_EXIT
Command13=ID_VIEW_ROTATEMODE
Command14=ID_VIEW_PANMODE
Command15=ID_VIEW_ZOOMMODE
Command16=ID_VIEW_LIGHTINGANGLE
Command17=ID_MODE_POINT
Command18=ID_VIEW_RESETVIEW
Command19=ID_VIEW_TOOLBAR
Command20=ID_VIEW_STATUS_BAR
Command21=ID_SHOW_MODEL
Command22=ID_SHOW_SAMPLES
Command23=ID_SHOW_SPHERES
Command24=ID_SPHERETREE_NONE
Command25=ID_SPHERETREE_TOP
Command26=ID_SPHERETREE_PREV
Command27=ID_SPHERETREE_NEXT
Command28=ID_GENERATE_SAMPLES
Command29=ID_VALIDATE_MESH
Command30=ID_GENERATE_STATIC_MA
Command31=ID_GENERATE_ADAPTIVE_MA
Command32=ID_GENERATE_IMPROVE_MA
Command33=ID_REDUCE_HUBBARD
Command34=ID_REDUCE_MERGE
Command35=ID_REDUCE_BURST
Command36=ID_REDUCE_EXPAND
Command37=ID_OPTIMISE_PERSPHERE
Command38=ID_OPTIMISE_SIMPLEX
Command39=ID_OPTIMISE_BALANCE
Command40=ID_GENTREE_OCTREE
Command41=ID_GENTREE_HUBBARD
Command42=ID_GENTREE_MERGE
Command43=ID_GENTREE_BURST
Command44=ID_GENTREE_EXPAND
Command45=ID_GENTREE_COMBINED
Command46=ID_GENTREE_SPAWN
Command47=ID_GENTREE_GRID
Command48=ID_USE_CONVEX_TESTER
Command49=ID_EVALUATE_SPHERES
Command50=ID_WINDOW_NEW
Command51=ID_WINDOW_CASCADE
Command52=ID_WINDOW_TILE_HORZ
Command53=ID_WINDOW_ARRANGE
Command54=ID_APP_ABOUT
CommandCount=54

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_NEXT_PANE
Command5=ID_PREV_PANE
CommandCount=5

[DLG:IDD_PROGRESS_METER (English (Ireland))]
Type=1
Class=?
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_PROGDIALOG_PROG,msctls_progress32,1342177280
Control3=IDC_MESSAGE,static,1342308352

[DLG:IDD_INPUT (English (Ireland))]
Type=1
Class=?
ControlCount=4
Control1=IDC_INPUT,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_MESSAGE,static,1342308352

[DLG:IDD_OUTPUTDIALOG (English (Ireland))]
Type=1
Class=?
ControlCount=3
Control1=IDC_OUTPUTBOX,edit,1352732676
Control2=IDC_CANCEL,button,1342242816
Control3=IDC_HIDE,button,1342242816

