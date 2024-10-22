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

#include "RESelect.h"

bool RESelect::setup(const Array<Sphere> &src, const SurfaceRep &surRep) {
  REBase::setup(src, surRep);

  const Array<Surface::Point> *surPts = surRep.getSurPts();
  int numPts = surPts->getSize();
  int numSph = src.getSize();

  //  flags for which points are covered
  Array<bool> coveredFlags;
  coveredFlags.resize(numPts);
  coveredFlags.clear();

  //  setup lists
  spheresPerPoint.resize(numPts);
  pointsPerSphere.resize(numSph);

  //  make sure lists are empty
  for (int i = 0; i < numPts; i++)
    spheresPerPoint.index(i).setSize(0);

  //  fill in lists
  for (int i = 0; i < numSph; i++) {
    Sphere s = srcSpheres->index(i);
    Array<int> *ptsInSphere = &pointsPerSphere.index(i);
    ptsInSphere->setSize(0);

    //  get points in the sphere
    surRep.listContainedPoints(ptsInSphere, &coveredFlags, s);

    //  add the sphere to the point's list
    int numPtsInSphere = ptsInSphere->getSize();
    for (int j = 0; j < numPtsInSphere; j++) {
      int pI = ptsInSphere->index(j);
      spheresPerPoint.index(pI).addItem() = i;
    }
  }

  for (int i = 0; i < numPts; i++)
    if (!coveredFlags.index(i))
      return false;
  return true;
}

void RESelect::tidyUp() {
  if (spheresPerPoint.getSize())
    spheresPerPoint.free();
  if (pointsPerSphere.getSize())
    pointsPerSphere.free();

  REBase::tidyUp();
}

bool RESelect::reduceSpheres(Array<int> *inds, int maxAllow,
                             Array<int> *destCounts, double maxMet,
                             Array<double> *mets) const {
  CHECK_DEBUG0(srcSpheres != NULL);
  CHECK_DEBUG0(surRep != NULL);

  const Array<Surface::Point> *surPts = this->surRep->getSurPts();
  int numPts = surPts->getSize();
  int numSph = srcSpheres->getSize();

  //  counts of points covered by each sphere (count <= 0 means sphere is now
  //  invalid)
  Array<int> counts;
  counts.resize(numSph);
  for (int i = 0; i < numSph; i++)
    counts.index(i) = pointsPerSphere.index(i).getSize();

  //  flags for points covered by selected set
  Array<bool> coveredPts;
  coveredPts.resize(numPts);
  coveredPts.clear();

  //  do selection
  int numCov = 0;
  inds->setSize(0);
  if (mets)
    mets->setSize(0);
  while (numCov < numPts) {
    if (maxAllow > 0 && inds->getSize() >= maxAllow)
      return false; //  still surface covered and no more spheres allowed

    //  get the next sphere to add to the set
    double selMet = DBL_MAX;
    int maxI =
        selectSphere(counts, coveredPts, pointsPerSphere, &selMet, maxMet);
    if (maxI < 0)
      return false; //  no more spheres to choose from

    //  store count
    if (destCounts)
      destCounts->addItem() = counts.index(maxI);

    //  add sphere to selected set
    inds->addItem() = maxI;
    counts.index(maxI) = -1; //  make invalid
    if (mets)
      mets->addItem() = selMet;

    //  flag points as contained and update counts for points
    const Array<int> *bestPts = &pointsPerSphere.index(maxI);
    int numBest = bestPts->getSize();
    for (int i = 0; i < numBest; i++) {
      int ptNum = bestPts->index(i);
      if (!coveredPts.index(ptNum)) {
        coveredPts.index(ptNum) = true;
        numCov++;

        //  point wasn't covered - so now the spheres containing this point
        //  contain one less point
        const Array<int> *changed = &spheresPerPoint.index(ptNum);
        int numChanged = changed->getSize();
        for (int j = 0; j < numChanged; j++) {
          int chSph = changed->index(j);
          counts.index(chSph)--;
        }
      }
    }
  }

  return true;
}
