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
    Line segment between two points
*/
#ifndef _LINE_SEGMENT_H_
#define _LINE_SEGMENT_H_

#include "Transform2D.h"
#include "Point2D.h"
#include "Line2D.h"

struct LineSeg {
public:
  Point2D p0, p1;

  //  construction
  __inline LineSeg() {}

  __inline LineSeg(const Point2D &p0, const Point2D &p1) { assign(p0, p1); }

  __inline LineSeg(const LineSeg &l) { assign(l); }

  //  assignment
  __inline void assign(const Point2D &p0, const Point2D &p1) {
    this->p0 = p0;
    this->p1 = p1;
  }

  __inline void assign(const LineSeg &seg) {
    p0 = seg.p0;
    p1 = seg.p1;
  }

  //  get line equation
  __inline void getLine(Line2D *l) const {
    CHECK_DEBUG(l != NULL, NULL);
    l->assign(p0, p1);
  }

  //  distance
  REAL distance(const Point2D &p) const;
  REAL distanceFrom2DLine(const Point2D &p) const;

  //  transform
  void transform(LineSeg *seg, Transform2D tr) const;

  //  bounding box
  void getBoundingBox(Point2D *pMin, Point2D *pMax) const;
  bool isInSegment(const Point2D &p) const;

  //  intersection
  bool intersect(Point2D *p, const LineSeg &other) const;
  REAL closestPoint(Point2D *p, const Point2D &other) const;

  //  projects
  bool projectOnto(Point2D *p, const Point2D &q) const;

  //  length
  __inline REAL length() const { return p0.distance(p1); }
};

#endif
