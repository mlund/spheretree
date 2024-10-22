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

#include "SRBurst.h"
#include "RELargest.h"
#include "../Export/POV.h"

#define ONE_SPHERE_PER_POINT

#define COST_FN(x) ((x) * (x) * (x) * (x) * (x) * (x))

SRBurst::SRBurst() {
  sphereFitter = NULL;
  sphereEval = NULL;
  doAllBelow = 20;
  doRedundantCheckBelow = -1;
  redElim = NULL;
  useFormingPoints = false;
  useBeneficial = true;
}

//  reduce sphere set
void SRBurst::getSpheres(Array<Sphere> *spheres, int numDest,
                         const SurfaceRep &surRep, const Sphere *filterSphere,
                         float parSphereErr) const {
  CHECK_DEBUG0(spheres != NULL);
  CHECK_DEBUG(sphereFitter != NULL, "need a sphere fitter");
  CHECK_DEBUG(sphereEval != NULL, "need a sphere evaluator");

#ifdef ONE_SPHERE_PER_POINT
  bool onlyOneSpherePerPoint =
      true; //  for speed each point is only ever contained in one sphere
#else
  bool onlyOneSpherePerPoint = false;
#endif

  //  make the set of medial spheres
  Array<MedialSphere> medialSpheres;
  constructSphereSet(&medialSpheres, surRep, filterSphere, parSphereErr,
                     onlyOneSpherePerPoint);
  int numSpheres = medialSpheres.getSize();

  //  reconfig is using forming points
  const Array<Surface::Point> *coverRep = surRep.getSurPts();
  Array<Surface::Point> localCoverPts;
  if (useFormingPoints) {
    //  setup local coverPts
    int numForming = vor->formingPoints.getSize();
    localCoverPts.resize(numForming);
    for (int i = 0; i < numForming; i++) {
      localCoverPts.index(i).p = vor->formingPoints.index(i).p;
      localCoverPts.index(i).n = vor->formingPoints.index(i).v;
    }

    //  replace the coverRep
    coverRep = &localCoverPts;

    //  replace point indices for the medial spheres
    for (int i = 0; i < numSpheres; i++) {
      //  get medial sphere
      MedialSphere *ms = &medialSpheres.index(i);

      //  get voronoi vertex
      Voronoi3D::Vertex *vert = &vor->vertices.index(ms->vertexNum);

      //  setup forming points
      ms->pts.resize(4);
      for (int i = 0; i < 4; i++)
        ms->pts.index(i) = vert->f[i];
    }
  }

  //  setup error records
  initErrors(&medialSpheres, sphereEval);

  //  build removal info
  Array<SphereRemove> removals;
  constructRemovals(&removals, medialSpheres, *coverRep);

  int numSph = medialSpheres.getSize();
  int numValid = numSph;
  bool doneAll = false;
  while (numValid > numDest) {
    //  let them all go wild at the end
    if (!doneAll && numValid <= doAllBelow) {
      makeAllNeighbours(&removals, &medialSpheres, *coverRep);
      doneAll = true;
    }

    int bestI = -1;
    float minCost = -1;
    if (useBeneficial) {
      //  find the worst sphere that makes an improvement
      double worstImprovedError = 0;
      for (int i = 0; i < numSph; i++) {
        const SphereRemove *r = &removals.index(i);
        const MedialSphere *ms = &medialSpheres.index(i);
        if (ms->valid && r->cost < 0) {
          double error = -1 * r->cost;
          if (error > worstImprovedError) {
            worstImprovedError = error;
            bestI = i;
          }
        }
      }
    }

    if (bestI < 0) {
      //  fall back to find lowest cost
      minCost = DBL_MAX;
      for (int i = 0; i < numSph; i++) {
        const SphereRemove *sr = &removals.index(i);
        const MedialSphere *ms = &medialSpheres.index(i);
        if (ms->valid && sr->cost < minCost) {
          bestI = i;
          minCost = sr->cost;
        }
      }
    }

    /*
        Array<int> ptMap;
        ptMap.clone(removals.index(bestI).pointMap);
        saveImagePre(medialSpheres, ptMap, bestI, 50);
    */

    if (numValid % 25 == 0)
      OUTPUTINFO("Removal %d spheres, Cheapest (%d) : %f\n", numValid, bestI,
                 minCost);
    applyRemoval(&removals, &medialSpheres, bestI, *coverRep,
                 (numValid - 1) > numDest);
    numValid--;
    /*
        saveImagePost(medialSpheres, ptMap, bestI, 50);
    */
    if (func)
      func(medialSpheres);

    if (numValid < doRedundantCheckBelow) { //  done use with useForming
      Array<Sphere> srcSph;
      int numSph = medialSpheres.getSize();
      for (int i = 0; i < numSph; i++)
        if (medialSpheres.index(i).valid)
          srcSph.addItem() = medialSpheres.index(i).s;

      RELargest elimLargest;
      REBase *elim = &elimLargest;
      if (this->redElim)
        elim = this->redElim;

      if (elim->reduceSpheres(spheres, srcSph, surRep, numDest))
        return;
    }
  }

  spheres->setSize(0);
  for (int i = 0; i < numSph; i++)
    if (medialSpheres.index(i).valid)
      spheres->addItem() = medialSpheres.index(i).s;
}

