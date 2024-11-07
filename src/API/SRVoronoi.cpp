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

#include "SRVoronoi.h"

SRVoronoi::SRVoronoi() {
  vorAdapt = NULL;
  vor = NULL;
  srcSph = NULL;
  func = NULL;
  mt = NULL;
  initSpheres = 0;
  minSpheresPerNode = 0;
  errorDecreaseFactor = 2;
  maxItersForVoronoi = 100;
}

//  setup
void SRVoronoi::setup(Voronoi3D *vor, const MedialTester *mt) {
  this->vor = vor;
  this->mt = mt;
}

void SRVoronoi::setup(const Array<Sphere> &srcSph) { this->srcSph = &srcSph; }

void SRVoronoi::setupForLevel(int level, int degree,
                              const SurfaceRep *surRep) const {
  if (vor && initSpheres > 0 && level == 0) {
    //  FOR : update diagram using filter sphere
    VFAdaptive adaptive;
    VFAdaptive *vorAdaptPtr = vorAdapt;
    if (!vorAdapt) {
      vorAdaptPtr = &adaptive;
      adaptive.mt = mt;
    }

    OUTPUTINFO("Updating VORONOI...\n");
    vorAdaptPtr->adaptiveSample(vor, 0, -1, initSpheres, 0, surRep, NULL, true);
  }
}

//  internal
void SRVoronoi::constructSphereSet(Array<MedialSphere> *medialSpheres,
                                   const SurfaceRep &surRep,
                                   const Sphere *filterSphere,
                                   float parSphereErr,
                                   bool onlyOneSpherePerPoint) const {
  CHECK_DEBUG(vor != NULL || srcSph != NULL,
              "need voronoi diagram or source spheres");
  const Array<Surface::Point> *surPts =
      surRep.getSurPts(); // will be null if using forming points

  //  FOR : update diagram using filter sphere
  if (vor && parSphereErr > 0 && filterSphere && surPts) {
    VFAdaptive adaptive;
    VFAdaptive *vorAdaptPtr = vorAdapt;
    if (!vorAdapt) {
      vorAdaptPtr = &adaptive;
      adaptive.mt = mt;
    }

    float destErr = parSphereErr / errorDecreaseFactor;
    OUTPUTINFO("Updating VORONOI... (DestErr = %f)\n", destErr);
    vorAdaptPtr->adaptiveSample(vor, destErr, -1, -1, minSpheresPerNode,
                                &surRep, filterSphere, true,
                                maxItersForVoronoi);
  }

  //  point containment flags
  Array<bool> ptCovered;
  if (surPts) {
    ptCovered.resize(surPts->getSize());
    ptCovered.clear();
  }

  if (vor) {
    OUTPUTINFO("Making Neighbours from VORONOI\n");

    CHECK_DEBUG(mt != NULL, "need medial tester");
    int numVert = vor->vertices.getSize();

    //  map of voronoi vertices to medial spheres
    Array<int> vertexToSphereMap;
    vertexToSphereMap.resize(numVert);
    for (int i = 0; i < numVert; i++)
      vertexToSphereMap.index(i) = -1;

    //  construct the spheres corresponding to internal spheres
    int numSpheres = 0;
    medialSpheres->setSize(0);
    for (int i = 0; i < numVert; i++) {
      Voronoi3D::Vertex *vert = &vor->vertices.index(i);
      if (filterSphere && !filterSphere->overlap(vert->s))
        continue;

      if (mt->isMedial(vert)) {
        //  get the medial sphere
        vertexToSphereMap.index(i) = numSpheres++;
        MedialSphere *medSph = &medialSpheres->addItem();
        medSph->neighbours.setSize(0); //  just in case
        medSph->valid = true;
        medSph->s = vert->s;
        medSph->vertexNum = i;

        //  covered points
        if (surPts) {
          Array<int> *sphPts = &medSph->pts;
          sphPts->setSize(0);
          surRep.listContainedPoints(sphPts, &ptCovered, medSph->s,
                                     onlyOneSpherePerPoint ? &ptCovered : NULL);

          if (sphPts->getSize() == 0) {
            //  no points covered - kill merge sphere
            vertexToSphereMap.index(i) = -1;
            numSpheres = medialSpheres->getSize() - 1;
            medialSpheres->setSize(numSpheres);
          }
        }
      }
    }

    //  setup neighbour information
    for (int i = 0; i < numVert; i++) {
      int mapsTo = vertexToSphereMap.index(i);
      if (mapsTo >= 0) {
        //  get data structures
        const Voronoi3D::Vertex *vert = &vor->vertices.index(i);

        //  add to neighbours list
        for (int j = 0; j < 4; j++) {
          int neighVert = vert->n[j];
          CHECK_DEBUG0(neighVert != i);
          int neighMapsTo = vertexToSphereMap.index(neighVert);
          if (neighMapsTo > mapsTo) {
            // CHECK_DEBUG0(!medialSpheres->index(mapsTo).neighbours.inList(neighMapsTo));
            // CHECK_DEBUG0(!medialSpheres->index(neighMapsTo).neighbours.inList(mapsTo));
            medialSpheres->index(mapsTo).neighbours.addItem() = neighMapsTo;
            medialSpheres->index(neighMapsTo).neighbours.addItem() = mapsTo;
          }
        }
      }
    }

    //  make sure to give neighbourless spheres some company
    checkNoNeighbours(medialSpheres);
  } else {
    //  TODO : add filterSphere stuff to this
    //  TODO : add filterInds stuff to this

    //  construct medial spheres from srcSph
    int numSph = srcSph->getSize();
    medialSpheres->resize(numSph);
    for (int i = 0; i < numSph; i++) {
      MedialSphere *ms = &medialSpheres->index(i);
      ms->neighbours.setSize(0);
      ms->s = srcSph->index(i);
      ms->valid = true;

      //  setup points
      ms->pts.setSize(0);
      if (surPts) {
        int numPts = surPts->getSize();
        for (int j = 0; j < numPts; j++)
          if (ms->s.contains(surPts->index(j).p)) {
            ms->pts.addItem() = j;
            ptCovered.index(j) = true;
          }
      }
    }

    //  setup neighbour info
    for (int i = 0; i < numSph; i++)
      for (int j = i + 1; j < numSph; j++)
        if (medialSpheres->index(i).s.overlap(medialSpheres->index(j).s)) {
          medialSpheres->index(i).neighbours.addItem() = j;
          medialSpheres->index(j).neighbours.addItem() = i;
        }

    checkNoNeighbours(medialSpheres);
  }

  //  assign uncovered points to closest sphere
  int numSph = medialSpheres->getSize();
  if (numSph && surPts) {
    int numPts = surPts->getSize();
    for (int i = 0; i < numPts; i++) {
      if (!ptCovered.index(i)) {
        int minJ = -1;
        float minD = REAL_MAX;
        Point3D p = surPts->index(i).p;
        for (int j = 0; j < numSph; j++) {
          float d = medialSpheres->index(j).s.c.distance(p) -
                    medialSpheres->index(j).s.r;
          if (d < minD) {
            minD = d;
            minJ = j;
          }
        }

        medialSpheres->index(minJ).pts.addItem() = i;
      }
    }
  }
}

