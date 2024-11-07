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

#include <math.h>
#include "Angles.h"
#include "Transform3D.h"
#include "../LinearAlgebra/Matrix.h"
#include "../Exceptions/CheckDebug.h"

void Transform3D::identity() {
  data[0][0] = 1;
  data[0][1] = 0;
  data[0][2] = 0;
  data[0][3] = 0;
  data[1][0] = 0;
  data[1][1] = 1;
  data[1][2] = 0;
  data[1][3] = 0;
  data[2][0] = 0;
  data[2][1] = 0;
  data[2][2] = 1;
  data[2][3] = 0;
  data[3][0] = 0;
  data[3][1] = 0;
  data[3][2] = 0;
  data[3][3] = 1;
}

void Transform3D::setAngleX(REAL ang) {
  identity();
  ang = (REAL)RAD(ang);
  REAL ca = (REAL)cos(ang), sa = (REAL)sin(ang);
  data[1][1] = ca;
  data[1][2] = -sa;
  data[2][1] = sa;
  data[2][2] = ca;
}

void Transform3D::setAngleY(REAL ang) {
  identity();
  ang = (REAL)RAD(-ang);
  REAL ca = (REAL)cos(ang), sa = (REAL)sin(ang);
  data[0][0] = ca;
  data[0][2] = -sa;
  data[2][0] = sa;
  data[2][2] = ca;
}

void Transform3D::setAngleZ(REAL ang) {
  identity();
  ang = (REAL)RAD(ang);
  REAL ca = (REAL)cos(ang), sa = (REAL)sin(ang);
  data[0][0] = ca;
  data[0][1] = -sa;
  data[1][0] = sa;
  data[1][1] = ca;
}

void Transform3D::transform(Point3D *dest, const Point3D &src) const {
  CHECK_DEBUG(dest != NULL, "NULL Destination");

  dest->x =
      data[0][0] * src.x + data[0][1] * src.y + data[0][2] * src.z + data[0][3];
  dest->y =
      data[1][0] * src.x + data[1][1] * src.y + data[1][2] * src.z + data[1][3];
  dest->z =
      data[2][0] * src.x + data[2][1] * src.y + data[2][2] * src.z + data[2][3];
}

void Transform3D::transform(Vector3D *dest, const Vector3D &src) const {
  CHECK_DEBUG(dest != NULL, "NULL Destination");

  dest->x = data[0][0] * src.x + data[0][1] * src.y + data[0][2] * src.z;
  dest->y = data[1][0] * src.x + data[1][1] * src.y + data[1][2] * src.z;
  dest->z = data[2][0] * src.x + data[2][1] * src.y + data[2][2] * src.z;
}

void Transform3D::compose(const Transform3D &t1, const Transform3D &t2) {
  //  would be nicer to derive equation
  for (int row = 0; row < 4; row++)
    for (int col = 0; col < 4; col++) {
      REAL val = 0.0;

      for (int k = 0; k < 4; k++)
        val += t1.data[row][k] * t2.data[k][col];

      data[row][col] = val;
    }
}

bool Transform3D::invert(const Transform3D &t) {
  Matrix m(4, 4), mI(4, 4);

  m.index(0, 0) = t.data[0][0];
  m.index(0, 1) = t.data[0][1];
  m.index(0, 2) = t.data[0][2];
  m.index(0, 3) = t.data[0][3];
  m.index(1, 0) = t.data[1][0];
  m.index(1, 1) = t.data[1][1];
  m.index(1, 2) = t.data[1][2];
  m.index(1, 3) = t.data[1][3];
  m.index(2, 0) = t.data[2][0];
  m.index(2, 1) = t.data[2][1];
  m.index(2, 2) = t.data[2][2];
  m.index(2, 3) = t.data[2][3];
  m.index(3, 0) = t.data[3][0];
  m.index(3, 1) = t.data[3][1];
  m.index(3, 2) = t.data[3][2];
  m.index(3, 3) = t.data[3][3];

  if (!mI.invert(m))
    return false;

  data[0][0] = mI.index(0, 0);
  data[0][1] = mI.index(0, 1);
  data[0][2] = mI.index(0, 2);
  data[0][3] = mI.index(0, 3);
  data[1][0] = mI.index(1, 0);
  data[1][1] = mI.index(1, 1);
  data[1][2] = mI.index(1, 2);
  data[1][3] = mI.index(1, 3);
  data[2][0] = mI.index(2, 0);
  data[2][1] = mI.index(2, 1);
  data[2][2] = mI.index(2, 2);
  data[2][3] = mI.index(2, 3);
  data[3][0] = mI.index(3, 0);
  data[3][1] = mI.index(3, 1);
  data[3][2] = mI.index(3, 2);
  data[3][3] = mI.index(3, 3);

  return true;
}

