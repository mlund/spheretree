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

#include "RELargestLM.h"

bool RELargestLM::reduceSpheres(Array<int> *inds, int maxAllow,
                                Array<int> *destCounts, double maxMet,
                                Array<double> *mets) const {
  //  get surface points
  const Array<Surface::Point> *surPts = surRep->getSurPts();
  int numPts = surPts->getSize();
  int numSph = srcSpheres->getSize();

  //  flags for which points have been covered
  Array<bool> coveredPts(numPts);
  coveredPts.clear();

  //  generate counts and mark covered points
  Array<int> counts(numSph);
  for (int i = 0; i < numSph; i++) {
    int count = surRep->flagContainedPoints(&coveredPts, srcSpheres->index(i));
    counts.index(i) = count;
  }

  //  check that all points are covered
  for (int i = 0; i < numPts; i++)
    if (!coveredPts.index(i))
      return false; //  fail straight off

  //  clear the covered flags again
  //  not they are to be the points covered by the selected set of spheres
  coveredPts.clear();

  //  do selection
  int numCov = 0;
  inds->setSize(0);
  if (mets)
    mets->setSize(0);

  while (numCov < numPts) {
    if (maxAllow > 0 && inds->getSize() >= maxAllow)
      return false; //  still surface covered and no more spheres allowed

    //  find the sphere with the largest count
    int maxCount = 0, maxI = -1;
    for (int i = 0; i < numSph; i++) {
      int count = counts.index(i);
      if (count > maxCount) {
        maxCount = count;
        maxI = i;
      }
    }
    if (maxI < 0)
      return false; //  no spheres to choose from

    //  store count
    if (destCounts)
      destCounts->addItem() = maxCount;

    //  add sphere to selected set
    inds->addItem() = maxI;
    counts.index(maxI) = -1; //  make invalid
    if (mets)
      mets->addItem() = maxCount;

    //  list the points in the sphere
    Array<int> list;
    surRep->listContainedPoints(&list, &coveredPts, srcSpheres->index(maxI),
                                &coveredPts);
    int numList = list.getSize();
    numCov += numList;

    //  test each point against the other spheres
    //  to decrement their counts
    for (int i = 0; i < numList; i++) {
      int pI = list.index(i);
      Point3D p = surPts->index(pI).p;

      for (int j = 0; j < numSph; j++) {
        int *count = &counts.index(j);
        if (*count > 0 && srcSpheres->index(j).contains(p))
          (*count)--;
      }
    }
  }

  return true;
}
