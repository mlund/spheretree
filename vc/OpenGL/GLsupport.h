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

/*
    A few useful things for open GL in windows
*/
#ifndef _GL_SUPPORT_H_
#define _GL_SUPPORT_H_

#include <afxwin.h>
#include <afxext.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "../../src/Geometry/Transform2D.h"
#include "../../src/Geometry/Transform3D.h"

//  color
struct GLcolor {
  GLfloat r, g, b;

  GLcolor(float gI = 0) { r = g = b = gI; }

  GLcolor(float rI, float gI, float bI) {
    r = rI;
    g = gI;
    b = bI;
  }
};

#define MAKE_GLCOLOR(c, l)                                                     \
  {                                                                            \
    c.r = GetRValue(l) / 255.0f;                                               \
    c.g = GetGValue(l) / 255.0f;                                               \
    c.b = GetBValue(l) / 255.0f;                                               \
  }

// context
struct GLInfo {
  HGLRC hRC;
  HPALETTE hPal;
};

//  setup and destroy context info
BOOL setWindowPixelFormat(HDC hDC, BOOL doubleBuf, BOOL depthBuf, BOOL stencil);
GLInfo createOpenGL(CWnd *wnd, BOOL doubleBuf, BOOL depthBif,
                    BOOL stencil = FALSE);
void destroyOpenGL(GLInfo &inf, CWnd *wnd);
HPALETTE createGLPalette(HDC hDC);
void init(GLInfo *inf);

//  MACROS for managing GL contexts
#define SELECT_CONTEXT()                                                       \
  HDC hDCold = wglGetCurrentDC();                                              \
  HGLRC hRCold = wglGetCurrentContext();                                       \
  HWND hWnd = GetSafeHwnd();                                                   \
  HDC hDC = ::GetDC(hWnd);                                                     \
  if (glInfo.hPal) {                                                           \
    SelectPalette(hDC, glInfo.hPal, FALSE);                                    \
    RealizePalette(hDC);                                                       \
  }                                                                            \
  if (hRCold != glInfo.hRC) {                                                  \
    wglMakeCurrent(hDC, glInfo.hRC);                                           \
  }

#define UNSELECT_CONTEXT()                                                     \
  if (hRCold != glInfo.hRC)                                                    \
    wglMakeCurrent(hDCold, hRCold);                                            \
  ::ReleaseDC(hWnd, hDC);

//  converting between transforms
void getGlTransform(GLfloat tm[16], const Transform2D &tr);
void getGlTransform(GLfloat tm[16], const Transform3D &tr);
void setGlTransform(Transform2D *tr, const GLfloat tm[16]);
void setGlTransform(Transform3D *tr, const GLfloat tm[16]);

#endif
