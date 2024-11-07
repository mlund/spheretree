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

/////////////////////////////////////////////////////////////////////////////
// CProgressMeter dialog
#include "stdafx.h"
#include "ProgressMeter.h"
#include "MFC-Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProgressMeter::CProgressMeter(CWnd *pParent /*=NULL*/)
    : CDialog(CProgressMeter::IDD, pParent) {
  init();

  //{{AFX_DATA_INIT(CProgressMeter)
  //}}AFX_DATA_INIT
}

CProgressMeter::~CProgressMeter() {
  // if (maxFrame != 0)
  //   m_procAni.Close();  //  destroyed by parent
}

void CProgressMeter::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProgressMeter)
  DDX_Control(pDX, IDC_MESSAGE, c_message);
  DDX_Control(pDX, IDC_PROGDIALOG_PROG, m_procMeter);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressMeter, CDialog)
//{{AFX_MSG_MAP(CProgressMeter)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressMeter message handlers
void CProgressMeter::init(float mHt, float fr) {
  cancelled = false;
  lastTime = startTime = clock();
  maxHideTime = mHt;
  frameRate = fr;
}

bool CProgressMeter::updateProgress(float absProg) {
  //  get time
  unsigned long currentTime = clock();

  //  do we need to show the window
  if (!IsWindow(m_hWnd)) {
    //  get time since start
    float elapseTime = (clock() - startTime) / (float)CLOCKS_PER_SEC;

    //  compute complete time
    float totalElapseTime = (currentTime - startTime) / (float)CLOCKS_PER_SEC;
    float fullTime = 100.0f * totalElapseTime / absProg;

    if (totalElapseTime > 0.25 && fullTime > maxHideTime) {
      //  create window
      Create(CProgressMeter::IDD);

      //  center window within parent
      CWnd *parent = GetParent();
      if (!parent)
        parent = AfxGetMainWnd(); //  surrogate parent
      this->CenterWindow(parent);

      //  show window
      ShowWindow(SW_NORMAL);
    }
  }

  //  get time since last picture
  float elapseTime = (currentTime - lastTime) / (float)CLOCKS_PER_SEC;

  if (IsWindow(m_hWnd) &&
      (m_procMeter.GetPos() != (int)absProg || elapseTime > frameRate)) {
    //  move progress bar
    m_procMeter.SetPos((int)absProg);

    //  pump messages
    handleMessages(this);
  }

  return !cancelled;
}

bool CProgressMeter::sayHello() {
  if (m_hWnd)
    return updateProgress((float)m_procMeter.GetPos());
  else
    return updateProgress(0.1f);
}

void CProgressMeter::OnCancel() { cancelled = true; }

BOOL CProgressMeter::OnInitDialog() {
  CDialog::OnInitDialog();

  c_message.SetWindowText(msg);

  return TRUE;
}

void CProgressMeter::setMessage(const CString &msg) {
  this->msg = msg;
  if (m_hWnd)
    c_message.SetWindowText(msg);
}
