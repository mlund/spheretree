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

#include "SRStandOff.h"
#include "../Base/Defs.h"
#include "../SphereTree/SphereTree.h"

SRStandOff::SRStandOff() {
  relTol = 1E-2;
  startErr = 0.0f;
  errStep = 0.5;
  stopExact = false;
  refitter = NULL;
  useIterativeSelect = false;
}

// this algorithm uses the binary search
void SRStandOff::getSpheresA(Array<Sphere> *spheres, int numDest,
                             const SurfaceRep &surRep,
                             const Sphere *filterSphere,
                             float parSphErr) const {
  if (!setupFilterSphere(filterSphere, parSphErr, &surRep))
    return;

  //  phase one - work out the upper bound
  Array<Sphere> tmpSph;
  float lowerErr = 0, upperErr = errStep;
  while (true) {
    OUTPUTINFO("upperErr = %f\n", upperErr);
    bool flag =
        generateStandOffSpheres(&tmpSph, upperErr, surRep, numDest,
                                useIterativeSelect ? 2 : 0, filterSphere);
    if (flag && tmpSph.getSize() <= numDest)
      break;

    OUTPUTINFO("%d spheres\n", tmpSph.getSize());
    upperErr *= 2;
  }

  //  phase two - binary search
  float lastGoodError = upperErr;
  while (true) {
    float nextErr = (upperErr + lowerErr) / 2.0f;
    float errRange = upperErr - lowerErr;

    OUTPUTINFO("**************************\n");
    OUTPUTINFO("err  \t: %f\n", nextErr);
    OUTPUTINFO("range\t: %f\n", errRange);

    bool flag =
        generateStandOffSpheres(&tmpSph, nextErr, surRep, numDest,
                                useIterativeSelect ? 2 : 0, filterSphere);
    if (flag && tmpSph.getSize() <= numDest) {
      OUTPUTINFO("#sph\t: %d \n", tmpSph.getSize());
      lastGoodError = nextErr;
      spheres->clone(tmpSph);
      upperErr = nextErr;

      if (errRange / lowerErr < relTol || errRange < 1E-3 ||
          (stopExact && spheres->getSize() == numDest)) {
        OUTPUTINFO("Done.\n");
        break;
      }
    } else {
      OUTPUTINFO("NO GOOD\n");
      lowerErr = nextErr;
    }
  }

  //  do proper reduction - no early stopping
  if (useIterativeSelect) {
    if (!generateStandOffSpheres(spheres, lastGoodError, surRep, numDest, 1,
                                 filterSphere))
      spheres->setSize(0);
  }

  OUTPUTINFO("%d spheres (%f)\n", spheres->getSize(), lastGoodError);
  ((SRStandOff *)this)->lastStandoff =
      lastGoodError; //  hack for graphing performance
}

//  this is the original search algorithm - has been replaced with binary search
//  for now
void SRStandOff::getSpheresB(Array<Sphere> *spheres, int numDest,
                             const SurfaceRep &surRep,
                             const Sphere *filterSphere,
                             float parSphErr) const {
  if (!setupFilterSphere(filterSphere, parSphErr, &surRep))
    return;

  //  iterative adaptive algorithm to find the error number requried
  //  to generate the correct number of spheres
  Array<Sphere> tmpSph;
  float delta = errStep;
  float errBelow = startErr, errAbove = REAL_MAX;
  while (true) {
    OUTPUTINFO("*********************\n");
    int numSph;
    float nextErr = errBelow + delta;

    if (nextErr >= errAbove) {
      //  will result in <= numDest
      numSph = -1;
    } else {
      bool flag =
          generateStandOffSpheres(&tmpSph, nextErr, surRep, numDest,
                                  useIterativeSelect ? 2 : 0, filterSphere);

      if (flag) {
        //  good coverage
        numSph = tmpSph.getSize();

        if (nextErr < errAbove &&
            numSph <= numDest) { //  value may be == numDest + 1
          errAbove = nextErr;
          spheres->clone(tmpSph);
        }
      } else {
        //  no coverage
        OUTPUTINFO("COVERAGE NOT VALID\n");
        numSph = numDest + 1;
      }
    }
    OUTPUTINFO("ERR = %f\n", nextErr);
    OUTPUTINFO("Delta = %f\n", delta);
    OUTPUTINFO("numSph = %d\n", numSph);

    //  update the step
    if (numSph > numDest) {
      errBelow = nextErr;
      nextErr += delta;

      //  test - to see if we will grow generate the set of expanded spheres
      if (nextErr < errAbove) {
        bool flag =
            generateStandOffSpheres(&tmpSph, nextErr, surRep, numDest,
                                    useIterativeSelect ? 2 : 0, filterSphere);

        if (!flag) {
          errBelow = nextErr;
          delta *= 2.0f;
        } else if (tmpSph.getSize() <= numDest) {
          spheres->clone(tmpSph);
          errAbove = nextErr;
        }
      }
    } else {
      if (delta / errBelow < relTol || delta < 1E-3f ||
          (stopExact && spheres->getSize() == numDest)) {
        OUTPUTINFO("Done.....\n");
        break;
      } else {
        delta /= 2.0f;
      }
    }
  }

  //  do proper reduction - no early stopping
  if (useIterativeSelect) {
    if (!generateStandOffSpheres(spheres, errAbove, surRep, numDest, 1,
                                 filterSphere))
      spheres->setSize(0);
  }

  OUTPUTINFO("%d spheres (%f)\n", spheres->getSize(), errAbove);
  ((SRStandOff *)this)->lastStandoff =
      errAbove; //  hack for graphing performance
}

void SRStandOff::getSpheres(Array<Sphere> *spheres, int numDest,
                            const SurfaceRep &surRep,
                            const Sphere *filterSphere, float parSphErr) const {
  getSpheresB(spheres, numDest, surRep, filterSphere, parSphErr);
}
