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

/*
    Vexelisation to accelerate inside/outside testing for surfaces
*/
#ifndef _VOXEL_VOL_H_
#define _VOXEL_VOL_H_

#include "../Storage/Array.h"
#include "../Geometry/Point3D.h"
#include "Surface.h"
#include "SpacialHash.h"
#include "Internal.h"

#define VL_FLAG_EDGE 0
#define VL_FLAG_OUTSIDE 1
#define VL_FLAG_INTERNAL 2

class VoxelLevel {
public:
  VoxelLevel();

  void setup(const Point3D pMin, float l, int d);

  char getFlag(int x, int y, int z) const;
  void setFlag(int x, int y, int z, char val);
  char getFlag(const Point3D &p) const;
  int getD() const { return d; }

  void getMinPt(Point3D *p, int x, int y, int z) const;
  void getMaxPt(Point3D *p, int x, int y, int z) const;
  void getMidPt(Point3D *p, int x, int y, int z) const;
  float getLength() const;

private:
  int d;
  float l;
  Point3D pMin;
  Array<char> flags;
};

class VoxelTree {
public:
  VoxelTree();

  void setup(const Surface &sur, const SpacialHash &faceHash,
             const SpacialHash &faceHashHiRes, int levels);

  bool insideSurface(const Point3D &p, ClosestPointInfo *inf = NULL) const;

private:
  const Surface *sur;
  const SpacialHash *faceHash, *faceHashHiRes;
  VoxelLevel level;

  void filterTriangles(Array<int> *selTris, const Array<int> &srcTris,
                       const Surface &sur, const Point3D &pMin,
                       float edgeLength);
  void fillChildren(const Surface &sur, const SpacialHash &faceHash,
                    const SpacialHash &faceHashHiRes, int levelNum, int pX,
                    int pY, int pZ, const Array<int> &tris, int numLevels,
                    const Point3D &PMIN, float eL);
  void fillFlags(int levelNum, int pX, int pY, int pZ, int flag, int numLevels);
};

#endif