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

#include "stdafx.h"
#include "ModelDoc.h"
#include "../../src/Surface/OBJLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int CModelDoc::DEFAULT_BOX_SIZE = 1000;

IMPLEMENT_DYNCREATE(CModelDoc, CDocument)

CModelDoc::CModelDoc() { scaleFactor = -1; }

CModelDoc::~CModelDoc() {}

BEGIN_MESSAGE_MAP(CModelDoc, CDocument)
//{{AFX_MSG_MAP(CModelDoc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelDoc commands

BOOL CModelDoc::OnOpenDocument(LPCTSTR lpszPathName) {
  int len = strlen(lpszPathName);
  if (stricmp(lpszPathName + len - 4, ".3dm") == 0) {
    char *err = NULL;
    if (!surface.loadRhinoSurface(lpszPathName, &err)) {
      CString msg;
      msg.Format("ERROR in RhinoIO : %s", err);
      return FALSE;
    }
  } else if (stricmp(lpszPathName + len - 4, ".obj") == 0) {
    if (!loadOBJ(&surface, lpszPathName)) {
      CString msg;
      msg.Format("ERROR loading %s", lpszPathName);
      AfxMessageBox(msg);
      return FALSE;
    }
  } else if (!surface.loadSurface(lpszPathName)) {
    CString msg;
    msg.Format("ERROR loading %s", lpszPathName);
    AfxMessageBox(msg);
    return FALSE;
  }

  //  scale the figure to fit standard box
  this->scaleFactor = surface.fitIntoBox(DEFAULT_BOX_SIZE);

  CString msg;
  msg.Format("%ld points, %ld triangles", surface.vertices.getSize(),
             surface.triangles.getSize());
  AfxMessageBox(msg);
  return TRUE;
}
