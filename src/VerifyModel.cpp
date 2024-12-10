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

#include "VerifyModel.h"

/*
    model verification routine
*/
bool verifyModel(const Surface &sur, const MedialTester *mt) {
  //  test model properties
  int badVerts, badNeighs, openEdges, multEdges, badFaces;
  sur.testSurface(&badVerts, &badNeighs, &openEdges, &multEdges, &badFaces);

  //  check if surface is ok
  bool ok = badVerts == 0 && badNeighs == 0 && openEdges == 0 &&
            multEdges == 0 && badFaces == 0;

  //  test in/out routine
  int numPts = 1000, numProbs = 0;
  if (ok && mt) {
    printf("Testing In/Out...\n");

    float lX = sur.pMax.x - sur.pMin.x;
    float lY = sur.pMax.y - sur.pMin.y;
    float lZ = sur.pMax.z - sur.pMin.z;
    for (int i = 0; i < numPts; i++) {
      float r1 = 1.2 * rand() / (float)RAND_MAX - 0.1;
      float r2 = 1.2 * rand() / (float)RAND_MAX - 0.1;
      float r3 = 1.2 * rand() / (float)RAND_MAX - 0.1;

      Point3D pTest;
      pTest.x = sur.pMin.x + r1 * lX;
      pTest.y = sur.pMin.y + r2 * lY;
      pTest.z = sur.pMin.z + r3 * lZ;

      // bool inClose = insideSurfaceClosest(pTest, sur, mt.getFaceHash());
      bool inCross = insideSurfaceCrossingIter(pTest, sur, mt->getFaceHash());
      bool inGem = insideSurfaceGem(pTest, sur);

      if (/*inClose != inCross ||*/ inGem != inCross)
        numProbs++;
    }
  }

  //  write out results
  printf("Verification Results : \n");
  printf("\tBad Vertices\t:\t%d\n", badVerts);
  printf("\tBad Neigh\t:\t%d\n", badNeighs);
  printf("\tOpen Edges\t:\t%d\n", openEdges);
  printf("\tMulti Edges\t:\t%d\n", multEdges);
  printf("\tBad Faces\t:\t%d\n", badFaces);
  if (ok && mt)
    printf("\tBad in/out\t:\t%f%%\n", 100.0 * numProbs / (float)numPts);
  printf("\n");

  return ok;
}
