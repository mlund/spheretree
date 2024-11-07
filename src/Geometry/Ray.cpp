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

#include "Ray.h"
#include <math.h>

bool intersectTriangle(const Point3D &o, const Vector3D &d, float *t,
                       const Point3D &p0, const Point3D &p1,
                       const Point3D &p2) {
  Vector3D e1, e2;
  e1.difference(p1, p0);
  e2.difference(p2, p0);

  Vector3D p;
  p.cross(d, e2);

  double a = e1.dot(p);
  if (fabs(a) < EPSILON)
    return false;

  double f = 1 / a;
  Vector3D s;
  s.difference(o, p0);
  double u = f * s.dot(p);
  if ((u < 0.0) || (u > 1.0))
    return false;

  Vector3D q;
  q.cross(s, e1);

  double v = f * d.dot(q);
  if ((v < 0.0) || ((u + v) > 1.0))
    return false;

  *t = f * e2.dot(q);
  return true;
}
