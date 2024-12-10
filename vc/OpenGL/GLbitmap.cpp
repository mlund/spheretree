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

#include "GLbitmap.h"

bool createGLDIB(GL_DIB_Info *inf, int w, int h) {
  CHECK_DEBUG0(inf);

  //  create new 24Bit bitmap, same size as window
  BITMAPINFOHEADER BIH;
  int iSize = sizeof(BITMAPINFOHEADER);
  memset(&BIH, 0, iSize);

  // Fill in the header info.
  BIH.biSize = iSize;
  BIH.biWidth = w;
  BIH.biHeight = h;
  BIH.biPlanes = 1;
  BIH.biBitCount = 24;
  BIH.biCompression = BI_RGB;

  // Create a new device context.
  CDC *pDC = new CDC;
  pDC->CreateCompatibleDC(NULL);
  HDC hdc = pDC->GetSafeHdc();

  if (!pDC)
    return false;

  // Create the DIB section.
  void *pBits;
  HBITMAP hBMP = CreateDIBSection(hdc, (BITMAPINFO *)&BIH, DIB_PAL_COLORS,
                                  &pBits, NULL, 0);

  if (!hBMP) {
    delete pDC;
    return false;
  }

  // Select the new bitmap into the buffer DC.
  HBITMAP hBMPold = (HBITMAP)::SelectObject(hdc, hBMP);

  // Fill in the pixel format descriptor.
  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1; // Version number
  pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;             // 32-bit depth buffer
  pfd.iLayerType = PFD_MAIN_PLANE; // Layer type

  // Choose the pixel format.
  int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

  // Set the pixel format.
  BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

  // Create a rendering context.
  HGLRC hRC = wglCreateContext(hdc);

  //  fill info
  inf->pDC = pDC;
  inf->hBMP = hBMP;
  inf->hOldBMP = hBMPold;
  inf->rc = hRC;

  //  select context
  wglMakeCurrent(hdc, hRC);

  //  setup for draw
  glViewport(0, 0, w, h);

  return hRC != NULL && hBMP != NULL;
}

void destroyGLDIB(const GL_DIB_Info &inf) {
  //  unslect context
  if (wglGetCurrentContext() == inf.rc)
    wglMakeCurrent(NULL, NULL);
  wglDeleteContext(inf.rc);

  //  tidy
  HDC hdc = inf.pDC->GetSafeHdc();
  ::SelectObject(hdc, inf.hOldBMP);
  ::DeleteObject(inf.hBMP);
  delete inf.pDC;
}