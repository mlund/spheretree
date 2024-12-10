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

#if !defined(AFX_MODELDOC_H__53B7BE82_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_)
#define AFX_MODELDOC_H__53B7BE82_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CModelDoc document
#include "../../src/Surface/Surface.h"

class CModelDoc : public CDocument {
protected:
  CModelDoc(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CModelDoc)

  // Attributes
public:
  const static int DEFAULT_BOX_SIZE;

private:
  Surface surface;
  float scaleFactor;

  // Operations
public:
  // Overrides
  //{{AFX_VIRTUAL(CModelDoc)
public:
  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CModelDoc();

  // Generated message map functions
protected:
  //{{AFX_MSG(CModelDoc)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  //  get the scale factor used to load the model
  float getScaleFactor() const { return scaleFactor; }

  friend class CModelView;
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_MODELDOC_H__53B7BE82_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_)
