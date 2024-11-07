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

#include "SSIsohedron.h"
#include "../Exceptions/CheckDebug.h"

//  constructor
SSIsohedron::SSIsohedron(int level) { setLevel(level); }

//  setup
void SSIsohedron::setLevel(int level) {
  CHECK_DEBUG0(level >= 0);
  this->levels = level;
}

//  sample
void SSIsohedron::generateSamples(Array<Point3D> *pts) const {
  generateSamples(pts, levels);
}

void SSIsohedron::generateSamples(Array<Point3D> *pts, int levels) {
  //  setup the initial vertices
  pts->resize(12);
  for (int i = 0; i < 12; i++) {
    Point3D *p = &pts->index(i);
    p->x = vdata[i][0];
    p->y = vdata[i][1];
    p->z = vdata[i][2];
  }

  //  construct by recursive sub division
  for (int i = 0; i < 20; i++)
    subDivideIso(pts, tindices[i][0], tindices[i][1], tindices[i][2],
                 levels - 1);
}

//  data for initial isohedron
#define X .525731112119133606f
#define Z .850650808352039932f
float SSIsohedron::vdata[12][3] = {
    {-X, 0.0f, Z}, {X, 0.0f, Z},  {-X, 0.0f, -Z}, {X, 0.0f, -Z},
    {0.0f, Z, X},  {0.0f, Z, -X}, {0.0f, -Z, X},  {0.0f, -Z, -X},
    {Z, X, 0.0f},  {-Z, X, 0.0f}, {Z, -X, 0.0f},  {-Z, -X, 0.0f}};

int SSIsohedron::tindices[20][3] = {
    {1, 4, 0},  {4, 9, 0},  {4, 5, 9},  {8, 5, 4},  {1, 8, 4},
    {1, 10, 8}, {10, 3, 8}, {8, 3, 5},  {3, 2, 5},  {3, 7, 2},
    {3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11}, {6, 1, 0},
    {10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9},  {11, 2, 7}};

void SSIsohedron::subDivideIso(Array<Point3D> *pts, int v0, int v1, int v2,
                               int depth) {
  //  check recursion
  if (depth < 0)
    return;

  int v[3];
  v[0] = v0;
  v[1] = v1;
  v[2] = v2;

  //  make new vertices
  int newV[3];
  for (int i = 0; i < 3; i++) {
    //  get new vertex positions
    Point3D newP;
    newP.x = pts->index(v[i]).x + pts->index(v[(i + 1) % 3]).x;
    newP.y = pts->index(v[i]).y + pts->index(v[(i + 1) % 3]).y;
    newP.z = pts->index(v[i]).z + pts->index(v[(i + 1) % 3]).z;
    float mag = sqrt(newP.x * newP.x + newP.y * newP.y + newP.z * newP.z);
    newP.x /= mag;
    newP.y /= mag;
    newP.z /= mag;

    //  scan for close point
    int numVert = pts->getSize();
    int j;
    for (j = 0; j < numVert; j++) {
      float d = pts->index(j).distance(newP);
      if (d < EPSILON_LARGE)
        break; //  won't work for very fine tessellations
    }

    if (j < numVert) {
      newV[i] = j;
    } else {
      newV[i] = pts->addIndex();
      pts->index(newV[i]) = newP;
    }
  }

  //  now sub divide triangle
  subDivideIso(pts, v[0], newV[0], newV[2], depth - 1);
  subDivideIso(pts, v[1], newV[1], newV[0], depth - 1);
  subDivideIso(pts, v[2], newV[2], newV[1], depth - 1);
  subDivideIso(pts, newV[0], newV[1], newV[2], depth - 1);
}
