/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

// SphereTree.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SphereTree.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ModelView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSphereTreeApp

BEGIN_MESSAGE_MAP(CSphereTreeApp, CWinApp)
//{{AFX_MSG_MAP(CSphereTreeApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSphereTreeApp construction
CSphereTreeApp::CSphereTreeApp() {}

CSphereTreeApp::~CSphereTreeApp() {
#ifdef USE_RHINO_IO
  RhinoIOCleanup();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSphereTreeApp object

CSphereTreeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSphereTreeApp initialization
extern char errorLogDir[];
extern char magickModDir[];

BOOL CSphereTreeApp::InitInstance() {
  /*
    //  setup directories
    char buffer[1024];
    GetModuleFileName(NULL, buffer, 1023);

    int lastI = -1;
    for (int i = 0; buffer[i] != '\0'; i++)
      if (buffer[i] == '\\' || buffer[i] == '/')
        lastI = i;

    if (lastI >= 0){
      buffer[lastI] = '\0';

      sprintf(errorLogDir, "%s\\Errors\\", buffer);
      sprintf(magickModDir, "%s\\", buffer);
      }
  */
  sprintf(errorLogDir, "c:\\Devel\\SphereTree\\");
  sprintf(magickModDir, "c:\\Devel\\ImageMagick-DIST\\DLL-MOD\\");

  // Standard initialization
  AfxEnableControlContainer();

#ifdef _AFXDLL
  Enable3dControls(); // Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif

  // Change the registry key under which our settings are stored.
  SetRegistryKey(_T("ISG, Trinity College Dublin"));

  LoadStdProfileSettings(); // Load standard INI file options (including MRU)

  // Register document templates
  CMultiDocTemplate *pDocTemplate;

  pDocTemplate = new CMultiDocTemplate(
      IDR_MODELTYPE_OBJ, RUNTIME_CLASS(CModelDoc), RUNTIME_CLASS(CChildFrame),
      RUNTIME_CLASS(CModelView));
  AddDocTemplate(pDocTemplate);

  pDocTemplate = new CMultiDocTemplate(
      IDR_MODELTYPE_SUR, RUNTIME_CLASS(CModelDoc), RUNTIME_CLASS(CChildFrame),
      RUNTIME_CLASS(CModelView));
  AddDocTemplate(pDocTemplate);

#ifdef USE_RHINO_IO
  pDocTemplate = new CMultiDocTemplate(
      IDR_MODELTYPE_3DM, RUNTIME_CLASS(CModelDoc), RUNTIME_CLASS(CChildFrame),
      RUNTIME_CLASS(CModelView));
  AddDocTemplate(pDocTemplate);
#endif

  // create main MDI Frame window
  CMainFrame *pMainFrame = new CMainFrame;
  if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
    return FALSE;
  m_pMainWnd = pMainFrame;

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  // Dispatch commands specified on the command line
  if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
    if (!ProcessShellCommand(cmdInfo))
      return FALSE;

  //  show window
  pMainFrame->ShowWindow(m_nCmdShow);
  pMainFrame->UpdateWindow();

  /*
    //  setup memory tracking
    CFile *file = new CFile("c:/devel/coll-dump.txt", CFile::modeCreate |
    CFile::modeWrite); BOOL bEnable = AfxEnableMemoryTracking(FALSE);
    afxDump.m_pFile = file;
    AfxEnableMemoryTracking(bEnable);
  */
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  // Dialog Data
  //{{AFX_DATA(CAboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSphereTreeApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSphereTreeApp message handlers
