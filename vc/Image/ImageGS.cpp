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

#include "ImageGS.h"
#include "ImageRGB.h"
#include "IMsupport.h"
#include <ImageMagick/Api.h>

//  for ImageMagick
ProcessorMonitor *ImageGS::procMon = NULL;

void ImageGS::ProgressMonitor(const char *text, const off_t quantum,
                              const off_t span) {
  if (procMon)
    if (!procMon->setProgress(100.0 * quantum / (float)span)) {
      //      CancelledException ce;
      //      throw ce;
    }
}

//  load/save using image magic
bool ImageGS::load(const char *fileName, char **err, ProcessorMonitor *mon) {
  CHECK_DEBUG(fileName != NULL, "NULL file name");

  //  init ImageMagick
  setupIM();
  ImageInfo *imageInfo = CloneImageInfo((ImageInfo *)NULL);
  strncpy(imageInfo->filename, fileName, 255);
  ExceptionInfo e;
  GetExceptionInfo(&e);

  //  proc mon
  float procScale = 0, procStart = 0;
  procMon = mon;
  if (mon) {
    //  loading say 80%
    procScale = mon->getScale();
    procStart = mon->getStart();
    mon->setup(procStart, 0.80 * procScale);
    procMon = mon;
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
}

bool ImageGS::save(const char *fileName, char **err,
                   ProcessorMonitor *mon) const {
  CHECK_DEBUG(fileName != NULL, "NULL file name");

  //  init ImageMagick
  setupIM();

  float procScale = 0, procStart = 0;
  if (mon) {
    //  convertion say 20%
    procScale = mon->getScale();
    procStart = mon->getStart();
    mon->setup(procStart, 0.20 * procScale);
  }

  Image *im = toImage(mon);
  strncpy(im->filename, fileName, 255);
  ImageInfo *imageInfo = CloneImageInfo(NULL);

  //  proc mon
  procMon = mon;
  if (mon) {
    //  save say 80%
    mon->setup(procStart + 20 * procScale, 0.80 * procScale);
  }

  SetMonitorHandler(ProgressMonitor);
  int res = WriteImage(imageInfo, im); //  proc

  DestroyImage(im);
  DestroyImageInfo(imageInfo);

  if (!res) {
    ExceptionInfo e;
    GetExceptionInfo(&e);
    if (err)
      *err = e.message;
  }

  return res != 0;
}

//  convert
void ImageGS::convertFromRGB(const class ImageRGB &im, ProcessorMonitor *mon) {
  int nM = im.getM(), nN = im.getN();
  makeRoom(nM, nN);

  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m); // no cancel
    for (int j = 0; j < n; j++) {
      RGB c = im.index(i, j);
      float r = c.r, g = c.g, b = c.b;
      index(i, j) = (unsigned char)((r + g + b) / 3.0);
    }
  }
}

void ImageGS::convertToRGB(class ImageRGB *im, ProcessorMonitor *mon) const {
  im->makeRoom(m, n);

  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m); //  no cancel
    for (int j = 0; j < n; j++) {
      unsigned char g = index(i, j);
      RGB *c = &im->index(i, j);
      c->r = g;
      c->g = g;
      c->b = g;
    }
  }
}

void ImageGS::fromImage(Image *image, ProcessorMonitor *mon) {
  CHECK_DEBUG(image != NULL, "NULL image");

  //  allocate storage
  int nM = image->rows, nN = image->columns;
  makeRoom(nM, nN);

  //  fill array
  ViewInfo *view = OpenCacheView(image);
  CHECK_DEBUG0(view != NULL);

  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m);

    //  copy row
    PixelPacket *pix = GetCacheView(view, 0, i, n, 1);
    CHECK_DEBUG0(pix != NULL);

    for (int j = 0; j < n; j++) {
      unsigned char r = pix[j].red;
      unsigned char g = pix[j].green;
      unsigned char b = pix[j].blue;

      index(i, j) = (unsigned char)((r + g + b) / 3.0);
    }
    //  destroy pix ??
  }

  CloseCacheView(view);
}

Image *ImageGS::toImage(ProcessorMonitor *mon) const {
  //  create new image
  ImageInfo *imageInfo = CloneImageInfo((ImageInfo *)NULL);
  CHECK_DEBUG0(imageInfo != NULL);

  Image *image = AllocateImage(imageInfo);
  DestroyImageInfo(imageInfo);
  CHECK_DEBUG0(image != NULL);

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
      unsigned char g = index(i, j);
      pix[j].red = g;
      pix[j].green = g;
      pix[j].blue = g;
    }
    //  destroy pix ??
  }

  SyncCacheView(view);
  CloseCacheView(view);

  return image;
}

void ImageGS::clearEdges(unsigned char val) {
  //  clear edges
  for (int i = 0; i < m; i++)
    index(i, 0) = index(i, n - 1) = val;

  for (int j = 0; j < n; j++)
    index(0, j) = index(m - 1, j) = val;
}

void ImageGS::mapBinary(unsigned char zero, unsigned char other,
                        ProcessorMonitor *mon) {
  for (int i = 0; i < m; i++) {
    if (mon)
      mon->setProgress(100.0 * i / (float)m);

    for (int j = 0; j < n; j++)
      if (index(i, j) == 0)
        index(i, j) = zero;
      else
        index(i, j) = other;
  }
}
