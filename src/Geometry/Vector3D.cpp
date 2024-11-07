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
#include "Vector3D.h"

//  statics
const Vector3D Vector3D::ZERO = {0, 0, 0};
const Vector3D Vector3D::X = {1, 0, 0};
const Vector3D Vector3D::Y = {0, 1, 0};
const Vector3D Vector3D::Z = {0, 0, 1};

//  normalize
void Vector3D::norm() {
  REAL m = mag();
  if (m > EPSILON) {
    x /= m;
    y /= m;
    z /= m;
  }
}

void Vector3D::norm(const Vector3D &v) {
  REAL m = v.mag();
  if (m > EPSILON) {
    x = v.x / m;
    y = v.y / m;
    z = v.z / m;
  }
}

REAL Vector3D::getAngle(const Vector3D &v) const {
  Vector3D v1, v2;
  v1.norm(*this);
  v2.norm(v);

  float dt = v1.dot(v2);
  if (dt > 1)
    dt = 1;
  else if (dt < -1)
    dt = -1;
  return DEG(acos(dt));
}
