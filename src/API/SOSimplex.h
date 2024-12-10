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
    Simplex based sphere optimiser.
*/
#ifndef _API_SIMPLEX_SPHERE_OPTIMISER_H_
#define _API_SIMPLEX_SPHERE_OPTIMISER_H_

#include "SOBase.h"
#include "SFBase.h"
#include "SEBase.h"

class SOSimplex : public SOBase {
public:
  //  parameters
  SFBase *sphereFitter;
  bool useErr, useVol;

  //  constructor
  SOSimplex();

  //  optimise
  void optimise(Array<Sphere> *spheres, const SurfaceRep &surRep,
                float stopBelow = -1, const Sphere *parSph = NULL,
                int level = 0) const;

private:
  struct OptInfo {
    OptInfo() {}

    const Array<Sphere> *srcSph;
    const Array<double> *startErr;
    const Array<Array<int> /**/> *sphInd;
    const SurfaceRep *surRep;
    const Sphere *parSph;
    int repInd; //  sphere to be optimised
    const SOSimplex *that;

    //  minimum error configuration discovered during optimisation
    double resErr;
    Array<double> resErrors;
    Array<Sphere> resSph;
    Array<Array<int> /**/> resInd;
  };

  static double computeErrorMetric(const Array<double> &err,
                                   const Array<Sphere> &sph);
  static double configEval(double vals[], void *data, int *canFinish);
  static double computeOverlap(const Array<Sphere> &sph, int J);

  double evalSphere(const Sphere &s, const Sphere *parSph) const;
  double myRefitSphere(Sphere *s, const Array<Surface::Point> &pts,
                       const Array<int> &inds, const Point3D &pC) const;

  void generateNewPointConfig(Array<Array<int> /**/> *newSphInd,
                              Array<bool> *changed,
                              const Array<Array<int> /**/> &sphInd,
                              const Array<Sphere> &srcSph,
                              const SurfaceRep &surRep, const Sphere &repSph,
                              int repInd) const;
  void generateNewConfig(Array<Sphere> *newSph,
                         Array<Array<int> /**/> *newSphInd,
                         Array<double> *newErr, const Array<Sphere> &srcSph,
                         const Array<Array<int> /**/> &srcSphInd,
                         const Array<double> &srcErr, const SurfaceRep &surRep,
                         const Sphere &repSph, int repInd,
                         const Sphere *parSph) const;

  void assignPointsAndGetErr(Array<Array<int> /**/> *sphInd,
                             Array<double> *initErr, Array<Sphere> *sph,
                             const SurfaceRep &surRep,
                             const Sphere *parSph) const;
  bool optimiseSphere(Array<Sphere> *sph, Array<Array<int> /**/> *sphInd,
                      Array<double> *initErr, const SurfaceRep &surRep,
                      int optNum, const Sphere *parSph) const;
};

#endif
