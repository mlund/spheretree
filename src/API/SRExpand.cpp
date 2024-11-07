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

#include "SRExpand.h"
#include "../Export/POV.h"

SRExpand::SRExpand() {
  vorAdapt = NULL;
  redElim = NULL;
  vor = NULL;
  mt = NULL;
  initSpheres = 0;
  minSpheresPerNode = 0;
  errorDecreaseFactor = 2;
  maxItersForVoronoi = 100;
}

void SRExpand::setup(Voronoi3D *vor, const MedialTester *mt) {
  this->vor = vor;
  this->mt = mt;
}

void SRExpand::setupForLevel(int level, int degree,
                             const SurfaceRep *surRep) const {
  if (vor && initSpheres > 0 && level == 0) {
    //  FOR : update diagram using filter sphere
    OUTPUTINFO("Updating VORONOI...\n");

    VFAdaptive adaptive;
    VFAdaptive *vorAdaptPtr = vorAdapt;
    if (!vorAdapt) {
      vorAdaptPtr = &adaptive;
      adaptive.mt = mt;
    }

    vorAdaptPtr->adaptiveSample(vor, 0, -1, initSpheres, 0, surRep, NULL, true);
  }
}

//  overload this to get the spheres
bool SRExpand::generateStandOffSpheres(Array<Sphere> *spheres, float err,
                                       const SurfaceRep &surRep, int maxNum,
                                       int tryIter,
                                       const Sphere *parSph) const {
  CHECK_DEBUG(redElim != NULL, "need redundency eliminator");
  spheres->setSize(0);

  Array<Sphere> expSph;
  generateExpandedSpheres(&expSph, err, parSph);
  if (maxNum < 0)
    maxNum = expSph.getSize() + 1;
  /*
    //  save off the spheres for illustrations
    static int frame = 0;
    frame++;
    char buffer[1024];
    sprintf(buffer, "c:/devel/expandPics/expand-%.4d-A.pov", frame);
    exportSpheresPOV(buffer, expSph, 1.0f/1000.0f, true);
  */
  if (tryIter == 0) {
    // Array<int> sel;
    bool ok = redElim->reduceSpheres(spheres, expSph, surRep, maxNum,
                                     NULL /*, &sel*/);

    /*
        //  save off spheres for illustration
        Array<int> fakeSel;
        Array<Sphere> selSph;
        for (int i = 0; i < sel.getSize(); i++){
          fakeSel.addItem() = i;
          selSph.addItem() = expSph.index(sel.index(i));
          }

        float selColor[] = {1,0,0};
        sprintf(buffer, "c:/devel/expandPics/expand-%.4d-B.pov", frame);
        exportSpheresPOV(buffer, selSph, 1.0f/1000.0f, true, NULL, &fakeSel,
       selColor);
    */
    return ok;
  } else {
    //  setup eliminator
    if (!redElim->setup(expSph, surRep))
      return false;

    Array<int> inds;
    Array<double> mets;
    double maxMet = DBL_MAX;
    bool result = false;
    bool endEarly = tryIter != 1;
    while (redElim->reduceSpheres(&inds, maxNum + 5, NULL, maxMet, &mets)) {
      OUTPUTINFO("Got %d Spheres\n", inds.getSize());
      if (inds.getSize() <= maxNum) {
        //  store the spheres
        redElim->getSpheres(spheres, inds);
        result = true;

        //  do we finish
        if (endEarly)
          break;
      }

      //  set maximum for metric
      double newMaxMet = mets.index(0);
      OUTPUTINFO("Metric changing from %g to %g\n", maxMet, newMaxMet);
      maxMet = newMaxMet;
    }

    //  tidy up
    redElim->tidyUp();
    return result;
  }
}

void SRExpand::generateExpandedSpheres(Array<Sphere> *dest, float err,
                                       const Sphere *parSph) const {
  CHECK_DEBUG0(mt != NULL);

  //  reset base set of spheres
  dest->setSize(0);

  //  get internal vertices
  int num = vor->vertices.getSize();
  for (int i = 0; i < num; i++) {
    //  find out if the sphere is internal
    Voronoi3D::Vertex *vert = &vor->vertices.index(i);
    if (mt->isMedial(vert)) {
      Point3D pClose;
      mt->closestPoint(&pClose, vert);

      float d = pClose.distance(vert->s.c);
      float newR = d + err;
      if (vert->flag == VOR_FLAG_COVER) {
        newR = err - d;
        if (newR < 0)
          newR = 0;
      }

      Sphere s;
      s.assign(vert->s.c, newR);
      if (newR > 0 && (!parSph || parSph->overlap(s)))
        dest->addItem() = s;
    }
  }
}

bool SRExpand::setupFilterSphere(const Sphere *filterSphere, float parSphereErr,
                                 const SurfaceRep *surRep) const {
  //  FOR : update diagram using filter sphere
  if (mt && vor && parSphereErr > 0 && filterSphere) {
    VFAdaptive adaptive;
    VFAdaptive *vorAdaptPtr = vorAdapt;
    if (!vorAdapt) {
      vorAdaptPtr = &adaptive;
      adaptive.mt = mt;
    }

    float destErr = parSphereErr / errorDecreaseFactor;
    OUTPUTINFO("Updating VORONOI... (DestErr = %f)\n", destErr);
    vorAdaptPtr->adaptiveSample(vor, destErr, -1, -1, minSpheresPerNode, surRep,
                                filterSphere, true, maxItersForVoronoi);
  }

  return true;
}
