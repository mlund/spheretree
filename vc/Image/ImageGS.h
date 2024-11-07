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

#ifndef _IMAGE_GS_H_
#define _IMAGE_GS_H_

#include "IMsupport.h" /* clashes with MFC - use class Image instead */
#include "../../src/Base/Types.h"
#include "../../src/Storage/Array2D.h"
#include "../../src/Base/ProcMon.h"
#include "Formats.h"

class ImageGS : public Array2D<unsigned char> {
public:
  _inline ImageGS() {};
  ImageGS(const char *fileName) { load(fileName); }

  //  load/save using image magic
  bool load(const char *fileName, char **err = NULL,
            ProcessorMonitor *mon = NULL);
  bool save(const char *fileName, char **err = NULL,
            ProcessorMonitor *mon = NULL) const;

  //  convert
  void convertFromRGB(const class ImageRGB &im, ProcessorMonitor *mon = NULL);
  void convertToRGB(class ImageRGB *im, ProcessorMonitor *mon = NULL) const;

  //  clear edges of image (often used for proc)
  void clearEdges(unsigned char val);

  //  manip for binary images
  void mapBinary(unsigned char zero, unsigned char other,
                 ProcessorMonitor *mon = NULL);

  void fromImage(struct _Image *im, ProcessorMonitor *mon);
  struct _Image *toImage(ProcessorMonitor *mon) const;

private:
  //  for ImageMagick
  static ProcessorMonitor *procMon;
  static void ProgressMonitor(const char *text, const off_t quantum,
                              const off_t span);
};

#endif