float SRBurst::costOfRemoval(SphereRemove &removal,
                             const Array<MedialSphere> &medialSpheres, int sI,
                             const Array<Surface::Point> &surPts) const {
  //  get info
  const MedialSphere *medSph = &medialSpheres.index(sI);
  int numPts = medSph->pts.getSize();

  //  compute the new bounding spheres for each neighbour
  float maxCost = 0, sumAllErr = 0;
  int numNeigh = medSph->neighbours.getSize();
  for (int i = 0; i < numNeigh; i++) {
    //  sphere for eval
    int testNum = medSph->neighbours.index(i);
    const MedialSphere *testS = &medialSpheres.index(testNum);
    CHECK_DEBUG(testS->valid,
                "costOfRemoval : a neighbouring sphere is invalid");

    //  build list of points
    Array<int> sphPts;
    sphPts.clone(testS->pts);

    bool haveNew = false;
    for (int j = 0; j < numPts; j++)
      if (removal.pointMap.index(j) == testNum) {
        int ptNum = medSph->pts.index(j);
        sphPts.addItem() = ptNum;
        haveNew = true;
      }

    //  eval cost
    if (haveNew) {
      Sphere newS;
      float cost = refitSphere(&newS, surPts, sphPts, sphereEval, sphereFitter,
                               &testS->s.c);

      //  add to total error
      sumAllErr += COST_FN(cost);

      // subtract existing error as it will be added again later
      // sumAllErr -= COST_FN(testS->error);  //  TEMP  --  only count updated
      // spheres

      //  store max
      if (cost > maxCost)
        maxCost = cost;
    }
  }

  //  add the rest of the errors
  /*  int numSph = medialSpheres.getSize();  //  TEMP  --  only count updated
    spheres for (i = 0; i < numSph; i++){ float err =
    medialSpheres.index(i).error; sumAllErr += COST_FN(err);
      }*/

  if (medSph->error < maxCost || !useBeneficial)
    return sumAllErr; //  maxCost doesn't perform as well here
  else
    return -1 * medSph->error;
}

void SRBurst::constructRemoval(SphereRemove *removal,
                               const Array<MedialSphere> &medialSpheres, int sI,
                               const Array<Surface::Point> &surPts) const {
  const MedialSphere *medSph = &medialSpheres.index(sI);
  int numPts = medSph->pts.getSize();

  //  work out which spheres to add points to
  removal->cost = -1;
  removal->pointMap.resize(numPts);
  for (int i = 0; i < numPts; i++) {
    int ptNum = medSph->pts.index(i);
    Point3D pTest = surPts.index(ptNum).p;

    //  find closest sphere from neighbours
    int minSph = -1;
    double minD = DBL_MAX;
    int numNeigh = medSph->neighbours.getSize();

    for (int j = 0; j < numNeigh; j++) {
      int sphNum = medSph->neighbours.index(j);
      CHECK_DEBUG(sphNum != sI,
                  "constructRemoval : i am in my own neighbour list");

      const MedialSphere *testS = &medialSpheres.index(sphNum);
      CHECK_DEBUG(testS->valid,
                  "constructRemoval : a neighbouring sphere is invalid");

      if (testS->s.contains(pTest)) {
        minSph = sphNum;
        break;
      }
      float d = pTest.distance(testS->s.c) - testS->s.r;
      if (d < minD) {
        minD = d;
        minSph = sphNum;
      }
    }

    //  record which sphere the point will be added to
    if (minSph < 0)
      removal->pointMap.index(i) = -1;
    else
      removal->pointMap.index(i) = minSph;
  }

  removal->cost = costOfRemoval(*removal, medialSpheres, sI, surPts);
}

