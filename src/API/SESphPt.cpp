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

#include "SESphPt.h"
#include "SEConvex.h"

//  constructor
SESphPt::SESphPt() { sphSamples = NULL; }

SESphPt::SESphPt(const SurfaceTester &st, const Array<Point3D> &sphSamples) {
  setup(st, sphSamples);
}

//  setup
void SESphPt::setup(const SurfaceTester &st, const Array<Point3D> &sphSamples) {
  this->st = &st;
  this->sphSamples = &sphSamples;
}

//  evaluate the fit of the given sphere
float SESphPt::evalSphere(const Sphere &s) const {
  CHECK_DEBUG(st != NULL, "Surface needed : use setup");
  CHECK_DEBUG(sphSamples, "Sphere points needed : use setup");
  /*
    SEConvex conv;
    conv.setTester(*this->st);
    float convD = conv.evalSphere(s);
    convD = convD*convD;   //  we are working in squares now
  */
  //  find the worst or the closest distances
  float maxD = 0;
  int numPts = sphSamples->getSize();
  for (int i = 0; i < numPts; i++) {
    //  transform the sphere point from unit spher to S
    Point3D pUnit = sphSamples->index(i);

    Point3D pTest;
    pTest.x = s.c.x + s.r * pUnit.x;
    pTest.y = s.c.y + s.r * pUnit.y;
    pTest.z = s.c.z + s.r * pUnit.z;

    //  find closest point to this sample
    Point3D pClose;
    pClose.x = pClose.y = pClose.z = acos(10);
    bool internal = st->getClosestPointConditional(&pClose, pTest, false, maxD);
    if (!internal) { //  will stop and return true when less than maxD
      CHECK_DEBUG0(finite(pClose.x));
      float d = pClose.distance(pTest);
      if (d > maxD) {
        maxD = d;
        /*
                if (d > convD * 1.01){
                  OUTPUTINFO("Aaaarrrrggghhh %f %f %f\n", d-convD, d, convD);

                  //  find closest point to the center of the sphere using both
           methods ClosestPointInfo inf1, inf2; getClosestPoint(&inf1, s.c,
           *st->getSurface(), st->getFaceHash()); getClosestPointBrute(&inf2,
           s.c, *st->getSurface()); OUTPUTINFO("Closest Point To Center Off By :
           %f\n", inf1.pClose.distance(inf2.pClose));

                  //  find closest point to the test point using both methods
                  getClosestPoint(&inf1, pTest, *st->getSurface(),
           st->getFaceHash()); getClosestPointBrute(&inf2, pTest,
           *st->getSurface()); OUTPUTINFO("Closest Point To pTest Off By :
           %f\n", inf1.pClose.distance(inf2.pClose));
                  }*/
      }
    }
  }

  return maxD;
}