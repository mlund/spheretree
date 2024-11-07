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

#include "GLsupport.h"

BOOL setWindowPixelFormat(HDC hDC, BOOL doubleBuf, BOOL depthBuf,
                          BOOL stencil) {
  PIXELFORMATDESCRIPTOR pD;
  memset(&pD, 0, sizeof(pD));

  pD.nSize = sizeof(pD);
  pD.nVersion = 1;
  pD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  if (doubleBuf)
    pD.dwFlags |= PFD_DOUBLEBUFFER;
  else
    pD.dwFlags |= PFD_DOUBLEBUFFER_DONTCARE;

  if (depthBuf)
    pD.cDepthBits = 32;
  else
    pD.dwFlags |= PFD_DEPTH_DONTCARE;

  pD.iPixelType = PFD_TYPE_RGBA;
  pD.cColorBits = 32;
  pD.cRedBits = 0;        // 8
  pD.cRedShift = 0;       // 16
  pD.cGreenBits = 0;      // 8
  pD.cGreenShift = 0;     // 8
  pD.cBlueBits = 0;       // 8
  pD.cBlueShift = 0;      // 0
  pD.cAlphaBits = 0;      // 8
  pD.cAlphaShift = 0;     // 24
  pD.cAccumBits = 0;      //  64
  pD.cAccumRedBits = 0;   //  16
  pD.cAccumGreenBits = 0; //  16
  pD.cAccumBlueBits = 0;  //  16
  pD.cAccumAlphaBits = 0; //  16
  if (stencil)
    pD.cStencilBits = 8;
  else
    pD.cStencilBits = 8;

  pD.cAuxBuffers = 0;
  pD.iLayerType = PFD_MAIN_PLANE;
  pD.bReserved = 0;
  pD.dwLayerMask = 0;
  pD.dwVisibleMask = 0;
  pD.dwDamageMask = 0;

  int GLPixelIndex = ChoosePixelFormat(hDC, &pD);
  if (GLPixelIndex == 0) {
    GLPixelIndex = 1;
    if (DescribePixelFormat(hDC, GLPixelIndex, sizeof(pD), &pD) == 0)
      return FALSE;
  }

  return SetPixelFormat(hDC, GLPixelIndex, &pD);
}

GLInfo createOpenGL(CWnd *wnd, BOOL doubleBuf, BOOL depthBuf, BOOL stencil) {
  CHECK_DEBUG(wnd != NULL, "Need a window");

  HWND hWnd = wnd->GetSafeHwnd();
  HDC hDC = ::GetDC(hWnd);
  GLInfo inf = {0, 0};

  // setup pixel format to be compatable with OpenGL
  if (!setWindowPixelFormat(hDC, doubleBuf, depthBuf, stencil))
    return inf;

  //   create palette if needed
  inf.hPal = createGLPalette(hDC);

  //  create context
  inf.hRC = wglCreateContext(hDC);
  ::ReleaseDC(hWnd, hDC);

  return inf;
}

void destroyOpenGL(GLInfo &inf, CWnd *wnd) {
  CHECK_DEBUG(wnd != NULL, "Need a window");

  //  destroy gl context
  if (inf.hRC) {
    if (wglGetCurrentContext() == inf.hRC)
      wglMakeCurrent(NULL, NULL);
    wglDeleteContext(inf.hRC);
  }

  //  destroy palette
  if (inf.hPal) {
    HWND hWnd = wnd->GetSafeHwnd();
    HDC hDC = ::GetDC(hWnd);

    ::SelectPalette(hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE), TRUE);
    ::DeleteObject(inf.hPal);
    ::ReleaseDC(hWnd, hDC);
  }
}

