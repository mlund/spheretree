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

// ObjectView.cpp : implementation of the CObjectView class
//
#include "stdafx.h"
#include "resource.h"
#include "ObjectView.h"
#include "ProgressMeter.h"
#include "../../src/Base/Defs.h"
#include "../../src/Exceptions/CheckDebug.h"
#include "../OpenGL/GLsupport.h"
#include "../OpenGL/GLbitmap.h"
#include "../OpenGL/GLsave.h"
#include "../GL2PS/gl2ps.h"
#include "../Image/ImageRGB.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectView
IMPLEMENT_DYNCREATE(CObjectView, CView)

CObjectView::CObjectView() {
  tr[0] = tr[1] = tr[2] = 0;
  scale = 1;

  dList = -1;

  fTr[0] = fTr[1] = fTr[2] = 0;
  fScale = 1.0f;

  mode = PAN;
  setMode(ROTATE);

  col[0] = col[1] = col[2] = 0.75;
  bkCol[0] = bkCol[1] = bkCol[2] = 1.0;
  hilite = -1;
  shine = -1;
  tex = -1;
  texScale = 1;

  selID = -1;

  init(&glInfo);
}

CObjectView::~CObjectView() {}

BEGIN_MESSAGE_MAP(CObjectView, CView)
//{{AFX_MSG_MAP(CObjectView)
ON_COMMAND(ID_VIEW_PANMODE, OnPan)
ON_UPDATE_COMMAND_UI(ID_VIEW_PANMODE, OnUpdatePan)
ON_COMMAND(ID_VIEW_ROTATEMODE, OnRotate)
ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATEMODE, OnUpdateRotate)
ON_COMMAND(ID_VIEW_ZOOMMODE, OnZoom)
ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMMODE, OnUpdateZoom)
ON_COMMAND(ID_VIEW_LIGHTINGANGLE, OnLighting)
ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHTINGANGLE, OnUpdateLighting)
ON_COMMAND(ID_VIEW_RESETVIEW, OnReset)
ON_COMMAND(ID_FILE_SAVEIMAGE, OnSaveImage)
ON_COMMAND(ID_FILE_SAVEIMAGEPS, OnSaveImagePS)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_ERASEBKGND()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_RBUTTONDBLCLK()
ON_WM_SETCURSOR()
ON_WM_DESTROY()
ON_COMMAND(ID_MODE_POINT, OnModePoint)
ON_UPDATE_COMMAND_UI(ID_MODE_POINT, OnUpdateModePoint)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  glInfo = createOpenGL(this, true, true, true); //  added stencil support too

  // clear matrix
  SELECT_CONTEXT();
  glEnable(GL_DITHER); //  for 8 bit mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  glGetFloatv(GL_MODELVIEW_MATRIX, mL);
  UNSELECT_CONTEXT();

  return 0;
}

void CObjectView::clearDisplayList() {
  SELECT_CONTEXT();

  if (glIsList(dList))
    glDeleteLists(dList, 1);
  dList = -1;

  UNSELECT_CONTEXT();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectView message handlers
BOOL CObjectView::PreCreateWindow(CREATESTRUCT &cs) {
  if (!CView::PreCreateWindow(cs))
    return FALSE;

  cs.dwExStyle |= WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
                                     ::LoadCursor(NULL, IDC_ARROW),
                                     HBRUSH(COLOR_WINDOW + 1), NULL);

  return TRUE;
}

void CObjectView::OnSize(UINT nType, int cx, int cy) {
  CView::OnSize(nType, cx, cy);

  //  set viewport
  SELECT_CONTEXT();
  glViewport(0, 0, cx, cy);

  UNSELECT_CONTEXT();
}

BOOL CObjectView::OnEraseBkgnd(CDC *pDC) {
  return TRUE; //  do nothing - OpenGL will be wiping screen
}

void CObjectView::setupPerspective(bool select, int x, int y) {
  RECT rc;
  GetClientRect(&rc);
  float r = rc.bottom / (float)rc.right;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //  select pick region
  if (select) {
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    gluPickMatrix(x, rc.bottom - y, 4, 4, view);
  }

  //  setup open GL viewing to have frustum which is -1..1 across
  if (r > 1)
    glFrustum(-1.0, 1.0, -r * 1.0, r * 1.0, 10, 10000);
  else
    glFrustum(-1.0 / r, 1.0 / r, -1.0, 1.0, 10, 10000);

  glTranslatef(0, 0, -15);
}

