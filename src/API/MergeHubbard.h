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

#ifndef _MERGE_HUBBARD_H_
#define _MERGE_HUBBARD_H_

#include "../Geometry/Sphere.h"
#include "../Storage/Array.h"
#include "../MedialAxis/Voronoi3D.h"
#include "../MedialAxis/MedialTester.h"

class MergeHubbard {
public:
  //  constructor
  MergeHubbard();

  //  setup
  void setup(Voronoi3D *vor, const MedialTester *mt);
  void getBoundingSphere(Sphere *s) const;

  //  reduce sphere set
  void getSpheres(Array<Sphere> *spheres, int numDest,
                  const Array<int> *filterInds = NULL) const;

private:
  //  internals
  struct Merger {
    int i1, i2;
    Sphere s;
    float cost;
  };

  struct MedialSphere {
    bool valid;
    Sphere s;
    Array<int> neighbours, pts;
    Array<int> vertices;
    float error;
  };

  //  list of forming points
  Array<Surface::Point> formingPoints;

  //  voronoi diagram etc.
  Voronoi3D *vor;
  const MedialTester *mt;

  //  internals
  void constructSphereSet(Array<MedialSphere> *medialSpheres,
                          const Array<int> *filterInds) const;
  void iterativeMerge(Array<MedialSphere> *medialSpheres, int numDest) const;
  void constructMerger(Merger *merger, const Array<MedialSphere> &medialSpheres,
                       int i1, int i2) const;
  void constructMergers(Array<Merger> *mergers,
                        const Array<MedialSphere> &medialSpheres) const;
  void constructAllMergers(Array<Merger> *mergers,
                           const Array<MedialSphere> &medialSpheres) const;
  void applyMerger(Array<Merger> *mergers, Array<MedialSphere> *medialSpheres,
                   int mergI, bool updateHouseKeeping) const;

  //  statics
  static void mergeLists(Array<int> *dest, const Array<int> &l);
  static void combineLists(Array<int> *dest, const Array<int> &l1,
                           const Array<int> &l2);

  friend class STGHubbard;
};

#endif