bool Transform3D::invertAffine(const Transform3D &tr) {
  Transform3D rotnT;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++)
      rotnT.data[i][j] = tr.data[j][i]; //  transpose rotnT
    rotnT.data[i][3] = 0;
    rotnT.data[3][i] = 0;
  }
  rotnT.data[3][3] = 1;

  Transform3D transI;
  for (int i = 0; i < 4; i++) {
    int j;
    for (j = 0; j < 3; j++)
      if (i == j)
        transI.data[i][j] = 1;
      else
        transI.data[i][j] = 0;

    transI.data[i][j] = -tr.data[i][3];
  }
  transI.data[3][3] = 1;

  compose(rotnT, transI);
  return true;
}

/*
    mapping onto 2D plane by: rotating a vector to lie along the Z axis
*/
void Transform3D::alignToZ(/*const*/ Vector3D /*&*/ v) {
  v.norm();
  REAL r = (REAL)sqrt(v.x * v.x + v.z * v.z);

  REAL cY, sY; //  sin(ThetaY)
  if (r < EPSILON_LARGE) {
    REAL aY = -atan2(v.x, v.z);
    cY = cos(aY);
    sY = sin(aY);
  } else {
    cY = v.z / r;
    sY = -v.x / r;
  }

  Transform3D tY; //  rotation about y (-ve)
  tY.data[0][0] = cY;
  tY.data[0][1] = 0;
  tY.data[0][2] = sY;
  tY.data[0][3] = 0;
  tY.data[1][0] = 0;
  tY.data[1][1] = 1;
  tY.data[1][2] = 0;
  tY.data[1][3] = 0;
  tY.data[2][0] = -sY;
  tY.data[2][1] = 0;
  tY.data[2][2] = cY;
  tY.data[2][3] = 0;
  tY.data[3][0] = 0;
  tY.data[3][1] = 0;
  tY.data[3][2] = 0;
  tY.data[3][3] = 1;

  REAL cX = r, sX = v.y; //  sin(ThetaX)

  Transform3D tX; //  rotation about x
  tX.data[0][0] = 1;
  tX.data[0][1] = 0;
  tX.data[0][2] = 0;
  tX.data[0][3] = 0;
  tX.data[1][0] = 0;
  tX.data[1][1] = cX;
  tX.data[1][2] = -sX;
  tX.data[1][3] = 0;
  tX.data[2][0] = 0;
  tX.data[2][1] = sX;
  tX.data[2][2] = cX;
  tX.data[2][3] = 0;
  tX.data[3][0] = 0;
  tX.data[3][1] = 0;
  tX.data[3][2] = 0;
  tX.data[3][3] = 1;

  compose(tX, tY); //  tX after tY
}

void Transform3D::alignToZ(const Vector3D &v, const Point3D &p) {
  alignToZ(v);

  Point3D pT;
  transform(&pT, p);
  data[0][3] = -pT.x;
  data[1][3] = -pT.y;
  data[2][3] = -pT.z;
}

void Transform3D::transformTo2D(Point2D *p, const Point3D &q) const {
  Point3D tmp;
  transform(&tmp, q);
  p->x = tmp.x;
  p->y = tmp.y;
}

void Transform3D::transformFrom2D(Point3D *p, const Point2D &q) const {
  Point3D tmp;
  tmp.x = q.x;
  tmp.y = q.y;
  tmp.z = 0;

  transform(p, tmp);
}
