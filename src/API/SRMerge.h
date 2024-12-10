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
    SphereSetReducer which uses merging algorithm

    Beneficial Merges (i.e. those with reduce error) DOESN'T work well for the
    merge algorithm and therefore is OFF by default.
*/
#ifndef _API_MERGE_SPHERE_REDUCER_H_
#define _API_MERGE_SPHERE_REDUCER_H_

#include "SRVoronoi.h"
#include "SFBase.h"
#include "SEBase.h"
#include "REBase.h"

class SRMerge : public SRVoronoi {
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

  //  use beneficial merges
  bool useBeneficial;

  //  constructor
  SRMerge();

  //  reduce sphere set
  virtual void getSpheres(Array<Sphere> *spheres, int n,
                          const SurfaceRep &surRep,
                          const Sphere *filterSphere = NULL,
                          float parSphereErr = -1) const;

private:
  //  internals
  struct Merger {
    int i1, i2;
    Sphere s;
    float cost;
    float newErr;
  };

  void constructMerger(Merger *merger, const Array<MedialSphere> &medialSpheres,
                       const Array<Surface::Point> *surPts, int i1,
                       int i2) const;
  void constructMergers(Array<Merger> *mergers,
                        const Array<MedialSphere> &medialSpheres,
                        const Array<Surface::Point> *surPts) const;
  void constructAllMergers(Array<Merger> *mergers,
                           const Array<MedialSphere> &medialSpheres,
                           const Array<Surface::Point> *surPts) const;
  void applyMerger(Array<Merger> *mergers, Array<MedialSphere> *medialSpheres,
                   const Array<Surface::Point> *surPts, int mergI,
                   bool updateHouseKeeping) const;

  void saveImage(const Array<MedialSphere> &medialSpheres, const Merger &merger,
                 int numMax) const;

  //  statics
  static void mergeLists(Array<int> *dest, const Array<int> &l);
  static void combineLists(Array<int> *dest, const Array<int> &l1,
                           const Array<int> &l2);

  friend class STGHubbard;
};

#endif