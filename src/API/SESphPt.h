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
    Expensive Sphere Evaluator which finds the distance of a set of points from
   the surface and returns the worst of these values.

    Sphere samples are distrinbuted over a sphere of unit radius
    centered about the origin.
*/
#ifndef _API_SPHERE_POINT_SPHERE_EVALUATOR_H_
#define _API_SPHERE_POINT_SPHERE_EVALUATOR_H_

#include "SEBase.h"
#include "../Storage/Array.h"
#include "../Geometry/Point3D.h"
#include "../Surface/SurfaceTester.h"

class SESphPt : public SEBase {
public:
  //  constructor
  SESphPt();
  SESphPt(const SurfaceTester &st, const Array<Point3D> &sphSamples);

  //  setup
  void setup(const SurfaceTester &st, const Array<Point3D> &sphSamples);

  //  evaluate the fit of the given sphere
  float evalSphere(const Sphere &s) const;

private:
  const SurfaceTester *st;
  const Array<Point3D> *sphSamples;
};

#endif