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

#include "SRSpawn.h"
#include "../MinMax/Simplex.h"
#include "../Export/POV.h"

SRSpawn::SRSpawn() {
  st = NULL;
  eval = NULL;
}

void SRSpawn::setup(const SurfaceTester &st, const SEBase *se) {
  this->st = &st;
  this->eval = se;
}

bool SRSpawn::generateSpheres(Array<Sphere> *sph, float err,
                              const SurfaceRep &surRep, int maxNum,
                              float maxRad) const {
  CHECK_DEBUG(st != NULL, "need tester");

  const Array<Surface::Point> *surPts = surRep.getSurPts();
  int numPts = surPts->getSize();

  sph->setSize(0);

  //  flags for points contained in the selected set
  Array<bool> cont;
  cont.resize(numPts);
  cont.clear();
  int numCont = 0;

  //  setup optimiser
  OptInfo optInf;
  optInf.maxErr = err;
  optInf.ptsCont = &cont;
  optInf.st = st;
  optInf.sph = sph;
  optInf.surRep = &surRep;
  optInf.maxRad = maxRad;
  optInf.eval = eval;

  //  flags for starting points
  Array<bool> tried;
  tried.resize(numPts);
  tried.clear();

  bool needNew = true;
  while (numCont < numPts) {
    Sphere newS = Sphere::INVALID;

    if (!needNew) {
      //  use last sphere as starting point
      newS = sph->index(sph->getSize() - 1);
      newS.r -= err / 10;
    } else {
      //  create initial sphere (guarantee it will be ok)
      int i;
      for (i = 0; i < numPts; i++)
        if (!cont.index(i) && !tried.index(i))
          break;

      //  setup sphere around the uncovered point
      if (i < numPts) {
        tried.index(i) = true;
        newS.c = surPts->index(i).p;
        newS.r = err * 0.95;
      }
    }
    if (newS.r < 0)
      return false; //  no starting sphere - must be done (failed)

    //  do optimisation
    long lastUnc = -1;
    int numFunc = 100;
    double vals[4] = {newS.c.x, newS.c.y, newS.c.z, newS.r};
    double sizes[4] = {10 * err, 10 * err, 10 * err, 10 * err};
    int dims = eval ? 4 : 3;
    simplexMin(dims, vals, sizes, EPSILON_LARGE, &numFunc, spawnFunc, &optInf,
               5);

    //  generate the final sphere
    if (eval) {
      newS.c.x = vals[0];
      newS.c.y = vals[1];
      newS.c.z = vals[2];
      newS.r = vals[3];
    } else {
      newS = makeSphereSpawn(vals, err, *st);
      if (newS.r <= 0)
        newS.r = err;
      else if (maxRad > 0 && newS.r > maxRad)
        newS.r = maxRad;
    }

    //  make list of points contained in this sphere which are not already
    //  covered
    int newCover = surRep.flagContainedPoints(NULL, newS, &cont);

    if (newCover < 2) {
      needNew = true;
    } else {
      //  flag covered points
      surRep.flagContainedPoints(&cont, newS, &cont);
      numCont += newCover;

      //  record the sphere
      sph->addItem() = newS;
      needNew = false;
      if (maxNum > 0 && sph->getSize() > maxNum)
        return false;
    }
  }

  return true;
}

bool SRSpawn::generateStandOffSpheres(Array<Sphere> *sph, float err,
                                      const SurfaceRep &surRep, int maxNum,
                                      int tryIter, const Sphere *parSph) const {
  if (tryIter == 0) {
    return generateSpheres(sph, err, surRep, maxNum);
  } else {
    Array<Sphere> tmpSph;
    bool endEarly = (tryIter != 1);
    bool gotNum = false;

    if (generateSpheres(&tmpSph, err, surRep, maxNum + 5)) {
      if (tmpSph.getSize() <= maxNum) {
        sph->clone(tmpSph);
        gotNum = true;

        if (endEarly)
          return true;
      }

      while (true) {
        //  find the maximum radius of the current set of spheres
        float maxR = -1;
        int numSpheres = tmpSph.getSize();
        for (int i = 0; i < numSpheres; i++) {
          float r = tmpSph.index(i).r;
          if (r > maxR)
            maxR = r;
        }

        //  generate the set of spheres with new max radius
        float newMaxR = 0.9995 * maxR - EPSILON_LARGE;
        if (!generateSpheres(&tmpSph, err, surRep, maxNum + 5, newMaxR))
          break;

        numSpheres = tmpSph.getSize();
        OUTPUTINFO("newMaxR = %f (%d spheres)\n", newMaxR, numSpheres);
        if (numSpheres <= maxNum) {
          gotNum = true;
          sph->clone(tmpSph);
          if (numSpheres <= maxNum || endEarly)
            break;
        }
      }
      return gotNum;
    } else {
      return false;
    }
  }
}

Sphere SRSpawn::makeSphereSpawn(double vals[4], float maxErr,
                                const SurfaceTester &st) {
  Sphere newS;
  newS.c.x = vals[0];
  newS.c.y = vals[1];
  newS.c.z = vals[2];

  float newR = 0;
  Point3D pClose;
  bool internal = st.insideSurface(&pClose, newS.c);
  CHECK_DEBUG0(finite(pClose.x + pClose.y + pClose.z));
  if (internal)
    newR = pClose.distance(newS.c) + maxErr;
  else
    newR = maxErr - pClose.distance(newS.c);

  newS.r = newR;
  return newS;
}

double SRSpawn::spawnFunc(double vals[4], void *data, int *canFinish) {
  const OptInfo *optInf = (OptInfo *)data;

  //  create new sphere
  Sphere newS;
  if (optInf->eval) {
    newS.c.x = vals[0];
    newS.c.y = vals[1];
    newS.c.z = vals[2];
    newS.r = vals[3];
    if (newS.r <= 0 || (optInf->maxRad > 0 && newS.r > optInf->maxRad))
      return DBL_MAX;

    //  evaluate the error
    double err = optInf->eval->evalSphere(newS);
    if (err > optInf->maxErr)
      return err;
  } else {
    newS = makeSphereSpawn(vals, optInf->maxErr, *optInf->st);
    if (optInf->maxRad > 0 && newS.r > optInf->maxRad)
      newS.r = optInf->maxRad;
    if (newS.r <= 0) {
      return DBL_MAX;
    }
  }

  //  make list of points contained in this sphere which are not already covered
  int numUnc = optInf->surRep->flagContainedPoints(NULL, newS, optInf->ptsCont);
  return -numUnc; //  minimiser
}
