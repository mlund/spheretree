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

#include "SRQuickBurst.h"

SRQuickBurst::SRQuickBurst() { burstThreshold = 5; }

void SRQuickBurst::getSpheres(Array<Sphere> *spheres, int n,
                              const SurfaceRep &surRep,
                              const Sphere *filterSphere,
                              float parSphereErr) const {
  CHECK_DEBUG(
      burstThreshold > 1,
      "Need to give value > 1 or the algorithm will end up being SRMerge");

  //  get the reduced set containing a multiple of N
  Array<Sphere> tmpSpheres;
  this->SRMerge::getSpheres(&tmpSpheres, burstThreshold * n, surRep,
                            filterSphere, parSphereErr);

  //  then do burst for the rest
  SRBurst burster;
  burster.doAllBelow = this->doAllBelow;
  burster.useBeneficial = this->useBeneficial;
  burster.useFormingPoints = this->useFormingPoints;
  burster.sphereFitter = this->sphereFitter;
  burster.sphereEval = this->sphereEval;
  burster.setup(tmpSpheres);
  burster.getSpheres(spheres, n, surRep, filterSphere, parSphereErr);
}
