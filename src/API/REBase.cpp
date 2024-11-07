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

#include "REBase.h"

REBase::REBase() {
  srcSpheres = NULL;
  surRep = NULL;
}

bool REBase::setup(const Array<Sphere> &src, const SurfaceRep &surRep) {
  this->srcSpheres = &src;
  this->surRep = &surRep;
  return true;
}

void REBase::tidyUp() {
  this->srcSpheres = NULL;
  this->surRep = NULL;
}

bool REBase::reduceSpheres(Array<Sphere> *dest, const Array<Sphere> &src,
                           const SurfaceRep &surRep, int maxAllow,
                           Array<int> *destCounts, Array<int> *list) {
  dest->setSize(0);
  if (!setup(src, surRep))
    return false; //  useful for early stages of the SRExpand algorithm

  Array<int> inds;
  bool res = reduceSpheres(&inds, maxAllow, destCounts);

  if (list)
    list->clone(inds);

  if (res)
    getSpheres(dest, inds);

  tidyUp();
  return res;
}

void REBase::getSpheres(Array<Sphere> *dest, const Array<int> &inds) const {
  CHECK_DEBUG0(srcSpheres != NULL);

  //  setup destination spheres
  int numInds = inds.getSize();
  dest->resize(numInds);
  for (int i = 0; i < numInds; i++) {
    int nS = inds.index(i);
    dest->index(i) = srcSpheres->index(nS);
  }
}