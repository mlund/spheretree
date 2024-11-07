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

#include "Line2D.h"
#include "Vector2D.h"
#include "../Exceptions/CheckDebug.h"
#include <stdlib.h>

//  intersection
bool Line2D::intersect(Point2D *p, const Line2D &l) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  REAL div = l.a * b - a * l.b;
  if (fabs(div) < EPSILON)
    return false;

  p->x = (c * l.b - b * l.c) / div;
  p->y = (a * l.c - c * l.a) / div;

  return true;
}

//  projection
void Line2D::project(Point2D *p, const Point2D &q) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  //  point on line
  Point2D pLine;
  if (fabs(a) > fabsl(b)) {
    pLine.y = 0;
    pLine.x = -c / a;
  } else {
    pLine.x = 0;
    pLine.y = -c / b;
  }

  Vector2D v;
  v.difference(q, pLine);
  REAL magV = v.mag();
  v.norm();

  //  line as vector
  Vector2D vLine;
  vLine.x = b;
  vLine.y = -a;
  vLine.norm();

  //  dot product projects point onto line
  REAL dt = v.dot(vLine);
  p->x = pLine.x + dt * vLine.x * magV;
  p->y = pLine.y + dt * vLine.y * magV;
}
