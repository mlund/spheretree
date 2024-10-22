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

#include "ImageRGB.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "IMsupport.h"
#ifdef USE_IMAGE_MAGICK
#include <ImageMagick/Api.h>
#endif

//  for ImageMagick
#ifdef USE_IMAGE_MAGICK
ProcessorMonitor *ImageRGB::procMon = NULL;

void ImageRGB::ProgressMonitor(const char *text, const off_t quantum,
                               const off_t span) {
  if (procMon)
    if (!procMon->setProgress(100.0 * quantum / (float)span)) {
      //      CancelledException ce;
      //      throw ce;
    }
}
#endif

//  load/save using image magic
bool ImageRGB::load(const char *fileName, char **err, ProcessorMonitor *mon) {
#ifdef USE_IMAGE_MAGICK
  CHECK_DEBUG(fileName != NULL, "NULL file name");

  //  init ImageMagick
  setupIM();
  ImageInfo *imageInfo = CloneImageInfo((ImageInfo *)NULL);
  CHECK_DEBUG0(imageInfo != NULL);
  strncpy(imageInfo->filename, fileName, 255);
  ExceptionInfo e;
  GetExceptionInfo(&e);

  //  monitor
  float procScale = 0, procStart = 0;
  procMon = mon;
  if (mon) {
    //  loading say 80%
    procScale = mon->getScale();
    procStart = mon->getStart();
    mon->setup(procStart, 0.80 * procScale);
  }

  //  load new one
  SetMonitorHandler(ProgressMonitor);
  Image *image = ReadImage(imageInfo, &e);
  if (!image) {
    if (err)
      *err = e.message;
  } else {
    if (mon) {
      //  convert say 20%
      mon->setup(procStart + 80 * procScale, 0.20 * procScale);
    }

    fromImage(image, mon);
    DestroyImage(image);
  }

  DestroyImageInfo(imageInfo);
  return image != NULL;
#else
  //  IMAGE FUNCTIONS ARE DISABLED !!!!
  return FALSE;
#endif
}

bool ImageRGB::save(const char *fileName, char **err,
                    ProcessorMonitor *mon) const {
#ifdef USE_IMAGE_MAGICK
  CHECK_DEBUG(fileName != NULL, "NULL file name");

  //  init ImageMagick
  setupIM();

  float procStart = 0, procScale = 0;
  if (mon) {
    //  convert say 20%
    procScale = mon->getScale();
    procStart = mon->getStart();
    mon->setup(procStart, 0.20 * procScale);
  }

  Image *im = toImage(mon);
  strncpy(im->filename, fileName, 255);
  ImageInfo *imageInfo = CloneImageInfo(NULL);
  CHECK_DEBUG0(imageInfo != NULL);

  procMon = mon;
  if (mon) {
    //  saving say 80%
    mon->setup(procStart + 20 * procScale, 0.80 * procScale);
  }

  SetMonitorHandler(ProgressMonitor);
  int res = WriteImage(imageInfo, im);
  DestroyImage(im);
  DestroyImageInfo(imageInfo);

  if (!res) {
    ExceptionInfo e;
    GetExceptionInfo(&e);
    if (err)
      *err = e.message;
  }

  return res != 0;
#else
  //  IMAGE FUNCTIONS ARE DISABLED !!!!
  return FALSE;
#endif
}

//  load/save using image magic and make sure
//  the width and height are powers of 2
//  return -1 for failure, else Y/X of original image
float ImageRGB::loadTexture(const char *fileName, char **err,
                            ProcessorMonitor *mon) {
#ifdef USE_IMAGE_MAGICK
  CHECK_DEBUG(fileName != NULL, "NULL file name");

  //  init ImageMagick
  setupIM();
  ImageInfo *imageInfo = CloneImageInfo((ImageInfo *)NULL);
  CHECK_DEBUG0(imageInfo != NULL);
  strncpy(imageInfo->filename, fileName, 255);
  ExceptionInfo e;
  GetExceptionInfo(&e);

  float procScale = 0, procStart = 0;
  procMon = mon;
  if (mon) {
    //  loading say 40%
    procScale = mon->getScale();
    procStart = mon->getStart();
    mon->setup(procStart, 0.40 * procScale);
  }

  //  load new one
  SetMonitorHandler(ProgressMonitor);
  Image *image = ReadImage(imageInfo, &e);
  DestroyImageInfo(imageInfo);

  if (!image) {
    if (err)
      *err = e.message;
    return -1;
  }

  //  work out size
  int w, h;
  float texCorrectY = mapForGL(&w, &h, image->columns, image->rows);

  if (mon) {
    //  scale say 40%
    mon->setup(procStart + 40 * procScale, 0.40 * procScale);
  }

  //  scale image
  Image *scaleImage = ScaleImage(image, w, h, &e);
  DestroyImage(image);
  if (!scaleImage) {
    if (err)
      *err = e.message;
    return -1;
  }

  if (mon) {
    //  loading say 20%
    mon->setup(procStart + 80 * procScale, 0.20 * procScale);
  }
  fromImage(scaleImage, mon);
  DestroyImage(scaleImage);

  return texCorrectY;
#else
  //  IMAGE FUNCTIONS ARE DISABLED !!!!
  return FALSE;
#endif
}

