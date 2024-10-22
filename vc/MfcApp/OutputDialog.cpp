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

// OutputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "spheretree.h"
#include "OutputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputDialog dialog
COutputDialog::COutputDialog(CWnd *pParent /*=NULL*/)
    : CDialog(COutputDialog::IDD, pParent) {
  inLoop = false;

  //{{AFX_DATA_INIT(COutputDialog)
  m_outputString = _T("");
  //}}AFX_DATA_INIT
}

void COutputDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COutputDialog)
  DDX_Control(pDX, IDC_OUTPUTBOX, c_outputBox);
  DDX_Text(pDX, IDC_OUTPUTBOX, m_outputString);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COutputDialog, CDialog)
//{{AFX_MSG_MAP(COutputDialog)
ON_WM_SYSCOMMAND()
ON_BN_CLICKED(IDC_CANCEL, OnCancel)
ON_BN_CLICKED(IDC_HIDE, OnHide)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//  additional functions
void COutputDialog::clearOutput() {
  m_outputString = "";

  updateAndShow();
}

void COutputDialog::addText(const char *text) {
  CString t(text);
  t.Replace("\n", "\r\n");

  m_outputString += t;

  updateAndShow();
}

void COutputDialog::updateAndShow() {
  if (!IsWindow(GetSafeHwnd())) {
    //  create window
    Create(IDD_OUTPUTDIALOG);

    //  center window within parent
    CWnd *parent = GetParent();
    if (!parent)
      parent = AfxGetMainWnd(); //  surrogate parent
    this->CenterWindow(parent);
  }

  //  show window
  ShowWindow(SW_NORMAL);

  //  update text box
  UpdateData(FALSE);

  //  update output box to scroll
  int len = m_outputString.GetLength();
  c_outputBox.SetSel(len, len);

  //  handle some of the messages
  handleMessages();
}

void COutputDialog::handleMessages() {
  AfxGetMainWnd()->EnableWindow(FALSE); //  disable main window
  EnableWindow(TRUE);                   //  reenable this window

  //  pump messages for cancel button & redraws
  MSG msg;
  inLoop = true;
  while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
    if (!PreTranslateMessage(&msg)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }
  inLoop = false;

  AfxGetMainWnd()->EnableWindow(TRUE);
}

void COutputDialog::OnCancel() {
  if (inLoop) {
    CancelException ex;
    throw ex;
  }
}

void COutputDialog::hideWindow() {
  m_outputString = "";

  ShowWindow(SW_HIDE);

  AfxGetMainWnd()->EnableWindow(TRUE);
  AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void COutputDialog::OnSysCommand(UINT nID, LPARAM lParam) {
  if (nID != SC_CLOSE)
    CDialog::OnSysCommand(nID, lParam);
}

void COutputDialog::OnHide() {
  if (!inLoop)
    hideWindow();
}
