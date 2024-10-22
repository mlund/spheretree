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
#include "Vector2D.h"

//  statics
const Vector2D Vector2D::ZERO = {0, 0};

//  angle
REAL Vector2D::angle(const Vector2D &v) const {
  Vector2D tmp1, tmp2;
  tmp1.norm(*this);
  tmp2.norm(v);

  REAL dt = tmp1.dot(tmp2);
  if (dt < -1)
    dt = 1;
  else if (dt > 1)
    dt = 1;

  return (REAL)DEG(acos(dt));
}

REAL Vector2D::angle() const { return (REAL)DEG(atan2(y, x)); }

//  normalize
void Vector2D::norm() {
  REAL m = mag();
  if (m > EPSILON) {
    x /= m;
    y /= m;
  }
}

void Vector2D::norm(const Vector2D &v) {
  REAL m = v.mag();

  if (m > EPSILON) {
    x = v.x / m;
    y = v.y / m;
  }
}
