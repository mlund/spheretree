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
    Definition of a point in 2D Euclidean Space
*/
#ifndef _POINT_2D_H_
#define _POINT_2D_H_

#include "../Base/Types.h"
#include <math.h>

//  Data Type
struct Point2D {
  REAL x, y;

  //  assignment
  __inline void assign(REAL xN, REAL yN) {
    x = xN;
    y = yN;
  }

  __inline void assign(const Point2D &p) {
    x = p.x;
    y = p.y;
  }

  //  comparison
  __inline bool equals(const Point2D &p) const {
    return fabs(x - p.x) < EPSILON && fabs(y - p.y) < EPSILON;
  }

  //  distance
  __inline REAL distanceSQR(const Point2D &p) const {
    REAL dX = p.x - x;
    REAL dY = p.y - y;

    return dX * dX + dY * dY;
  }

  __inline REAL distance(const Point2D &p) const {
    return (REAL)sqrt(distanceSQR(p));
  }

  //  max/min store
  __inline void storeMax(const Point2D &p) {
    if (p.x > x)
      x = p.x;
    if (p.y > y)
      y = p.y;
  }

  __inline void storeMin(const Point2D &p) {
    if (p.x < x)
      x = p.x;
    if (p.y < y)
      y = p.y;
  }

  //  constants
  const static Point2D ZERO;
  const static Point2D MAX;
  const static Point2D MIN;
};

#endif
