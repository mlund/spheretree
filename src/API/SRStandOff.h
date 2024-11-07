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
    Base class for algorithms which use the StandOff distance for generating
   spheres i.e. we need to search for the correct standoff distance to get the
   required number of spheres.
*/
#ifndef _API_STANDOFF_SPHERE_REDUCER_H_
#define _API_STANDOFF_SPHERE_REDUCER_H_

#include "SRBase.h"
#include "REBase.h"
#include "SFBase.h"

class SRStandOff : public SRBase {
public:
  float relTol;
  float startErr;
  float errStep;
  bool stopExact;
  const SFBase *refitter;
  float lastStandoff; //  hack for generating graphs
  bool useIterativeSelect;

  //  constructor
  SRStandOff();

  //  reduction
  void getSpheresA(Array<Sphere> *spheres, int numDest,
                   const SurfaceRep &surRep, const Sphere *filterSphere = NULL,
                   float parSphErr = -1) const;
  void getSpheresB(Array<Sphere> *spheres, int numDest,
                   const SurfaceRep &surRep, const Sphere *filterSphere = NULL,
                   float parSphErr = -1) const;
  void getSpheres(Array<Sphere> *spheres, int numDest, const SurfaceRep &surRep,
                  const Sphere *filterSphere = NULL,
                  float parSphErr = -1) const;

  //  overload this to get the spheres
  virtual bool generateStandOffSpheres(Array<Sphere> *spheres, float err,
                                       const SurfaceRep &surRep,
                                       int maxNum = -1, int tryIter = 0,
                                       const Sphere *parSph = NULL) const = 0;
  virtual bool setupFilterSphere(const Sphere *filterSphere, float parSphErr,
                                 const SurfaceRep *surRep) const = 0;
};

#endif
