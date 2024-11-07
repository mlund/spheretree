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
    Base class for Sphere Reducers which use the Voronoi diagram

    NOTE : when using numSpheresPerNode we only count spheres which cover the
   surface points in SurfaceRep.  This means that merge etc will start with this
   number. this means that 5-10 times the destination number of spheres should
   be more than enough.
*/

#ifndef _API_VORONOI_SPHERE_REDUCER_H_
#define _API_VORONOI_SPHERE_REDUCER_H_

#include "SRBase.h"
#include "SEBase.h"
#include "VFAdaptive.h"
#include "../MedialAxis/Voronoi3D.h"
#include "../MedialAxis/MedialTester.h"

class SRVoronoi : public SRBase {
public:
  VFAdaptive *vorAdapt;
  int initSpheres, minSpheresPerNode, maxItersForVoronoi;
  float errorDecreaseFactor;

  struct MedialSphere {
    bool valid;
    Sphere s;
    Array<int> neighbours, pts;
    float error;
    int vertexNum;
  };

  //  constructor
  SRVoronoi();

  //  setup
  void setup(Voronoi3D *vor, const MedialTester *mt);
  void setup(const Array<Sphere> &srcSph);
  void setupForLevel(int level, int degree,
                     const SurfaceRep *surRep = NULL) const;

  //  test function i.e. for graphing
  void (*func)(Array<MedialSphere> &spheres);

protected:
  //  internals
  Voronoi3D *vor;
  const MedialTester *mt;
  const Array<Sphere> *srcSph;

  void constructSphereSet(Array<MedialSphere> *medialSpheres,
                          const SurfaceRep &surRep,
                          const Sphere *filterSphere = NULL,
                          float parSphereErr = -1,
                          bool onlyOneSpherePerPoint = false) const;
  void initErrors(Array<MedialSphere> *spheres, const SEBase *eval) const;
  void checkNoNeighbours(Array<MedialSphere> *medialSpheres) const;

  void listNoNeighbours(Array<MedialSphere> *medialSpheres) const;
};

#endif