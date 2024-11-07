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
#include <memory.h>
#include "Angles.h"
#include "Transform2D.h"
#include "../LinearAlgebra/Matrix.h"
#include "../Exceptions/CheckDebug.h"

void Transform2D::identity() {
  data[0][0] = 1;
  data[0][1] = 0;
  data[0][2] = 0;
  data[1][0] = 0;
  data[1][1] = 1;
  data[1][2] = 0;
  data[2][0] = 0;
  data[2][1] = 0;
  data[2][2] = 1;
}

void Transform2D::setup(REAL ang, REAL tX, REAL tY) {
  data[2][0] = 0;
  data[2][1] = 0;
  data[2][2] = 1;
  setTranslate(tX, tY);
  setAngle(ang);
}

void Transform2D::setAngle(REAL ang) {
  ang = (REAL)RAD(ang);
  REAL ca = (REAL)cos(ang), sa = (REAL)sin(ang);
  data[0][0] = ca;
  data[0][1] = -sa;
  data[1][0] = sa;
  data[1][1] = ca;
}

void Transform2D::transform(Point2D *dest, const Point2D &src) const {
  CHECK_DEBUG(dest != NULL, "NULL Destination");

  dest->x = data[0][0] * src.x + data[0][1] * src.y + data[0][2];
  dest->y = data[1][0] * src.x + data[1][1] * src.y + data[1][2];
}

void Transform2D::transform(Vector2D *dest, const Vector2D &src) const {
  CHECK_DEBUG(dest != NULL, "NULL Destination");

  dest->x = data[0][0] * src.x + data[0][1] * src.y;
  dest->y = data[1][0] * src.x + data[1][1] * src.y;
}

void Transform2D::compose(const Transform2D &t1, const Transform2D &t2) {
  //  would be nicer to derive equation
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++) {
      REAL val = 0.0;

      for (int k = 0; k < 3; k++)
        val += t1.data[row][k] * t2.data[k][col];

      data[row][col] = val;
    }
}

void Transform2D::compose(const Transform2D &t1) {
  Transform2D trTmp(*this);
  compose(t1, trTmp);
}

bool Transform2D::invert(const Transform2D &t) {
  Matrix m(3, 3), mI(3, 3);

  m.index(0, 0) = t.data[0][0];
  m.index(0, 1) = t.data[0][1];
  m.index(0, 2) = t.data[0][2];
  m.index(1, 0) = t.data[1][0];
  m.index(1, 1) = t.data[1][1];
  m.index(1, 2) = t.data[1][2];
  m.index(2, 0) = t.data[2][0];
  m.index(2, 1) = t.data[2][1];
  m.index(2, 2) = t.data[2][2];

  if (!mI.invert(m))
    return false;

  data[0][0] = mI.index(0, 0);
  data[0][1] = mI.index(0, 1);
  data[0][2] = mI.index(0, 2);
  data[1][0] = mI.index(1, 0);
  data[1][1] = mI.index(1, 1);
  data[1][2] = mI.index(1, 2);
  data[2][0] = mI.index(2, 0);
  data[2][1] = mI.index(2, 1);
  data[2][2] = mI.index(2, 2);
  return true;
}

bool Transform2D::invertAffine(const Transform2D &t) {
  //  assume affine
  Transform2D rotT;
  rotT.identity();

  for (int i = 0; i < 2; i++) //  transpose rotation
    for (int j = 0; j < 2; j++)
      rotT.index(i, j) = t.index(j, i);

  Transform2D trN;
  trN.identity();
  trN.index(0, 2) = -t.index(0, 2); //  negate translation
  trN.index(1, 2) = -t.index(1, 2);

  compose(rotT, trN);
  return true;
}
