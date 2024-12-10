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

#include "Voxel.h"
#include "../Geometry/CubeTri.h"
#include "Internal.h"

/*
    Level of the Voxel Tree
*/
VoxelLevel::VoxelLevel() {
  d = 0;
  l = 0;
}

void VoxelLevel::setup(const Point3D pMin, float l, int d) {
  flags.resize(d * d * d);
  this->d = d;
  this->l = l;
  this->pMin = pMin;
}

char VoxelLevel::getFlag(int x, int y, int z) const {
  return flags.index(x * d * d + y * d + z);
}

void VoxelLevel::setFlag(int x, int y, int z, char val) {
  flags.index(x * d * d + y * d + z) = val;
}

char VoxelLevel::getFlag(const Point3D &p) const {
  if (p.x < pMin.x || p.x > pMin.x + l || p.y < pMin.y || p.y > pMin.y + l ||
      p.z < pMin.z || p.z > pMin.z + l)
    return VL_FLAG_OUTSIDE;

  //  work out the cell location
  float cL = getLength();
  int i = (p.x - pMin.x) / cL;
  int j = (p.y - pMin.y) / cL;
  int k = (p.z - pMin.z) / cL;

  //  do inside check
  if (i >= 0 && i < d && j >= 0 && j < d && k >= 0 && k < d)
    return getFlag(i, j, k);
  else
    return VL_FLAG_OUTSIDE;
}

void VoxelLevel::getMinPt(Point3D *p, int x, int y, int z) const {
  float cL = getLength();
  p->x = pMin.x + x * cL;
  p->y = pMin.y + y * cL;
  p->z = pMin.z + z * cL;
}

void VoxelLevel::getMaxPt(Point3D *p, int x, int y, int z) const {
  float cL = getLength();
  p->x = pMin.x + (x + 1) * cL;
  p->y = pMin.y + (y + 1) * cL;
  p->z = pMin.z + (z + 1) * cL;
}

void VoxelLevel::getMidPt(Point3D *p, int x, int y, int z) const {
  float cL = getLength();
  p->x = pMin.x + (x + 0.5) * cL;
  p->y = pMin.y + (y + 0.5) * cL;
  p->z = pMin.z + (z + 0.5) * cL;
}

float VoxelLevel::getLength() const { return l / (float)d; }

/*
    Voxel Tree
*/
VoxelTree::VoxelTree() {
  sur = NULL;
  faceHash = NULL;
  faceHashHiRes = NULL;
}

void VoxelTree::setup(const Surface &sur, const SpacialHash &faceHash,
                      const SpacialHash &faceHashHiRes, int numLevels) {
  this->sur = &sur;
  this->faceHash = &faceHash;
  this->faceHashHiRes = &faceHashHiRes;

  //  work out the bounding box
  Point3D pMin = sur.pMin;
  float lX = sur.pMax.x - sur.pMin.x;
  float lY = sur.pMax.y - sur.pMin.y;
  float lZ = sur.pMax.z - sur.pMin.z;
  float l = lX;
  if (lY > l)
    l = lY;
  if (lZ > l)
    l = lZ;

  //  create the levels
  int d = pow(2, numLevels);
  level.setup(pMin, l, d);

  //  initialise the list of triangle
  Array<int> tris;
  int numTris = sur.triangles.getSize();
  tris.resize(numTris);
  for (int i = 0; i < numTris; i++)
    tris.index(i) = i;

  //  fill the levels
  fillChildren(sur, faceHash, faceHashHiRes, 0, 0, 0, 0, tris, numLevels, pMin,
               l / 2.0);

  //  verify lowest set of cells
  /*{
    const VoxelLevel *vl = &level;
    float eL = vl->getLength();
    int d = vl->getD();
    OUTPUTINFO("d = %d\n", d);
    for (int i = 0; i < d; i++){
      for (int j = 0; j < d; j++){
        for (int k = 0; k < d; k++){
          char flag = vl->getFlag(i, j, k);

          Point3D pMin;
          vl->getMinPt(&pMin, i, j, k);
          Point3D pMid;
          vl->getMidPt(&pMid, i, j, k);

          char flag1 = vl->getFlag(pMid);
          if (flag1 != flag)
            OUTPUTINFO("Test Fail : %d %d\n", flag, flag1);

          Array<int> selTri;
          filterTriangles(&selTri, tris, sur, pMin, eL);
          OUTPUTINFO("%d Triangles\n", selTri.getSize());
          if (selTri.getSize()){
            //  EDGE
            if (flag != VL_FLAG_EDGE)
              OUTPUTINFO("Should be VL_FLAG_EDGE was (%d)\n", flag);
            }
          else{
            //  IN/OUT
            bool in = ::insideSurface(pMid, sur, sh);

            if (in && flag != VL_FLAG_INTERNAL)
              OUTPUTINFO("Should be VL_FLAG_INTERNAL was (%d)\n", flag);
            else if (!in && flag != VL_FLAG_OUTSIDE)
              OUTPUTINFO("Should be VL_FLAG_INTERNAL was (%d)\n", flag);
            }
          }
        }
      }
    }*/
}

