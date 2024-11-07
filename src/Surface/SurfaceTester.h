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

#ifndef _SURFACE_TESTER_H_
#define _SURFACE_TESTER_H_

#include "Surface.h"
#include "SpacialHash.h"
#include "Voxel.h"
#include "../Geometry/Plane.h"

class SurfaceTester {
public:
  SurfaceTester();

  // surface
  void setSurface(const Surface &sur, int shDiv = 20, int vtLevel = 5);
  bool hasSurface() const;

  //  inout / closest point
  bool insideSurface(const Point3D &q) const;
  bool insideSurface(Point3D *p, const Point3D &q) const;
  void getClosestPoint(Point3D *p, const Point3D &q) const;
  bool getClosestPointConditional(Point3D *p, const Point3D &q, bool internal,
                                  float stopBelow = -FLT_MAX) const;

  //  ray tester returns, -1 for no hit
  float getFirstIntersectPositive(Point3D *p, const Point3D &o,
                                  const Vector3D &v) const;
  float getFirstIntersect(Point3D *p, const Point3D &o,
                          const Vector3D &v) const; // NaN for no hit

  //  bits and pieces
  const Surface *getSurface() const { return sur; }
  const VoxelTree &getVoxelTree() const { return vt; }
  const SpacialHash &getFaceHash() const { return faceHash; }

protected:
  const Surface *sur;
  SpacialHash faceHash, faceHashHiRes;
  VoxelTree vt;
};

#endif