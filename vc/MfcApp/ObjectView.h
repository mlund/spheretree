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
// CObjectView window
#if !defined(AFX_ObjectView_H__6C0D1478_5CCB_11D4_9A1B_00104B9D2B6D__INCLUDED_)
#define AFX_ObjectView_H__6C0D1478_5CCB_11D4_9A1B_00104B9D2B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "../../src/Geometry/Point2D.h"
#include "../../src/Geometry/Vector3D.h"
#include "../OpenGL/GLSupport.h"

class CObjectView : public CView {
  // Construction
public:
  CObjectView();
  bool save(const char *fileName);
  bool saveEPS(const char *fileName);
  DECLARE_DYNCREATE(CObjectView)

  // Attributes
public:
  enum MODE { PAN, ROTATE, ZOOM, LIGHT, POINT, NEXT_MODE };
  MODE getMode() const;
  void setMode(MODE newMode);
  void resetView();
  void setupTexture(const char *fileName = NULL, float scale = 1.0f);
  void setupSurface(COLORREF col, COLORREF bkCol, float hilite = -1,
                    float shinyness = -1); // -1 for off

  COLORREF getRGB() const;
  COLORREF getBkRGB() const;
  CString getTextureFile() const;
  float getTextureScale() const;
  float getHilite() const;
  float getShinyness() const;

  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CObjectView)
protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  virtual void OnDraw(CDC *pDC);
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CObjectView();

  // Generated message map functions
protected:
  //{{AFX_MSG(CObjectView)
  afx_msg void OnPan();
  afx_msg void OnUpdatePan(CCmdUI *pCmdUI);
  afx_msg void OnRotate();
  afx_msg void OnUpdateRotate(CCmdUI *pCmdUI);
  afx_msg void OnZoom();
  afx_msg void OnUpdateZoom(CCmdUI *pCmdUI);
  afx_msg void OnLighting();
  afx_msg void OnUpdateLighting(CCmdUI *pCmdUI);
  afx_msg void OnReset();
  afx_msg void OnSaveImage();
  afx_msg void OnSaveImagePS();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL OnEraseBkgnd(CDC *pDC);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CView *pWnd, UINT nHitTest, UINT message);
  afx_msg void OnDestroy();
  afx_msg void OnModePoint();
  afx_msg void OnUpdateModePoint(CCmdUI *pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  GLInfo glInfo; //  OpenGL support
  virtual void drawObject(bool sel);
  void drawScene(GLint *dList, GLuint tex);
  bool loadTexture(const char *file, GLuint *tex);
  virtual void drawExtraObjects() {}

  GLint dList; // display list
  void clearDisplayList();

  CPoint lastPt; //  tracking and mode
  MODE mode;

  //  transformations
  float tr[3], scale;   //    translation and scale
  float fTr[3], fScale; //    FULL scane and center
  float m[16], mL[16];  //    object & light orientation
  void applyRotation(float m[16], float a, const Vector3D &v);
  void clearMatrix(float m[16]);
  //  mapping
  void screenToFrustum(Point2D *p, const CPoint &pt) const;
  void screenToVector(Vector3D *v, const CPoint &pt) const;

  HCURSOR hCursor; //  cursor support
  void setNewCursor(UINT nID);
  void setNewCursor(const char *res);

  float col[3], bkCol[3]; //  color
  float hilite, shine;    //  lighting

  GLuint tex; //  texture
  CString textureFile;
  float texScale;

  int selID; //  selection

  void setupPerspective(bool select = FALSE, int x = 0, int y = 0);
  void setupModelView();
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_ObjectView_H__6C0D1478_5CCB_11D4_9A1B_00104B9D2B6D__INCLUDED_)
