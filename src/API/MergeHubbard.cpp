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

#include "MergeHubbard.h"
#include "SFRitter.h"
#include "SEPoint.h"

//  constructor
MergeHubbard::MergeHubbard() {
  vor = NULL;
  mt = NULL;
}

//  setup
void MergeHubbard::setup(Voronoi3D *vor, const MedialTester *mt) {
  this->vor = vor;
  this->mt = mt;

  //  list of forming points (excluding the ones from the initial diagram)
  int numForm = vor->formingPoints.getSize();
  formingPoints.resize(numForm - 8);
  for (int i = 8; i < numForm; i++) {
    Surface::Point *pt = &formingPoints.index(i - 8);

    pt->p = vor->formingPoints.index(i).p;
    pt->n = vor->formingPoints.index(i).v;
  }
}

void MergeHubbard::getBoundingSphere(Sphere *s) const {
  SFRitter::makeSphere(s, formingPoints);
}

//  reduce sphere set
void MergeHubbard::getSpheres(Array<Sphere> *spheres, int numDest,
                              const Array<int> *filterInds) const {
  Array<MedialSphere> medialSpheres;
  constructSphereSet(&medialSpheres, filterInds);

  iterativeMerge(&medialSpheres, numDest);

  spheres->setSize(0);
  int numSph = medialSpheres.getSize();
  for (int i = 0; i < numSph; i++)
    if (medialSpheres.index(i).valid)
      spheres->addItem() = medialSpheres.index(i).s;
}

//  INTERNAL
void MergeHubbard::constructSphereSet(Array<MedialSphere> *medialSpheres,
                                      const Array<int> *filterInds) const {
  CHECK_DEBUG(vor != NULL && mt != NULL,
              "need voronoi diagram or source spheres");
  int numVert = vor->vertices.getSize();

  //  map of voronoi vertices to medial spheres
  Array<int> vertexToSphereMap;
  vertexToSphereMap.resize(numVert);
  for (int i = 0; i < numVert; i++)
    vertexToSphereMap.index(i) = -1;

  //  construct the spheres corresponding to internal spheres
  int numSpheres = 0;
  medialSpheres->setSize(0);

  int numToDo = numVert;
  if (filterInds)
    numToDo = filterInds->getSize();

  for (int i = 0; i < numToDo; i++) {
    int vI = i;
    if (filterInds)
      vI = filterInds->index(i);

    Voronoi3D::Vertex *vert = &vor->vertices.index(vI);
    if (mt->isMedial(vert)) {
      //  get the medial sphere
      vertexToSphereMap.index(vI) =
          numSpheres++; /* this has i instead of vI - totally wrong i think */
      MedialSphere *medSph = &medialSpheres->addItem();
      medSph->neighbours.setSize(0); //  just in case
      medSph->valid = true;
      medSph->vertices.resize(1);
      medSph->vertices.index(0) = vI;
      medSph->s = vert->s;

      //  covered points
      //  first 8 are bounding points so we need to adjust for this
      Array<int> *sphPts = &medSph->pts;
      sphPts->resize(0);
      for (int j = 0; j < 4; j++) {
        int fI = vert->f[j] - 8;
        if (fI >= 0)
          sphPts->addItem() = fI;
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
        int neighMapsTo = vertexToSphereMap.index(neighVert);
        if (neighMapsTo > mapsTo) {
          medialSpheres->index(mapsTo).neighbours.addItem() = neighMapsTo;
          medialSpheres->index(neighMapsTo).neighbours.addItem() = mapsTo;
        }
      }
    }
  }

  //  make sure to give neighbourless spheres some company
  numSpheres = medialSpheres->getSize();
  for (int i = 0; i < numSpheres; i++) {
    MedialSphere *medSph = &medialSpheres->index(i);
    if (medSph->neighbours.getSize() == 0) {
      //  add any other sphere which we overlap
      for (int j = i + 1; j < numSpheres; j++) {
        if (medSph->s.overlap(medialSpheres->index(j).s)) {
          medialSpheres->index(j).neighbours.addItem() = i;
          medialSpheres->index(i).neighbours.addItem() = j;
        }
      }
    }
  }
}

void MergeHubbard::iterativeMerge(Array<MedialSphere> *medialSpheres,
                                  int numDest) const {
  //  construct mergers
  Array<Merger> mergers;
  constructMergers(&mergers, *medialSpheres);

  //  dump neighbour info
  int numSpheres = medialSpheres->getSize();
  for (int i = 0; i < numSpheres; i++) {
    MedialSphere *ms = &medialSpheres->index(i);
    if (ms->neighbours.getSize())
      ms->neighbours.free();
  }

  //  do reduction
  while (numSpheres > numDest) {
    int numMergers = mergers.getSize();

    //  no mergers - i guess we merge all
    if (numMergers == 0) {
      constructAllMergers(&mergers, *medialSpheres);
      numMergers = mergers.getSize();
    }

    //  find best merger
    int minI = -1;
    REAL minCost = REAL_MAX;
    for (int i = 0; i < numMergers; i++) {
      float cost = mergers.index(i).cost;
      if (cost < minCost) {
        minCost = cost;
        minI = i;
      }
    }

    //  do merger
    if (numSpheres % 25 == 0)
      OUTPUTINFO("NumSph = %d, MinI = %d, MinCost = %f\n", numSpheres, minI,
                 minCost);
    applyMerger(&mergers, medialSpheres, minI, (numSpheres - 1) > numDest);
    numSpheres--;
  }
}

