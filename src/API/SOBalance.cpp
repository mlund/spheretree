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

#include "SOBalance.h"
#include "SRBurst.h"
#include "SRMerge.h"
#include "SFWhite.h"

SOBalance::SOBalance() {
  A = 1;
  V = 0;
  B = 0;
}

float SOBalance::computeWorstErr(Array<Sphere> &sph) const {
  int numSph = sph.getSize();
  float worstErr = -1;
  for (int i = 0; i < numSph; i++) {
    float e = sphereEval->evalSphere(sph.index(i));
    if (e > worstErr)
      worstErr = e;
  }
  return worstErr;
}

void SOBalance::optimise(Array<Sphere> *spheres, const SurfaceRep &surRep,
                         float stopBelow, const Sphere *parSph,
                         int level) const {
  //  optimise the set of spheres to start with
  optimiser->optimise(spheres, surRep, -1, parSph);
  int initNum = spheres->getSize();
  if (initNum <= 2)
    return;

  //  compute the worst error to start with
  float firstErr = computeWorstErr(*spheres);
  OUTPUTINFO("numSpheres = %d, error = %f\n", initNum, firstErr);

  //  setup burster
  SFWhite fitterWhite;
  SRBurst burster;
  burster.sphereFitter = &fitterWhite;
  burster.sphereEval = sphereEval;

  //  successive bursts
  bool fullyOpt = true;
  double vComp = V;
  for (int i = initNum - 1; i > 2; i--) {
    //  work out the allowed error
    double allowExtra =
        (V < EPSILON) ? (0) : ((double)(initNum - i) / (A * initNum) * vComp);
    double divFact = 1;
    OUTPUTINFO("DivFact = %d\n", divFact);
    double allowError = firstErr * (1 + (allowExtra + B) / divFact);
    vComp *= V;

    //  do reduction
    Array<Sphere> testSph;
    burster.setup(*spheres);
    burster.getSpheres(&testSph, i, surRep);
    int numSph = testSph.getSize();

    //  compute error
    bool thisSetOpt = false;
    float err = computeWorstErr(testSph);
    if (err >= allowError) {
      OUTPUTINFO("Error is too large trying to optimise %f\n", err);
      optimiser->optimise(&testSph, surRep, allowError, parSph);
      err = computeWorstErr(testSph);
      thisSetOpt = true;
    }
    OUTPUTINFO("allowErr = %f (%f)\n", allowError, allowExtra);
    OUTPUTINFO("numSph = %d err = %f\n", numSph, err);

    if (err > allowError) {
      //  can't accept that change as too much error is introduced
      OUTPUTINFO("Nah we will stick with what we had (%d)\n", numSph + 1);
      break;
    } else {
      //  change is ok - save spheres
      spheres->clone(testSph);
      fullyOpt = thisSetOpt;
    }
  }

  if (!fullyOpt)
    optimiser->optimise(spheres, surRep, -1, parSph);
}