void SRVoronoi::initErrors(Array<MedialSphere> *spheres,
                           const SEBase *eval) const {
  int numSph = spheres->getSize();
  for (int i = 0; i < numSph; i++) {
    MedialSphere *ms = &spheres->index(i);
    ms->error = eval->evalSphere(ms->s);
  }
}

void SRVoronoi::checkNoNeighbours(Array<MedialSphere> *medialSpheres) const {
  int numSph = medialSpheres->getSize();

  //  check for no neighbours
  Array<int> noNeigh, stillNoNeigh;
  for (int i = 0; i < numSph; i++) {
    if (medialSpheres->index(i).valid &&
        medialSpheres->index(i).neighbours.getSize() == 0)
      noNeigh.addItem() = i;
  }

  //  give ones without neighbours some neighbours
  int numNoNeigh = noNeigh.getSize();
  for (int i = 0; i < numNoNeigh; i++) {
    int mI = noNeigh.index(i);
    MedialSphere *ms = &medialSpheres->index(mI);

    //  construct neighbours
    for (int j = 0; j < numSph; j++) {
      MedialSphere *msOther = &medialSpheres->index(j);
      if (j == mI || !msOther->valid)
        continue;

      if (ms->s.overlap(msOther->s)) {
        if (!ms->neighbours.inList(j)) {
          ms->neighbours.addItem() = j;
          // CHECK_DEBUG0(!msOther->neighbours.inList(mI));
          msOther->neighbours.addItem() = mI;
        }
      }
    }

    if (ms->neighbours.getSize() == 0)
      stillNoNeigh.addItem() = mI;
  }

  //  last resort - drop overlap test
  numNoNeigh = stillNoNeigh.getSize();
  for (int i = 0; i < numNoNeigh; i++) {
    int mI = stillNoNeigh.index(i);
    MedialSphere *ms = &medialSpheres->index(mI);

    //  construct neighbours
    for (int j = 0; j < numSph; j++) {
      MedialSphere *msOther = &medialSpheres->index(j);
      if (j == mI || !msOther->valid)
        continue;

      if (!ms->neighbours.inList(j)) {
        ms->neighbours.addItem() = j;
        // CHECK_DEBUG0(!msOther->neighbours.inList(mI));
        msOther->neighbours.addItem() = mI;
      }
    }

    if (ms->neighbours.getSize() == 0)
      OUTPUTINFO("Big Problem - no neighbours !!!\n");
  }
}

void SRVoronoi::listNoNeighbours(Array<MedialSphere> *medialSpheres) const {
  int numSph = medialSpheres->getSize();

  //  check for no neighbours
  Array<int> noNeigh, stillNoNeigh;
  for (int i = 0; i < numSph; i++) {
    if (medialSpheres->index(i).valid &&
        medialSpheres->index(i).neighbours.getSize() == 0)
      OUTPUTINFO("PROBLEM : Sphere %d has no neighbours\n");
  }
}
