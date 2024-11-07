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
    Voronoi Fixer that searched for gap crossing cells within the Voronoi
   diagram and inserts new points to try to fix thes problems.
*/
#ifndef _API_GAP_CROSS_VORONOI_FIXER_H_
#define _API_GAP_CROSS_VORONOI_FIXER_H_

#include "VFBase.h"
#include "../MedialAxis/VoronoiFace.h"
#include "../MedialAxis/Clusters.h"

class VFGapCross : public VFBase {
public:
  //  constructor
  VFGapCross();
  VFGapCross(const Surface &sur);

  //  surface
  void setSurface(const Surface &sur);

  //  fix
  void fixVoronoi(Voronoi3D *vor) const;
  static void fixGapCrossers(Voronoi3D *vor, const Surface &sur);

private:
  const Surface *sur;

  //  internal
  static void intersectFaceCluster(Array<int> *tris, const VoronoiFace &face,
                                   const Array<int> &cluster,
                                   const Surface &sur, const Voronoi3D &vor);
  static bool inTriangle(const Point3D &p, const Point3D tri[3]);
  static void addSnaps(Array<Surface::Point> *pts, const Point3D &pProj,
                       const Vector3D &clusterNorm, const Array<int> &intTri,
                       const Surface &sur);
  static bool inClusterOrNeighbour(Point3D &p, const ClusterInfo &clusterInf,
                                   const Surface &sur, int clusterNum);
  static bool inCluster(Point3D &p, const Array<int> &cluster,
                        const Surface &sur);
  static void findGapPoints(Array<Surface::Point> *pts, const Voronoi3D &vor,
                            const Surface &sur, const ClusterInfo &clusterInf,
                            int startIndex);
};

#endif
