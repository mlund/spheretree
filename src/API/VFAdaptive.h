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
    Voronoi Fixer which adaptively samples the surface of the object so as to
    improve the accuracy of the voronoi diagram
*/
#ifndef _API_ADAPTIVE_VORONOI_FIXER_H_
#define _API_ADAPTIVE_VORONOI_FIXER_H_

#include "SEBase.h"
#include "../MedialAxis/MedialTester.h"
#include "../Surface/SurfaceRep.h"

class VFAdaptive {
public:
  //  parameters
  const SEBase *eval;
  const MedialTester *mt;
  const SurfaceRep *coverRep;

  //  constructor
  VFAdaptive();

  //  fix
  void adaptiveSample(Voronoi3D *vor, float maxErr, int maxSam = -1,
                      int maxSph = -1, int minSph = 0,
                      const SurfaceRep *coverRep = NULL,
                      const Sphere *filterSphere = NULL,
                      bool countOnlyCoverSpheres = false, int maxLoops = -1);
  static void
  adaptiveSample(Voronoi3D *vor, const MedialTester &mt,
                 const SurfaceRep *coverRep = NULL, float maxErr = EPSILON,
                 int maxSam = -1, int maxSph = -1, int minSph = 0,
                 const Sphere *filterSphere = NULL, const SEBase *eval = NULL,
                 bool countOnlyCoverSpheres = false, int maxLoops = -1);

private:
  static double getErr(Voronoi3D::Vertex *vert, const SEBase *eval,
                       const MedialTester *mt, const Voronoi3D *vor);
  static int findWorstSphere(Voronoi3D *vor, const MedialTester &mt,
                             const SEBase *eval, const Sphere *filterSphere,
                             float *resultErr, int *numInternal,
                             bool includeCover);
};

#endif