bool ImageRGB::save(const char *fileName, unsigned char *d, int w, int h,
                    char **err, ProcessorMonitor *mon) {
#ifdef USE_IMAGE_MAGICK

  CHECK_DEBUG(fileName != NULL, "NULL filename");
  CHECK_DEBUG2(d != NULL && w > 0 && h > 0, "Invalid Input %d*%d", w, h);

  //  init ImageMagick
  setupIM();

  ExceptionInfo e;
  Image *im = ConstituteImage(w, h, "RGB", (StorageType)0, d, &e);
  CHECK_DEBUG0(im != NULL);
  strncpy(im->filename, fileName, 255);
  ImageInfo *imageInfo = CloneImageInfo(NULL);
  CHECK_DEBUG0(imageInfo != NULL);

  procMon = mon;
  SetMonitorHandler(ProgressMonitor);
  int res = WriteImage(imageInfo, im);

  DestroyImage(im);
  DestroyImageInfo(imageInfo);

  if (!res) {
    ExceptionInfo e;
    GetExceptionInfo(&e);
    if (err)
      *err = e.message;
  }

  return res != 0;
#else
  //  IMAGE FUNCTIONS ARE DISABLED !!!!
  return FALSE;
#endif
}

#ifdef USE_IMAGE_MAGICK

void ImageRGB::fromImage(Image *image, ProcessorMonitor *mon) {
  CHECK_DEBUG(image != NULL, "NULL destination");

  //  allocate storage
  int nM = image->rows, nN = image->columns;
  makeRoom(nM, nN);

  //  fill array
  ViewInfo *view = OpenCacheView(image);
  CHECK_DEBUG0(view);
  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m);

    //  copy row
    PixelPacket *pix = GetCacheView(view, 0, i, n, 1);
    CHECK_DEBUG0(pix != NULL);
    for (int j = 0; j < n; j++) {
      RGB *c = &index(i, j);
      c->r = pix[j].red;
      c->g = pix[j].green;
      c->b = pix[j].blue;
    }
    //  destroy pix ??
  }

  CloseCacheView(view);
}

Image *ImageRGB::toImage(ProcessorMonitor *mon) const {
  //  create new image
  ImageInfo *imageInfo = CloneImageInfo((ImageInfo *)NULL);
  CHECK_DEBUG0(imageInfo != NULL);
  Image *image = AllocateImage(imageInfo);
  CHECK_DEBUG0(image != NULL);
  DestroyImageInfo(imageInfo);

  //  set size
  image->columns = n;
  image->rows = m;

  //  fill image
  ViewInfo *view = OpenCacheView(image);
  CHECK_DEBUG0(view != NULL);
  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m);

    // copy row
    PixelPacket *pix = SetCacheView(view, 0, i, n, 1);
    CHECK_DEBUG0(pix != NULL);
    for (int j = 0; j < n; j++) {
      RGB c = index(i, j);
      pix[j].red = c.r;
      pix[j].green = c.g;
      pix[j].blue = c.b;
    }
    //  destroy pix ??
  }

  SyncCacheView(view);
  CloseCacheView(view);

  return image;
}
#endif

//  mapping for GL texture
int ImageRGB::nearestPowerOfTwo(int i) const {
  i = abs(i);

  int n = 1, t = 2;
  while (t < i) {
    t *= 2;
    n++;
  }

  //  prefer to scale UP
  return (int)pow(2, n);
}

float ImageRGB::mapForGL(int *w, int *h, int W, int H) {
  CHECK_DEBUG0(w != NULL && h != NULL);

  *w = nearestPowerOfTwo(W);
  *h = nearestPowerOfTwo(H);

  float oldRatio = H / (float)W;
  float newRatio = (*h) / (float)(*w);

  return W / (float)H; //  multiply texture by this to keep correct aspect
}
