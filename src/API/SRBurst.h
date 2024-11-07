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
    Sphere Reducer which replaces the merging from Hubbard's algorithm with a
    bursting strategy.

    Beneficial Merges (i.e. those with reduce error) work well for the burst
    algorithm and therefore is the default
*/
#ifndef _API_BURST_SPHERE_REDUCER_H_
#define _API_BURST_SPHERE_REDUCER_H_

#include "SFBase.h"
#include "SEBase.h"
#include "SRVoronoi.h"
#include "REBase.h"

class SRBurst : public SRVoronoi {
public:
  //  parameters
  const SFBase *sphereFitter;
  const SEBase *sphereEval;

  //  use forming points instead of surRep for coverage (for comparisons)
  bool useFormingPoints;

  //  consider ALL pairs below this number of spheres
  int doAllBelow;

  //  eliminate redundent spheres during merge (will affect later processing)
  int doRedundantCheckBelow;
  REBase *redElim; //  uses RELargest if none given

  //  use beneficial merges first
  bool useBeneficial;

  //  constructor
  SRBurst();

  //  reduce
  void getSpheres(Array<Sphere> *spheres, int n, const SurfaceRep &surRep,
                  const Sphere *filterSphere = NULL,
                  float parSphereErr = -1) const;

private:
  struct SphereRemove {
    Array<int> pointMap;
    float cost;
  };
  void constructRemoval(SphereRemove *removal,
                        const Array<MedialSphere> &medialSpheres, int i,
                        const Array<Surface::Point> &surPts) const;
  void constructRemovals(Array<SphereRemove> *removals,
                         const Array<MedialSphere> &medialSpheres,
                         const Array<Surface::Point> &surPts) const;
  float costOfRemoval(SphereRemove &removal,
                      const Array<MedialSphere> &medialSpheres, int sI,
                      const Array<Surface::Point> &surPts) const;
  void applyRemoval(Array<SphereRemove> *removals,
                    Array<MedialSphere> *medialSpheres, int sI,
                    const Array<Surface::Point> &surPts,
                    bool updateHouseKeeping) const;
  void makeAllNeighbours(Array<SphereRemove> *removals,
                         Array<MedialSphere> *medialSpheres,
                         const Array<Surface::Point> &surPts) const;

  void saveImagePre(const Array<MedialSphere> &medialSpheres,
                    const Array<int> &ptList, int remNum, int maxNum) const;
  void saveImagePost(const Array<MedialSphere> &medialSpheres,
                     const Array<int> &ptList, int remNum, int maxNum) const;
};

#endif