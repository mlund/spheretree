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

#include "STGOctree.h"
#include "SFWhite.h"
#include "../Geometry/CubeTri.h"

STGOctree::STGOctree() {
  sur = NULL;
  surTester = NULL;
}

STGOctree::STGOctree(const Surface &sur) { setSurface(sur); }

void STGOctree::setSurface(const Surface &sur) { this->sur = &sur; }

void STGOctree::constructTree(SphereTree *st) const {
  CHECK_DEBUG(st != NULL, "Need SphereTree");
  CHECK_DEBUG(sur != NULL, "Need Surface : use setSurface");

  constructTree(st, *sur, surTester);
}

void STGOctree::constructTree(SphereTree *st, const Surface &sur,
                              const SurfaceTester *surTester) {
  CHECK_DEBUG(st->degree == 8, "OCTREE i.e. degree == 8");
  CHECK_DEBUG0(st->levels >= 0);

  getSpheres(st, sur, 2, surTester);
}

void STGOctree::filterTriangles(Array<int> *selTris, const Array<int> &srcTris,
                                const Surface &sur, const Point3D &pMin,
                                float edgeLength) {
  selTris->setSize(0);

  double extra = edgeLength * EPSILON_LARGE + EPSILON_LARGE;

  int numTris = srcTris.getSize();
  for (int i = 0; i < numTris; i++) {
    //  get triangle
    int tNum = srcTris.index(i);
    const Surface::Triangle *tri = &sur.triangles.index(tNum);

    //  get vertices
    Point3D pTri[3];
    for (int j = 0; j < 3; j++)
      pTri[j] = sur.vertices.index(tri->v[j]).p;

    //  test for triangle - cube overlap
    if (overlapTest(pMin, edgeLength, pTri, extra))
      selTris->addItem() = tNum;
  }
}

void STGOctree::getChildren(SphereTree *tree, const Surface &sur, int node,
                            int level, int divs, const Point3D &pMin,
                            float size, const Array<int> &tris,
                            const SurfaceTester *surTester) {
  //  work out the size of the new spheres
  float subSize = size / divs;

  //  generate children and recurse
  int firstChild = tree->getFirstChild(node);
  int numChildren = 0;
  for (int i = 0; i < divs; i++) {
    float x = pMin.x + i * subSize;
    for (int j = 0; j < divs; j++) {
      float y = pMin.y + j * subSize;
      for (int k = 0; k < divs; k++) {
        float z = pMin.z + k * subSize;
        Point3D pMinNew = {x, y, z};

        //  find the triangles overlapping the cube
        Array<int> selTris;
        filterTriangles(&selTris, tris, sur, pMinNew, subSize);

        //   create sphere
        STSphere sph;
        sph.c.x = x + subSize / 2.0f;
        sph.c.y = y + subSize / 2.0f;
        sph.c.z = z + subSize / 2.0f;
        sph.r = sph.c.distance(pMinNew);

        if (selTris.getSize()) {
          //  store sphere
          int childNum = firstChild + numChildren;
          numChildren++;
          tree->nodes.index(childNum) = sph;

          //  generate children
          if (level < tree->levels - 1)
            getChildren(tree, sur, childNum, level + 1, divs, pMinNew, subSize,
                        selTris, surTester);
        } else if (surTester) { //  making solid octree
          //  test center point
          Point3D pC;
          pC.x = x + subSize / 2.0f;
          pC.y = y + subSize / 2.0f;
          pC.z = z + subSize / 2.0f;
          Point3D pClose;
          bool in = surTester->getClosestPointConditional(&pClose, pC, true);

          if (in) {
            //  store sphere
            int childNum = firstChild + numChildren;
            numChildren++;
            tree->nodes.index(childNum) = sph;

            //  generate children (if the sphere overlap's surface)
            int completelyInternal = pClose.distance(pC) > sph.r;
            // OUTPUTINFO("Internal Node (%s)\n", completelyInternal?
            // "Complete":"SubDivide");
            if (level < tree->levels - 1 && !completelyInternal)
              getChildren(tree, sur, childNum, level + 1, divs, pMinNew,
                          subSize, selTris, surTester);
          }
        }
      }
    }
  }

  //  invalidate rest of child spheres
  int totChild = divs * divs * divs;
  for (int i = numChildren; i < totChild; i++) {
    int childNum = firstChild + i;
    tree->initNode(childNum, level + 1);
  }
}

void STGOctree::getSpheres(SphereTree *tree, const Surface &sur, int divs,
                           const SurfaceTester *surTester) {
  //  get bounding box
  STSphere s;
  float sizeX = sur.pMax.x - sur.pMin.x;
  float sizeY = sur.pMax.y - sur.pMin.y;
  float sizeZ = sur.pMax.z - sur.pMin.z;
  float size = sizeX;
  if (sizeY > size)
    size = sizeY;
  if (sizeZ > size)
    size = sizeZ;

  //  get bounding sphere
  // s.c.x = sur.pMin.x + size/2.0f;
  // s.c.y = sur.pMin.y + size/2.0f;
  // s.c.z = sur.pMin.z + size/2.0f;
  // s.r = s.c.distance(sur.pMin);
  // tree->nodes.index(0) = s;

  // changed to use tighter sphere
  SFWhite::makeSphere(&tree->nodes.index(0), sur.vertices);

  //  generate list of triangles contained in the parent box (for recursion)
  Array<int> tris;
  int numTris = sur.triangles.getSize();
  tris.resize(numTris);
  for (int i = 0; i < numTris; i++)
    tris.index(i) = i;

  getChildren(tree, sur, 0, 1, divs, sur.pMin, size, tris, surTester);
}
