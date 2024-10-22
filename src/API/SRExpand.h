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
    Sphere Reducer which expands the medial spheres out until they only the
   required number are needed.


    NOTE : when using numSpheresPerNode we only count spheres which cover the
   surface points in SurfaceRep.  This means that we will start with this
   number. This means that 5-10 times the destination number of spheres should
   be more than enough.


    tryIter == 0 for no iteration
    tryIter == 1 for full iteration
    tryIter == 2 for early end
*/
#ifndef _API_EXPAND_SPHERE_REDUCER_H_
#define _API_EXPAND_SPHERE_REDUCER_H_

#include "REBase.h"
#include "SRStandOff.h"
#include "VFAdaptive.h"
#include "../MedialAxis/Voronoi3D.h"
#include "../MedialAxis/MedialTester.h"

class SRExpand : public SRStandOff {
public:
  REBase *redElim;
  VFAdaptive *vorAdapt;
  int initSpheres, minSpheresPerNode, maxItersForVoronoi;

  float errorDecreaseFactor;

  //  constructor
  SRExpand();

  //  setup
  void setup(Voronoi3D *vor, const MedialTester *mt);
  void setupForLevel(int level, int degree, const SurfaceRep *surRep) const;

  //  overloaded
  void generateExpandedSpheres(Array<Sphere> *dest, float err,
                               const Sphere *parSph = NULL) const;
  bool generateStandOffSpheres(Array<Sphere> *spheres, float err,
                               const SurfaceRep &surRep, int maxNum = -1,
                               int tryIter = 0,
                               const Sphere *parSph = NULL) const;
  bool setupFilterSphere(const Sphere *filterSphere, float parSphErr,
                         const SurfaceRep *surRep) const;

protected:
  Voronoi3D *vor;
  const MedialTester *mt;
};

#endif