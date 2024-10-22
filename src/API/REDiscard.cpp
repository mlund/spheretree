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

#include "REDiscard.h"

bool REDiscard::isSphereRemovable(const Array<int> &pointCounts,
                                  int sphereNum) const {
  Array<int> list;
  surRep->listContainedPoints(&list, NULL, srcSpheres->index(sphereNum));
  int numList = list.getSize();

  for (int i = 0; i < numList; i++) {
    int pI = list.index(i);
    if (pointCounts.index(pI) < 2)
      return false;
  }

  return true;
}

//  implementation of RE algorithm
bool REDiscard::reduceSpheres(Array<int> *inds, int maxNum,
                              Array<int> *destCounts, double maxMet,
                              Array<double> *mets) const {
  //  get points
  const Array<Surface::Point> *surPts = surRep->getSurPts();
  int numPts = surPts->getSize();
  int numSph = srcSpheres->getSize();

  //  list of counts for number of spheres over each point
  Array<int> pointCounts(numPts), sphereCount(numSph);
  pointCounts.clear();

  //  fill list
  Array<bool> coveredPts(numPts);
  coveredPts.clear();
  for (int i = 0; i < numSph; i++) {
    Array<int> list;
    surRep->listContainedPoints(&list, &coveredPts, srcSpheres->index(i));

    int numList = list.getSize();
    for (int j = 0; j < numList; j++) {
      int pI = list.index(j);
      pointCounts.index(pI)++;
    }

    sphereCount.index(i) = numList;
  }

  //  no point continuing if the points aren't all covered to start
  for (int i = 0; i < numPts; i++)
    if (!coveredPts.index(i))
      return false;

  //  make list of spheres that are removable
  Array<bool> removable(numSph);
  for (int i = 0; i < numSph; i++)
    removable.index(i) = isSphereRemovable(pointCounts, i);

  //  flags for which spheres have been dumped
  Array<bool> removed(numSph);
  removed.clear();

  //  do reduction
  while (true) {
    //  find "smallest" removable sphere
    int minI = -1;
    int minCount = INT_MAX;
    for (int i = 0; i < numSph; i++) {
      if (removable.index(i)) {
        int count = sphereCount.index(i);
        if (count < minCount) {
          minCount = count;
          minI = i;
        }
      }
    }

    //  did we find a removable sphere
    if (minI < 0)
      break;

    OUTPUTINFO("Discarding Sphere %d\n", minI);

    //  remove sphere
    removed.index(minI) = true;
    removable.index(minI) = false;

    //  decrement counts for number of spheres covering the points
    Array<int> list;
    surRep->listContainedPoints(&list, NULL, srcSpheres->index(minI));
    int numList = list.getSize();
    for (int i = 0; i < numList; i++) {
      int pI = list.index(i);
      pointCounts.index(pI)--;
    }

    //  check the spheres for removability
    for (int i = 0; i < numSph; i++) {
      bool *flag = &removable.index(i);
      if ((*flag) == true) {
        (*flag) = isSphereRemovable(pointCounts, i);
      }
    }
  }

  //  generate indices
  inds->setSize(0);
  for (int i = 0; i < numSph; i++) {
    if (!removed.index(i))
      inds->addItem() = i;
  }

  OUTPUTINFO("%d Spheres left\n", inds->getSize());

  return (maxNum < 0) || (inds->getSize() <= maxNum);
}
