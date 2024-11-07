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

#include "SFMinErr.h"
#include "SEConvex.h"
#include "SOPerSphere.h"

SFMinErr::SFMinErr() { eval = NULL; }

bool SFMinErr::makeSphere(Sphere *s, const Array<Surface::Point> &points,
                          const Array<int> &inds, const SEBase *eval) {
  Array<Point3D> pts;
  convertPoints(&pts, points, inds);
  return makeSphere(s, pts, eval);
}

bool SFMinErr::makeSphere(Sphere *s, const Array<Surface::Point> &points,
                          const SEBase *eval) {
  Array<Point3D> pts;
  convertPoints(&pts, points);
  return makeSphere(s, pts, eval);
}

bool SFMinErr::makeSphere(Sphere *s, const Array<Point3D> &points,
                          const SEBase *eval) {
  //  build point list
  int numInds = points.getSize();
  if (numInds < 2) {
    if (numInds == 1)
      s->c = points.index(0);
    else
      s->c = Point3D::ZERO;

    s->r = 0;
    return true;
  }

  //  determine evaluator
  SEConvex evalConvex;
  const SEBase *localEval = eval;
  if (!eval)
    localEval = &evalConvex;

  SOPerSphere opt;
  opt.eval = localEval;
  opt.optimise(s, points);

  return finite(s->r);
}
