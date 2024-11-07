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

#include "SSRings.h"
#define PI 3.1415926535

//  constructor
SSRings::SSRings(int numRings) { setRings(numRings); }

//  setup
void SSRings::setRings(int numRings) { this->numRings = numRings; }

//  generate the samples
void SSRings::generateSamples(Array<Point3D> *pts) const {
  generateSamples(pts, numRings);
}

void SSRings::generateSamples(Array<Point3D> *pts, int numRings) {
  //  work out number of stacks and slices
  int hemiStacks = numRings /= 2;
  int slices = 2 * hemiStacks + 1;

  //  go down through the layers
  pts->setSize(0);
  for (int stack = -hemiStacks; stack <= hemiStacks; stack++) {
    //  calculate the radius as fraction of unit radius
    float phi = (stack * PI) / (2.0f * hemiStacks) + PI / 2.0f;
    float radius = sin(phi);

    //  work out how many points to generate
    int numDots = (int)(slices * radius);
    float div = 360.0 / numDots;

    //  generate dots
    float sP = sin(phi);
    float cP = cos(phi);

    for (int dot = 0; dot < numDots; dot++) {
      float theta = 2.0 * PI * dot / (float)numDots;
      float sT = sin(theta);
      float cT = cos(theta);

      Point3D *p = &pts->addItem();
      p->x = sT * sP;
      p->y = cT * sP;
      p->z = cP;
    }
  }
}
