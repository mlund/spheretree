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

#ifndef _SURFACE_REP_H_
#define _SURFACE_REP_H_

#include "Surface.h"
#include "SpacialHash.h"

class SurfaceRep {
private:
  SpacialHash sh;
  const Array<Surface::Point> *surPts;

public:
  SurfaceRep() { surPts = NULL; }

  //  setup
  void setup(const Array<Surface::Point> &pts, int divs = 20);

  //  get access to points
  const Array<Surface::Point> *getSurPts() const { return surPts; };

  //  point in sphere tests
  int flagContainedPoints(Array<bool> *flags, const Sphere &s,
                          const Array<bool> *banFlags = NULL) const;

  //  list and flag the points in the sphere - but don't do those in banFlags
  void listContainedPoints(Array<int> *list, Array<bool> *setFlags,
                           const Sphere &s,
                           const Array<bool> *banFlags = NULL) const;

private:
  //  flag points which are contained
  int processCell(Array<bool> *flags, const Sphere &s, int i, int j, int k,
                  const Array<bool> *banFlags) const;

  //  list and flag the points which are contained (flags can be NULL)
  void processCell(Array<int> *list, Array<bool> *setFlags, const Sphere &s,
                   int i, int j, int k, const Array<bool> *banFlags) const;

  //  0 == totally outside
  //  1 == totally inside
  //  2 == partially inside
  int getCellStatus(const Sphere &s, int i, int j, int k, int numPts) const;
};

#endif