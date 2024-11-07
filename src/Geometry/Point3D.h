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
    Definition of a point in 3D Euclidean Space
*/
#ifndef _POINT_3D_H_
#define _POINT_3D_H_

#include "../Base/Types.h"
#include <math.h>

//  Data Type
struct Point3D {
  REAL x, y, z;

  //  assignment
  __inline void assign(REAL xN, REAL yN, REAL zN) {
    x = xN;
    y = yN;
    z = zN;
  }

  __inline void assign(const Point3D &p) {
    x = p.x;
    y = p.y;
    z = p.z;
  }

  //  comparison
  __inline bool equals(const Point3D &p) const {
    return fabs(x - p.x) < EPSILON && fabs(y - p.y) < EPSILON &&
           fabs(z - p.z) < EPSILON;
  }

  //  distance
  __inline REAL distanceSQR(const Point3D &p) const {
    REAL dX = p.x - x;
    REAL dY = p.y - y;
    REAL dZ = p.z - z;

    return dX * dX + dY * dY + dZ * dZ;
  }

  __inline REAL distance(const Point3D &p) const {
    return (REAL)sqrt(distanceSQR(p));
  }

  //  min/max store
  __inline void storeMax(const Point3D &p) {
    if (p.x > x)
      x = p.x;
    if (p.y > y)
      y = p.y;
    if (p.z > z)
      z = p.z;
  }

  __inline void storeMin(const Point3D &p) {
    if (p.x < x)
      x = p.x;
    if (p.y < y)
      y = p.y;
    if (p.z < z)
      z = p.z;
  }

  const static Point3D ZERO;
  const static Point3D MAX;
  const static Point3D MIN;
};

#endif
