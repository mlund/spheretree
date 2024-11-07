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
    Generates the set of spheres by spawning one sphere from another

    Note : at present this algorithm finds it very difficult to create
   approximations with large numnbers of spheres.  It does work well for
   approximations with low numbers i.e. when constructing sphere trees.


*/
#ifndef _API_SPAWN_SPHERE_GENERATOR_H_
#define _API_SPAWN_SPHERE_GENERATOR_H_

#include "SEBase.h"
#include "SRStandOff.h"
#include "../Surface/SurfaceTester.h"

class SRSpawn : public SRStandOff {
public:
  const SEBase *eval;

  //  constructor
  SRSpawn();

  //  setup
  void setup(const SurfaceTester &st, const SEBase *eval = NULL);
  void setupForLevel(int level, int degree,
                     const SurfaceRep *coverRep = NULL) const {};

  //  generate spheres
  bool generateSpheres(Array<Sphere> *spheres, float err,
                       const SurfaceRep &surRep, int maxNum = -1,
                       float maxR = -1) const;
  bool generateStandOffSpheres(Array<Sphere> *spheres, float err,
                               const SurfaceRep &surRep, int maxNum = -1,
                               int tryIter = 0,
                               const Sphere *parSph = NULL) const;
  bool setupFilterSphere(const Sphere *filterSphere, float parSphErr,
                         const SurfaceRep *coverRep) const {
    return true;
  }

private:
  const SurfaceTester *st;

  struct OptInfo {
    const Array<Sphere> *sph;
    const SurfaceRep *surRep;
    const Array<bool> *ptsCont;
    const SurfaceTester *st;
    float maxErr, maxRad;
    const SEBase *eval;
  };
  static Sphere makeSphereSpawn(double vals[4], float maxErr,
                                const SurfaceTester &st);
  static double spawnFunc(double vals[4], void *data, int *canFinish);
};

#endif