HPALETTE createGLPalette(HDC hDC) {
  //  check if we need a palette
  PIXELFORMATDESCRIPTOR pD;
  int n = GetPixelFormat(hDC);
  DescribePixelFormat(hDC, n, sizeof(pD), &pD);
  if (!(pD.dwFlags & PFD_NEED_PALETTE))
    return NULL;

  //  allocate palette and setup header
  int nColors = 1 << pD.cColorBits;
  LOGPALETTE *pPal =
      (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));
  pPal->palVersion = 0x300;
  pPal->palNumEntries = nColors;

  //  create masks for the number of bits in each channel
  BYTE redRange = (1 << pD.cRedBits) - 1; //  7 for 3-3-2
  BYTE greenRange = (1 << pD.cGreenBits) - 1;
  BYTE blueRange = (1 << pD.cBlueBits) - 1; //  3 for 3-3-2

  //  loop through entries
  for (int i = 0; i < nColors; i++) {
    //  fill in 8 bit equivilant for each
    BYTE v = (i >> pD.cRedShift) & redRange;
    pPal->palPalEntry[i].peRed = (unsigned char)((double)v * 255.0 / redRange);

    v = (i >> pD.cGreenShift) & greenRange;
    pPal->palPalEntry[i].peGreen =
        (unsigned char)((double)v * 255.0 / greenRange);

    v = (i >> pD.cBlueShift) & blueRange;
    pPal->palPalEntry[i].peBlue =
        (unsigned char)((double)v * 255.0 / blueRange);

    pPal->palPalEntry[i].peFlags = 0;
  }

  //  create palette
  HPALETTE hPal = CreatePalette(pPal);

  //  select and realize palette
  SelectPalette(hDC, hPal, FALSE);
  RealizePalette(hDC);

  //  tidy
  free(pPal);

  return hPal;
}

void init(GLInfo *inf) {
  inf->hPal = NULL;
  inf->hRC = NULL;
}

void getGlTransform(GLfloat tm[16], const Transform2D &tr) {
  CHECK_DEBUG(tm, "NULL destination");

  tm[0] = tr.index(0, 0);
  tm[4] = tr.index(0, 1);
  tm[8] = 0;
  tm[12] = tr.index(0, 2);
  tm[1] = tr.index(1, 0);
  tm[5] = tr.index(1, 1);
  tm[9] = 0;
  tm[13] = tr.index(1, 2);
  tm[2] = 0;
  tm[6] = 0;
  tm[10] = 1;
  tm[14] = 0;
  tm[3] = 0;
  tm[7] = 0;
  tm[11] = 0;
  tm[15] = 1;
}

void getGlTransform(GLfloat tm[16], const Transform3D &tr) {
  CHECK_DEBUG(tm, "NULL destination");

  tm[0] = tr.index(0, 0);
  tm[4] = tr.index(0, 1);
  tm[8] = tr.index(0, 2);
  tm[12] = tr.index(0, 3);
  tm[1] = tr.index(1, 0);
  tm[5] = tr.index(1, 1);
  tm[9] = tr.index(1, 2);
  tm[13] = tr.index(1, 3);
  tm[2] = tr.index(2, 0);
  tm[6] = tr.index(2, 1);
  tm[10] = tr.index(2, 2);
  tm[14] = tr.index(2, 3);
  tm[3] = 0;
  tm[7] = 0;
  tm[11] = 0;
  tm[15] = 1;
}

void setGlTransform(Transform2D *tr, const GLfloat tm[16]) {
  CHECK_DEBUG(tr, "NULL destination");

  tr->index(0, 0) = tm[0];
  tr->index(0, 1) = tm[4];
  tr->index(0, 2) = tm[12];
  tr->index(1, 0) = tm[1];
  tr->index(1, 1) = tm[5];
  tr->index(1, 2) = tm[13];
  tr->index(2, 0) = 0;
  tr->index(2, 1) = 0;
  tr->index(2, 2) = 1;
}

void setGlTransform(Transform3D *tr, const GLfloat tm[16]) {
  CHECK_DEBUG(tr, "NULL destination");

  tr->index(0, 0) = tm[0];
  tr->index(0, 1) = tm[4];
  tr->index(0, 2) = 0;
  tr->index(0, 3) = tm[12];
  tr->index(1, 0) = tm[1];
  tr->index(1, 1) = tm[5];
  tr->index(1, 2) = 0;
  tr->index(1, 3) = tm[13];
  tr->index(2, 0) = 0;
  tr->index(2, 1) = 0;
  tr->index(2, 2) = 1;
  tr->index(2, 3) = tm[14];
  tr->index(3, 0) = 0;
  tr->index(3, 1) = 0;
  tr->index(3, 2) = 0;
  tr->index(3, 3) = 1;
}