void MergeHubbard::constructMerger(Merger *merger,
                                   const Array<MedialSphere> &medialSpheres,
                                   int i1, int i2) const {
  //  setup merger
  merger->i1 = i1;
  merger->i2 = i2;

  if (!medialSpheres.index(i1).valid || !medialSpheres.index(i2).valid)
    OUTPUTINFO("Constructing merger between invalid spheres\n");

  //  merge point lists
  Array<int> mergedPts;
  combineLists(&mergedPts, medialSpheres.index(i1).pts,
               medialSpheres.index(i2).pts);

  //  make the new sphere
  SFRitter::makeSphere(&merger->s, formingPoints, mergedPts);

  //  make list of points in the sphere
  int numPts = formingPoints.getSize();
  Array<Surface::Point> testPoints;
  for (int i = 0; i < numPts; i++) {
    const Surface::Point *p = &formingPoints.index(i);
    if (merger->s.contains(p->p))
      testPoints.addItem() = *p;
  }
  /*
    //  do evaluation
    SEPoint eval;
    eval.setSurfaceSamples(testPoints);
    merger->cost = eval.evalSphere(merger->s);
  */

  //  changed so it uses ALL points in the new sphere for eval
  SEPoint eval;
  merger->cost = eval.evalSphere(merger->s, formingPoints, mergedPts);
}

void MergeHubbard::constructMergers(
    Array<Merger> *mergers, const Array<MedialSphere> &medialSpheres) const {
  mergers->setSize(0);
  int numSpheres = medialSpheres.getSize();
  for (int i = 0; i < numSpheres; i++) {
    const MedialSphere *ms = &medialSpheres.index(i);
    int numNeigh = ms->neighbours.getSize();
    for (int j = 0; j < numNeigh; j++) {
      int neighNum = ms->neighbours.index(j);
      if (neighNum > i) {
        Merger *merger = &mergers->addItem();
        constructMerger(merger, medialSpheres, i, neighNum);
      }
    }
  }
}

void MergeHubbard::constructAllMergers(
    Array<Merger> *mergers, const Array<MedialSphere> &medialSpheres) const {
  //  make merger for each pair of spheres
  mergers->setSize(0);
  int numSpheres = medialSpheres.getSize();
  for (int i = 0; i < numSpheres; i++) {
    if (medialSpheres.index(i).valid) {
      for (int j = i + 1; j < numSpheres; j++) {
        if (medialSpheres.index(j).valid) {
          //  construct new merger
          Merger *m = &mergers->addItem();
          constructMerger(m, medialSpheres, i, j);
        }
      }
    }
  }
}

void MergeHubbard::applyMerger(Array<Merger> *mergers,
                               Array<MedialSphere> *medialSpheres, int mergI,
                               bool updateHouseKeeping) const {
  int numMergs = mergers->getSize();
  Merger *merger = &mergers->index(mergI);
  int i1 = merger->i1, i2 = merger->i2;
  if (i1 > i2) {
    int tmp = i1;
    i1 = i2;
    i2 = tmp;
  }
  MedialSphere *s1 = &medialSpheres->index(i1);
  MedialSphere *s2 = &medialSpheres->index(i2);

  if (!s1->valid || !s2->valid)
    OUTPUTINFO("One of these sphere is already invalid\n");

  //  update S1 with merged sphere
  s1->s = merger->s;

  //  merge lists of forming points and vertices (for Hubbard's Method)
  mergeLists(&s1->pts, s2->pts);
  mergeLists(&s1->vertices, s2->vertices);

  //  remove merger as sJ will map to sI, can't merge sphere to itself
  if (mergI != numMergs - 1)
    mergers->index(mergI) = mergers->index(numMergs - 1);
  mergers->setSize(--numMergs);

  //  mark s2 as dead
  s2->valid = false;
  s2->pts.setSize(0);

  if (updateHouseKeeping) {
    //  list of mergers already re-done
    Array<int> mergesWithI1;

    //  update merges
    int numNewMergers = 0;
    for (int i = 0; i < numMergs; i++) {
      Merger *m = &mergers->index(i);
      //  if merger involved i1 or i2, update
      if (m->i1 == i1 || m->i2 == i1 || m->i1 == i2 || m->i2 == i2) {
        //  remap i2 to i1
        if (m->i1 == i2)
          m->i1 = i1;
        else if (m->i2 == i2)
          m->i2 = i1;

        //  find index which isn't i1
        int otherI;
        if (m->i1 == i1)
          otherI = m->i2;
        else
          otherI = m->i1;

        if (mergesWithI1.inList(otherI)) {
          //  merge already done, delete merge
          if (i != numMergs - 1)
            mergers->index(i) = mergers->index(numMergs - 1);
          mergers->setSize(numMergs - 1);
          numMergs--;
          i--;
        } else {
          //  update merge
          numNewMergers++;
          constructMerger(m, *medialSpheres, i1, otherI);
          mergesWithI1.addItem() = otherI;
        }
      }
    }
  }
}

//  statics
void MergeHubbard::mergeLists(Array<int> *dest, const Array<int> &l) {
  int numL = l.getSize();
  int numDest = dest->getSize();
  for (int i = 0; i < numL; i++) {
    int n = l.index(i);

    //  check for duplicate assumes dest list contained no dups
    int j;
    for (j = 0; j < numDest; j++)
      if (dest->index(j) == n)
        break;

    //  unique entry
    if (j == numDest)
      dest->addItem() = n;
  }
}

void MergeHubbard::combineLists(Array<int> *dest, const Array<int> &l1,
                                const Array<int> &l2) {
  //  don't bother removing duplicates
  dest->clone(l1);
  dest->append(l2);
}
