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

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

//  NOTE : STOPBELOW IS SPEED UP FOR DODEC SPHERE DISTANCE - IF WE KNOW THIS
//  POINT ISN'T GOING TO BE THE WORST

#include "../Geometry/Point3D.h"
#include "../Surface/Surface.h"
#include "../Storage/Array.h"
#include "../Base/Defs.h"
#include "SpacialHash.h"
#include "InternalGem.h"

enum DIST_TYPE { FACE, EDGE, VERTEX, DIST_TYPE_INVALID };
struct ClosestPointInfo {
  int triangle;   //  triangle number
  int num;        //  edge/vertex number
  DIST_TYPE type; //  type of closest feature
  Point3D pClose; //  closest point
};

//  distance from triangle
double distanceToTriangle(ClosestPointInfo *inf, const Point3D &pTest,
                          const Point3D P[3]);

//  closest point on triangulated surface
//  inf->type == DIST_TYPE_INVALID if no point found
double getClosestPointOrdered(ClosestPointInfo *inf, const Point3D &pTest,
                              const Surface &s, const SpacialHash &faceHash,
                              float stopBelow = -FLT_MAX);
double getClosestPoint(ClosestPointInfo *inf, const Point3D &pTest,
                       const Surface &s, const SpacialHash &faceHash,
                       float stopBelow = -FLT_MAX);
double getClosestPointBrute(ClosestPointInfo *inf, const Point3D &pTest,
                            const Surface &s);

//  inside test for triangulated surface
bool insideSurfaceClosest(const Point3D &p, const Surface &s,
                          const SpacialHash &faceHash,
                          ClosestPointInfo *inf = NULL,
                          float stopBelow = -FLT_MAX,
                          bool allowQuickTest = TRUE);

//  inside test using crossings test
bool insideSurfaceCrossing(const Point3D &p, const Surface &s,
                           const SpacialHash &faceHash);
bool insideSurfaceCrossingIter(const Point3D &p, const Surface &s,
                               const SpacialHash &faceHash, float mult = 2.0f);

//  wrapper to allow me to choose the method (HACK)
bool insideSurface(const Point3D &p, const Surface &s,
                   const SpacialHash &faceHash,
                   const SpacialHash &faceHashHiRes,
                   ClosestPointInfo *inf = NULL, float stopBelow = -FLT_MAX,
                   bool allowQuickTest = TRUE);

#endif