void SRBurst::constructRemovals(Array<SphereRemove> *removals,
                                const Array<MedialSphere> &medialSpheres,
                                const Array<Surface::Point> &surPts) const {
  int numSpheres = medialSpheres.getSize();
  removals->resize(numSpheres);
  for (int i = 0; i < numSpheres; i++)
    constructRemoval(&removals->index(i), medialSpheres, i, surPts);
}

void SRBurst::applyRemoval(Array<SphereRemove> *removals,
                           Array<MedialSphere> *medialSpheres, int sI,
                           const Array<Surface::Point> &surPts,
                           bool updateHouseKeeping) const {
  SphereRemove *removal = &removals->index(sI);
  MedialSphere *medSph = &medialSpheres->index(sI);
  int numSph = medialSpheres->getSize();
  int numPts = medSph->pts.getSize();
  int numNeigh = medSph->neighbours.getSize();

  //  invalidate the sphere
  medSph->valid = false;

  //  tracking spheres whose pointlist has changed
  Array<bool> update;
  update.resize(numSph);
  update.clear();

  //  assign it's points to the assigned spheres
  for (int i = 0; i < numPts; i++) {
    int pNum = medSph->pts.index(i);
    int dNum = removal->pointMap.index(i);
    if (dNum >= 0) {
      MedialSphere *sDest = &medialSpheres->index(dNum);
      CHECK_DEBUG(sDest->valid,
                  "applyRemoval : destination sphere is invalid!!!");

#ifdef ONE_SPHERE_PER_POINT
      // CHECK_DEBUG0(!sDest->pts.inList(pNum));
#else
      if (!sDest->pts.inList(pNum))
#endif
      {
        update.index(dNum) = true;
        sDest->pts.addItem() = pNum;
      }
    }
  }

  //  update bounding spheres
  for (int i = 0; i < numSph; i++) {
    if (update.index(i)) {
      MedialSphere *uS = &medialSpheres->index(i);

      Point3D pOld = uS->s.c;
      uS->error =
          refitSphere(&uS->s, surPts, uS->pts, sphereEval, sphereFitter, &pOld);
    }
  }

  if (updateHouseKeeping) {
    //  remove myself from my neighbours
    for (int i = 0; i < numNeigh; i++) {
      int neighNum = medSph->neighbours.index(i);
      Array<int> *neigh = &medialSpheres->index(neighNum).neighbours;
      int num = neigh->getSize();
      int j;
      for (j = 0; j < num; j++)
        if (neigh->index(j) == sI)
          break;

      CHECK_DEBUG(
          j != num,
          "applyRemoval : I was not a neighbour of one of it's neighbours");
      neigh->index(j) = neigh->index(num - 1);
      neigh->resize(num - 1);

      // CHECK_DEBUG(!neigh->inList(sI), "Im still in my neighbour's neighbour
      // list");
    }

    //  connect my neighbours as neighbours
    for (int i = 0; i < numNeigh; i++) {
      int s1Num = medSph->neighbours.index(i);
      MedialSphere *s1 = &medialSpheres->index(s1Num);
      CHECK_DEBUG(s1->valid, "applyRemoval : i have an invalid neighbour");

      for (int j = i + 1; j < numNeigh; j++) {
        int s2Num = medSph->neighbours.index(j);
        MedialSphere *s2 = &medialSpheres->index(s2Num);
        CHECK_DEBUG(s2->valid, "applyRemoval : i have an invalid neighbour");
        CHECK_DEBUG0(s2Num != s1Num);

        if (s1->s.overlap(s2->s)) { //  do we need overlap
          if (!s1->neighbours.inList(s2Num)) {
            s1->neighbours.addItem() = s2Num;
            update.index(s1Num) = true;

            // CHECK_DEBUG0(!s2->neighbours.inList(s1Num));
            s2->neighbours.addItem() = s1Num;
            update.index(s2Num) = true;
          }
        }
      }
    }

    //  removals for neighbours of neighbours will also have to be updated
    for (int i = 0; i < numNeigh; i++) {
      int nI = medSph->neighbours.index(i);

      if (update.index(nI)) {
        MedialSphere *mN = &medialSpheres->index(nI);
        int num = mN->neighbours.getSize();
        for (int j = 0; j < num; j++)
          update.index(mN->neighbours.index(j)) = true;
      }

      //  update the neighbour as I am no longer valid
      update.index(nI) = true;
    }

    //  make sure all the nodes have neighbours before we make removal
    checkNoNeighbours(medialSpheres);

    //  do updates
    for (int i = 0; i < numSph; i++) {
      if (update.index(i)) {
        CHECK_DEBUG(medialSpheres->index(i).valid,
                    "applyRemoval : trying to update an invalid sphere");
        SphereRemove *sr = &removals->index(i);
        constructRemoval(sr, *medialSpheres, i, surPts);
      }
    }
  }

  //  now i can free up my memory
  if (medSph->neighbours.getSize())
    medSph->neighbours.free();
  if (medSph->pts.getSize()) {
    medSph->pts.free();
    removal->pointMap.free();
  }
}