void CObjectView::setupModelView() {
  //  setup matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(scale * fScale, scale * fScale, scale * fScale);
  glTranslatef(tr[0], tr[1], tr[2]);
  glMultMatrixf(m);
  glTranslatef(fTr[0], fTr[1], fTr[2]);
  glEnable(GL_NORMALIZE);
}

void CObjectView::OnDraw(CDC *pDC) {
  SELECT_CONTEXT();

  setupPerspective();
  drawScene(&dList, tex);

  //  turn off lighting
  glDisable(GL_LIGHTING);

  //  draw tracker ball
  if ((mode == ROTATE || mode == LIGHT) && GetCapture() == this) {
    //  do overlay
    glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);

    // work out size of tracker
    float rad = 0.85f;

    //  setup transform
    glMatrixMode(GL_MODELVIEW);
    if (mode == LIGHT)
      glLoadMatrixf(mL);
    else
      glLoadMatrixf(m);

    //  mesh
    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(1);

    //  draw light
    if (mode == LIGHT) {
      //  get position
      Vector3D v = {1.0f, 1.0f, 1.0f};
      v.norm();
      v.scale(rad);

      //  setup dot size
      GLint dotSize;
      glGetIntegerv(GL_POINT_SIZE, &dotSize);
      glPointSize(3);

      //  draw light
      glColor3f(1.0f, 1.0f, 0.0f);
      glBegin(GL_POINTS);
      glVertex3f(v.x, v.y, v.z);
      glEnd();

      glBegin(GL_LINES);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(v.x, v.y, v.z);
      glEnd();

      //  restore dot size
      glPointSize(dotSize);
    }

    //  draw sphere
    glColor3f(1, 0, 1);
    glRotatef(90, 1, 0, 0);
    GLUquadricObj *q = gluNewQuadric();
    gluSphere(q, rad, 10, 10);
    gluDeleteQuadric(q);
  }

  //  clean up and show picture
  glFlush();
  SwapBuffers(hDC);

  UNSELECT_CONTEXT();
}

void CObjectView::drawObject(bool sel) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  GLUquadricObj *q = gluNewQuadric();
  gluSphere(q, 1, 10, 10);
  gluDeleteQuadric(q);
}

