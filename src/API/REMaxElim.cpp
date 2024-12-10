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

#include "REMaxElim.h"

int REMaxElim::selectSphere(const Array<int> &counts,
                            const Array<bool> &coveredPts,
                            const Array<Array<int> /**/> &pointsPerSphere,
                            double *selMet, double maxMet) const {
  const Array<Surface::Point> *surPts = surRep->getSurPts();

  int maxI = -1, maxCount = 0;
  int numSph = srcSpheres->getSize();
  int numPts = surPts->getSize();
  for (int i = 0; i < numSph; i++) {
    //  only do valid spheres
    if (counts.index(i) <= 0)
      continue;

    //  make the list of points contained within the new selected set
    Array<bool> tmpCover;
    tmpCover.clone(coveredPts);
    const Array<int> *list = &pointsPerSphere.index(i);
    int numList = list->getSize();
    for (int j = 0; j < numList; j++) {
      int num = list->index(j);
      tmpCover.index(num) = true;
    }

    //  count the spheres that will be removed
    int count = 0;
    for (int j = 0; j < numSph; j++) {
      //  only do valid spheres
      if (j == i || counts.index(j) <= 0)
        continue;

      //  early terminate
      if (count + (numSph - j) < maxCount)
        break; //  not enough sphere to break current max
      else if (count > maxMet)
        break;

      //  check if the sphere will be redundent
      const Array<int> *list = &pointsPerSphere.index(j);
      int numList = list->getSize();
      int k;
      for (k = 0; k < numList; k++) {
        int num = list->index(k);
        if (!tmpCover.index(num))
          break;
      }

      if (k == numList)
        count++;
    }

    //  update max
    if (count > maxCount && count < maxMet) {
      maxCount = count;
      maxI = i;
    }
  }

  if (selMet)
    *selMet = maxCount;

  return maxI;
}