void SRBurst::makeAllNeighbours(Array<SphereRemove> *removals,
                                Array<MedialSphere> *medialSpheres,
                                const Array<Surface::Point> &surPts) const {
  //  clear all neighbours
  int numSph = medialSpheres->getSize();
  for (int i = 0; i < numSph; i++)
    medialSpheres->index(i).neighbours.setSize(0);

  //  setup new neighbours
  for (int i = 0; i < numSph; i++) {
    MedialSphere *s1 = &medialSpheres->index(i);
    if (!s1->valid)
      continue;

    //  check against other spheres
    for (int j = i + 1; j < numSph; j++) {
      MedialSphere *s2 = &medialSpheres->index(j);
      if (!s2->valid)
        continue;

      s1->neighbours.addItem() = j;
      s2->neighbours.addItem() = i;
    }
  }

  for (int i = 0; i < numSph; i++) {
    if (medialSpheres->index(i).valid)
      constructRemoval(&removals->index(i), *medialSpheres, i, surPts);
  }
}

void SRBurst::saveImagePre(const Array<MedialSphere> &medialSpheres,
                           const Array<int> &ptList, int remNum,
                           int maxNum) const {
  float selCol1[] = {1, 1, 0};
  float selCol2[] = {0, 1, 0};

  char buffer[1024];
  Array<int> sel, selRem;
  Array<Sphere> spheres;

  //  base set of spheres
  int numValid = 0;
  int numMed = medialSpheres.getSize();
  for (int i = 0; i < numMed; i++) {
    const MedialSphere *ms = &medialSpheres.index(i);
    if (ms->valid && i != remNum) {
      numValid++;
      int newSph = spheres.addIndex();
      spheres.index(newSph) = ms->s;

      if (ptList.inList(i))
        selRem.addItem() = newSph;
    }
  }
  int oldNum = spheres.getSize();

  if (numValid < maxNum) {
    //  make the set with the sphere to be removed highlighted
    int newSph = spheres.addIndex();
    spheres.index(newSph) = medialSpheres.index(remNum).s;
    sel.addItem() = newSph;
    sprintf(buffer, "c:/devel/burstPics/burstPic-%.4d-A.pov",
            maxNum - numValid);
    exportSpheresPOV(buffer, spheres, 1.0f / 1000, true, NULL, &sel, selCol1);

    //  reset list
    sel.resize(0);
    spheres.resize(oldNum);

    //  make the set with the sphere to be removed highlighted
    sprintf(buffer, "c:/devel/burstPics/burstPic-%.4d-B.pov",
            maxNum - numValid);
    exportSpheresPOV(buffer, spheres, 1.0f / 1000, true, NULL, &selRem,
                     selCol2);
  }
}

void SRBurst::saveImagePost(const Array<MedialSphere> &medialSpheres,
                            const Array<int> &ptList, int remNum,
                            int maxNum) const {
  float selCol2[] = {0, 1, 0};

  Array<int> sel;
  Array<Sphere> spheres;

  //  base set of spheres
  int numValid = 0;
  int numMed = medialSpheres.getSize();
  for (int i = 0; i < numMed; i++) {
    const MedialSphere *ms = &medialSpheres.index(i);
    if (ms->valid && i != remNum) {
      numValid++;
      int newSph = spheres.addIndex();
      spheres.index(newSph) = ms->s;

      if (ptList.inList(i))
        sel.addItem() = newSph;
    }
  }
  int oldNum = spheres.getSize();
  numValid++; //  so numbering is the same as before it was removed

  if (numValid < maxNum) {
    //  make the set with the sphere to be removed highlighted
    char buffer[1024];
    sprintf(buffer, "c:/devel/burstPics/burstPic-%.4d-C.pov",
            maxNum - numValid + 1);
    exportSpheresPOV(buffer, spheres, 1.0f / 1000, true, NULL, &sel, selCol2);
  }
}