void CObjectView::drawScene(GLint *dList, GLuint tex) {
  //  erase background
  glClearColor(bkCol[0], bkCol[1], bkCol[2], 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  //  setup state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  //  setup lighting (can do after modelview if wanted to move with object)
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(mL);
  GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  GLfloat spec[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  setupModelView();

  //  draw object
  if (dList && glIsList(*dList))
    glCallList(*dList);
  else {
    if (dList) {
      *dList = glGenLists(1);
      glNewList(*dList, GL_COMPILE_AND_EXECUTE);
    }

    /*
        //  texturing & color
        if (glIsTexture(tex)){
          glBindTexture(GL_TEXTURE_2D, tex);
          glEnable(GL_TEXTURE_2D);
          }
        else{
          glDisable(GL_TEXTURE_2D);
          }

        if (hilite > 0){
          float e = 128.0 - hilite*128.0/100.0;
          glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, e);
          float spec[4];
          spec[0] = spec[1] = spec[2] = shine/100.0;
          spec[3] = 1.0;
          glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
          }
        else{
          float spec[] = {0.0, 0.0, 0.0, 0.0};
          glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);   //  fully non
       specular
          }
    */

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(col[0], col[1], col[2]);
    drawObject(false);

    if (dList)
      glEndList();
  }

  drawExtraObjects();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectView modes
CObjectView::MODE CObjectView::getMode() const { return mode; }

void CObjectView::setMode(MODE newMode) {
  CHECK_DEBUG1(newMode == PAN || newMode == ROTATE || newMode == ZOOM ||
                   newMode == LIGHT || newMode == POINT,
               "Bad Mode (%d)", newMode);

  if (mode != newMode) {
    mode = newMode;
    if (GetCapture() == this)
      ReleaseCapture();

    switch (mode) {
    case PAN:
      setNewCursor(IDC_MOVE);
      break;
    case ROTATE:
    case LIGHT:
      setNewCursor(IDC_ROTATE_3D);
      break;
    case ZOOM:
      setNewCursor(IDC_ZOOM);
      break;
    case POINT:
      setNewCursor(IDC_ARROW);
      break;
    };
  }
}

/////////////////////////////////////////////////////////////////////////////
// CObjectView mapping
void CObjectView::screenToFrustum(Point2D *p, const CPoint &pt) const {
  RECT rc;
  GetClientRect(&rc);
  int w = rc.right - rc.left;
  int h = rc.bottom - rc.top;
  float r = (float)h / w;

  float wFact = 1.0f, hFact = 1.0f;
  if (r > 1) {
    wFact = 1;
    hFact = r;
  } else {
    wFact = 1 / r;
    hFact = 1;
  }

  //  map to -wFact..wFact in x
  p->x = 2.0 * wFact * (pt.x - w / 2) / (float)w;

  //  map to -hFact..hFact in y
  p->y = -2.0 * hFact * (pt.y - h / 2) / (float)h;
}

void CObjectView::screenToVector(Vector3D *v, const CPoint &pt) const {
  RECT rc;
  GetClientRect(&rc);
  int w = rc.right - rc.left;
  int h = rc.bottom - rc.top;
  float r = (float)h / w;

  float wFact = 1.0f, hFact = 1.0f;
  if (r > 1) {
    wFact = 1;
    hFact = r;
  } else {
    wFact = 1 / r;
    hFact = 1;
  }

  //  map to -wFact..wFact in x
  v->x = 2.0 * wFact * (pt.x - w / 2) / (float)w;

  //  map to -hFact..hFact in y
  v->y = -2.0 * r * (pt.y - h / 2) / (float)h;

  //  find z
  v->z = 0;
  float d = v->mag();
  v->z = cos((M_PI / 2.0) * (d < 1.0 ? d : 1.0));

  //  normalise
  v->norm();
}

void CObjectView::applyRotation(float m[16], float a, const Vector3D &v) {
  //  update matrix to rotate about object's origin (apply rotation last)
  SELECT_CONTEXT();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(a, v.x, v.y, v.z);
  glMultMatrixf(m);
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  glPopMatrix();
  UNSELECT_CONTEXT();
}

void CObjectView::clearMatrix(float m[16]) {
  //  give us an identity matrix
  SELECT_CONTEXT();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  glPopMatrix();
  UNSELECT_CONTEXT();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectView interaction
void CObjectView::OnLButtonDown(UINT nFlags, CPoint point) {
  if (mode == POINT) {
    selID = -1;

    SELECT_CONTEXT();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  setup select mode
    GLuint selBuf[1024];
    glSelectBuffer(1024, selBuf);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(~0);

    //  save projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    setupPerspective(true, point.x, point.y);

    glMatrixMode(GL_MODELVIEW);
    setupModelView();

    //  draw scene for selection
    drawObject(true);

    int sI = glRenderMode(GL_RENDER);
    if (sI) {
      //  find closest hit

      /*      GLint minZ = 1000000;
            for (int l = 0; l < sI; l++){
              GLint z = selBuf[4*l+1];
              if (z < minZ){
                minZ = z;
                selID = selBuf[4*l+3];
                }
              }*/

      selID = selBuf[4 * (sI - 1) + 3];
      TRACE("selID = %d\n", selID);
      clearDisplayList();
      InvalidateRect(NULL);
    } else {
      selID = -1;
      TRACE("No Hit\n");
      clearDisplayList();
      InvalidateRect(NULL);
    }

    //  restore matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    UNSELECT_CONTEXT();
  } else if (mode != ZOOM) {
    //  record starting point and hook mouse
    lastPt = point;
    SetCapture();
    RedrawWindow();
  } else {
    scale *= 2.0f; //  zoom in
    RedrawWindow();
  }

  CView ::OnLButtonDown(nFlags, point);
}

void CObjectView::OnLButtonUp(UINT nFlags, CPoint point) {
  if (GetCapture() == this) {
    ReleaseCapture();
    RedrawWindow();
  }

  CView ::OnLButtonUp(nFlags, point);
}

void CObjectView::OnRButtonDown(UINT nFlags, CPoint point) {
  if (mode == ZOOM) {
    scale *= 0.5f; // zoom out
    RedrawWindow();
  }

  CView ::OnRButtonDown(nFlags, point);
}

void CObjectView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  if (mode == ZOOM)
    OnLButtonDown(nFlags, point); //  nicer action
  else
    CView ::OnLButtonDblClk(nFlags, point);
}

void CObjectView::OnRButtonDblClk(UINT nFlags, CPoint point) {
  if (mode == ZOOM)
    OnRButtonDown(nFlags, point); //  nicer action
  else
    CView::OnRButtonDblClk(nFlags, point);
}

void CObjectView::OnMouseMove(UINT nFlags, CPoint point) {
  if (GetCapture() == this && (nFlags & MK_LBUTTON)) {
    if (mode == PAN) {
      Point2D pL, pC;
      screenToFrustum(&pL, lastPt);
      screenToFrustum(&pC, point);

      //  update translate
      tr[0] += (pC.x - pL.x) * 1000.0 / scale;
      tr[1] += (pC.y - pL.y) * 1000.0 / scale;
    } else if (mode == ROTATE || mode == LIGHT) {
      Vector3D vL, vC;
      screenToVector(&vL, lastPt);
      screenToVector(&vC, point);

      //  calculate angle prop to length mouse movement
      float dX = vC.x - vL.x;
      float dY = vC.y - vL.y;
      float dZ = vC.z - vL.z;
      float ang = 90.0 * sqrt(dX * dX + dY * dY + dZ * dZ);

      // vector is just cross product
      Vector3D v;
      v.cross(vL, vC);
      v.norm();

      if (mode == ROTATE)
        applyRotation(m, ang, v);
      else
        applyRotation(mL, ang, v);
    }

    lastPt = point;
    RedrawWindow();
  }

  CView ::OnMouseMove(nFlags, point);
}

void CObjectView::resetView() {
  tr[0] = 0;
  tr[1] = 0;
  tr[2] = 0;
  scale = 1.0;

  clearMatrix(m);
  clearMatrix(mL);

  RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
//  cursor support
void CObjectView::setNewCursor(UINT nID) {
  HCURSOR newCursor = AfxGetApp()->LoadCursor(nID);
  if (GetCursor() == hCursor)
    SetCursor(newCursor);
  DeleteObject(hCursor);
  hCursor = newCursor;
}

void CObjectView::setNewCursor(const char *res) {
  HCURSOR newCursor = AfxGetApp()->LoadCursor(res);
  if (GetCursor() == hCursor)
    SetCursor(newCursor);
  DeleteObject(hCursor);
  hCursor = newCursor;
}

BOOL CObjectView::OnSetCursor(CView *pWnd, UINT nHitTest, UINT message) {
  if (hCursor && nHitTest == HTCLIENT) {
    SetCursor(hCursor);
    return TRUE;
  } else {
    return CView::OnSetCursor(pWnd, nHitTest, message);
  }
}

/////////////////////////////////////////////////////////////////////////////
//  attributes
COLORREF CObjectView::getRGB() const {
  long R = col[0] * 255.0;
  long G = col[1] * 255.0;
  long B = col[2] * 255.0;
  return RGB(R, G, B);
}

COLORREF CObjectView::getBkRGB() const {
  long R = bkCol[0] * 255.0;
  long G = bkCol[1] * 255.0;
  long B = bkCol[2] * 255.0;
  return RGB(R, G, B);
}

CString CObjectView::getTextureFile() const { return textureFile; }

float CObjectView::getTextureScale() const { return texScale; }

float CObjectView::getHilite() const { return hilite; }

float CObjectView::getShinyness() const { return hilite; }

void CObjectView::setupTexture(const char *file, float scale) {
  SELECT_CONTEXT();

  if (file == NULL || textureFile.Compare(file) != 0) {
    textureFile = file;
    if (file && file[0] != '\0') {
      loadTexture(file, &tex);
    } else if (glIsTexture(tex)) {
      glDeleteTextures(1, &tex);
      tex = -1;
    }
  }
  UNSELECT_CONTEXT();

  texScale = scale;
  clearDisplayList();
  InvalidateRect(NULL);
}

void CObjectView::setupSurface(COLORREF c, COLORREF bkC, float nH,
                               float nS) { // -1 for off
  col[0] = GetRValue(c) / 256.0;
  col[1] = GetGValue(c) / 256.0;
  col[2] = GetBValue(c) / 256.0;

  bkCol[0] = GetRValue(bkC) / 256.0;
  bkCol[1] = GetGValue(bkC) / 256.0;
  bkCol[2] = GetBValue(bkC) / 256.0;

  hilite = nH;
  shine = nS;

  clearDisplayList();
  InvalidateRect(NULL);
}

bool CObjectView::loadTexture(const char *file, GLuint *tex) {
  //  kill old one
  if (!glIsTexture(*tex)) {
    //  load new texture
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    /*GL_LINEAR*/ GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    /*GL_LINEAR*/ GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  } else {
    glBindTexture(GL_TEXTURE_2D, *tex);
  }

  CProgressMeter pm(this);
  pm.init();

  ImageRGB im;
  char *err;
  float r = im.loadTexture(file, &err, &pm);
  if (r > 0) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.getN(), im.getM(), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, &im.index(0, 0));
    return true;
  } else {
    AfxMessageBox(err, MB_OK | MB_ICONSTOP);
    return false;
  }
}

bool CObjectView::save(const char *file) {
  RECT rc;
  GetClientRect(&rc);
  int w = rc.right - rc.left;
  int h = rc.bottom - rc.top;

  GL_DIB_Info inf;
  createGLDIB(&inf, w, h);

  //  only works because bitmap is same size as window
  setupPerspective();

  //  do draw (need to save stuff from old context)
  GLuint tmpTex = -1;
  if (textureFile.GetLength())
    loadTexture(textureFile, &tmpTex);
  drawScene(NULL, tmpTex);

  CProgressMeter pm(this);
  pm.init();

  char *err;
  bool saved = saveGL(file, &err, &pm);
  if (!saved)
    AfxMessageBox(err, MB_OK | MB_ICONSTOP);

  destroyGLDIB(inf);
  return saved;
}

bool CObjectView::saveEPS(const char *fileName) {
  SELECT_CONTEXT();

  //  to use GL2PS
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  int buffsize = 0, state = GL2PS_OVERFLOW;
  while (state == GL2PS_OVERFLOW) {
    buffsize += 1024 * 1024;
    TRACE("bufferSize set to %d\n", buffsize);

    gl2psBeginPage("GRB", "GRB", GL2PS_BSP_SORT /*GL2PS_SIMPLE_SORT*/,
                   GL2PS_NONE /*GL2PS_OCCLUSION_CULL*/, GL_RGBA, 0, NULL,
                   buffsize, f);
    gl2psLineWidth(1);
    gl2psPointSize(1);

    setupPerspective();
    drawScene(NULL, -1);

    state = gl2psEndPage();
  }
  fclose(f);

  UNSELECT_CONTEXT();
  return true;
}

void CObjectView::OnDestroy() {
  destroyOpenGL(glInfo, this);
  CView ::OnDestroy();
}

void CObjectView::OnPan() { setMode(CObjectView::PAN); }

void CObjectView::OnUpdatePan(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(getMode() == CObjectView::PAN);
}

void CObjectView::OnRotate() { setMode(CObjectView::ROTATE); }

void CObjectView::OnUpdateRotate(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(getMode() == CObjectView::ROTATE);
}

void CObjectView::OnZoom() { setMode(CObjectView::ZOOM); }

void CObjectView::OnUpdateZoom(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(getMode() == CObjectView::ZOOM);
}

void CObjectView::OnLighting() { setMode(CObjectView::LIGHT); }

void CObjectView::OnUpdateLighting(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(getMode() == CObjectView::LIGHT);
}

void CObjectView::OnReset() { resetView(); }

void CObjectView::OnSaveImage() {
  CFileDialog cd(FALSE, "jpg", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 formatsWrite, this);
  if (cd.DoModal() == IDOK) {
    save(cd.GetPathName());
  }
}

void CObjectView::OnSaveImagePS() {
  CFileDialog cd(FALSE, "eps", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "Encapsulated Postscript (*.eps,*.ps)|*.eps; *.ps||", this);
  if (cd.DoModal() == IDOK) {
    saveEPS(cd.GetPathName());
  }
}

void CObjectView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint) {
  clearDisplayList();
  InvalidateRect(NULL);
}

void CObjectView::OnModePoint() { setMode(CObjectView::POINT); }

void CObjectView::OnUpdateModePoint(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(getMode() == CObjectView::POINT);
}