void VoxelTree::fillChildren(const Surface &sur, const SpacialHash &faceHash,
                             const SpacialHash &faceHashHiRes, int levelNum,
                             int pX, int pY, int pZ, const Array<int> &tris,
                             int numLevels, const Point3D &PMIN, float eL) {
  //  for each voxel
  for (int x = 0; x < 2; x++) {
    int cX = pX * 2 + x;

    for (int y = 0; y < 2; y++) {
      int cY = pY * 2 + y;

      for (int z = 0; z < 2; z++) {
        int cZ = pZ * 2 + z;

        Point3D pMin;
        pMin.x = PMIN.x + cX * eL;
        pMin.y = PMIN.y + cY * eL;
        pMin.z = PMIN.z + cZ * eL;

        //  test if the cell is an edge cell
        Array<int> subTris;
        filterTriangles(&subTris, tris, sur, pMin, eL);
        if (subTris.getSize()) {
          if (levelNum == numLevels - 1)
            level.setFlag(cX, cY, cZ, VL_FLAG_EDGE);
          else
            fillChildren(sur, faceHash, faceHashHiRes, levelNum + 1, cX, cY, cZ,
                         subTris, numLevels, PMIN, eL / 2.0);
        } else {
          //  test if the cell is inside the object
          Point3D pMid;
          pMid.x = PMIN.x + (cX + 0.5) * eL;
          pMid.y = PMIN.y + (cY + 0.5) * eL;
          pMid.z = PMIN.z + (cZ + 0.5) * eL;

          bool in = ::insideSurface(pMid, sur, faceHash, faceHashHiRes);
          char flag = in ? VL_FLAG_INTERNAL : VL_FLAG_OUTSIDE;
          if (levelNum == numLevels - 1)
            level.setFlag(cX, cY, cZ, flag);
          else
            fillFlags(levelNum + 1, cX, cY, cZ, flag, numLevels);
        }
      }
    }
  }
}

void VoxelTree::fillFlags(int levelNum, int pX, int pY, int pZ, int flag,
                          int numLevels) {
  //  left over from the days of having multiple levels - TIDY UP

  //  for each voxel
  for (int x = 0; x < 2; x++) {
    int cX = pX * 2 + x;

    for (int y = 0; y < 2; y++) {
      int cY = pY * 2 + y;

      for (int z = 0; z < 2; z++) {
        int cZ = pZ * 2 + z;
        if (levelNum == numLevels - 1)
          level.setFlag(cX, cY, cZ, flag);
        else
          fillFlags(levelNum + 1, cX, cY, cZ, flag, numLevels);
      }
    }
  }
}

void VoxelTree::filterTriangles(Array<int> *selTris, const Array<int> &srcTris,
                                const Surface &sur, const Point3D &pMin,
                                float edgeLength) {
  selTris->setSize(0);

  int numTris = srcTris.getSize();
  for (int i = 0; i < numTris; i++) {
    //  get triangle
    int tNum = srcTris.index(i);
    const Surface::Triangle *tri = &sur.triangles.index(tNum);

    //  get vertices
    Point3D pTri[3];
    for (int j = 0; j < 3; j++)
      pTri[j] = sur.vertices.index(tri->v[j]).p;

    if (overlapTest(pMin, edgeLength, pTri))
      selTris->addItem() = tNum;
  }
}

bool VoxelTree::insideSurface(const Point3D &p, ClosestPointInfo *inf) const {
  if (inf)
    inf->type = DIST_TYPE_INVALID;

  //  index the bottom level directly
  const VoxelLevel *vl = &level;
  char flag = vl->getFlag(p);
  if (flag == VL_FLAG_EDGE) {
    return ::insideSurface(p, *sur, *faceHash, *faceHashHiRes, inf);
  } else {
    return (flag == VL_FLAG_INTERNAL);
  }
}
