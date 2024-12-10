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
#if !defined(                                                                  \
    AFX_PROGRESSMeter_H__15C68BA1_52A2_11D4_9A1B_00104B9D2B6D__INCLUDED_)
#define AFX_PROGRESSMeter_H__15C68BA1_52A2_11D4_9A1B_00104B9D2B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../src/Base/ProcMon.h"
#include "resource.h"

class CProgressMeter : public CDialog, public ProcessorMonitor {
  // Construction
public:
  CProgressMeter(CWnd *pParent = NULL);
  ~CProgressMeter();

  void init(float maxHideTime = 1.0, float frameRate = 0.25);
  bool updateProgress(float absProg);
  bool sayHello();
  void setMessage(const CString &msg);

private:
  bool cancelled;
  unsigned long startTime, lastTime;
  float maxHideTime, frameRate;

  // Dialog Data
  //{{AFX_DATA(CProgressMeter)
  enum { IDD = IDD_PROGRESS_METER };
  CStatic c_message;
  CProgressCtrl m_procMeter;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProgressMeter)
public:
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CProgressMeter)
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  CString msg;
};

//{{AFX_INSERT_LOCATION}}
#endif
