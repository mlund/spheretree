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
    Sphere Evaluator which defines the error in a sphere to be the worst
   distance from a set of sample points to the sphere along the surface normals
   at that point.

    As HUBBARD except we consider all surface points from which are contained in
   the sphere
*/
#ifndef _SPHERE_EVAL_POINT_H_
#define _SPHERE_EVAL_POINT_H_

#include "SEBase.h"
#include "../Surface/Surface.h"

class SEPoint : public SEBase {
public:
  //  constructors
  SEPoint();
  SEPoint(const Array<Surface::Point> &samples);

  //  evaluate the fit of the given sphere
  float evalSphere(const Sphere &s) const;
  float evalSphere(const Sphere &s, const Array<Surface::Point> &surPts,
                   const Array<int> &inds) const;

  //  setup the set of sample points
  void setSurfaceSamples(const Array<Surface::Point> &samples);

private:
  //  reference to the set of samples to use for the evaluations
  const Array<Surface::Point> *samples;

  //  eval for a single point
  float eval(const Sphere &s, const Point3D &p, const Vector3D &n) const;
};

#endif