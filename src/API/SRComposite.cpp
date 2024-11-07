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

#include "SRComposite.h"
#include "SOPerSphere.h"

SRComposite::SRComposite() {
  eval = NULL;
  useRefit = true;
}

void SRComposite::resetReducers() { reducers.setSize(0); }

void SRComposite::addReducer(SRBase *red) { reducers.addItem() = red; }

void SRComposite::setupForLevel(int level, int degree,
                                const SurfaceRep *surRep) const {
  //  pass call to the all the reducers
  int numRed = reducers.getSize();
  for (int i = 0; i < numRed; i++) {
    SRBase *red = reducers.index(i);
    red->setupForLevel(level, degree, surRep);
  }
}

void SRComposite::getSpheres(Array<Sphere> *spheres, int n,
                             const SurfaceRep &surRep,
                             const Sphere *filterSphere,
                             float parSphereErr) const {
  CHECK_DEBUG0(eval != NULL);
  CHECK_DEBUG0(reducers.getSize() > 0);

  //  setup the optimiser
  SOPerSphere perSphere;
  perSphere.numIter = 3;
  perSphere.eval = eval;

  //  do reductions
  int bestReducer = -1;
  double bestErr = DBL_MAX;
  int numRed = reducers.getSize();

  for (int i = 0; i < numRed; i++) {
    //  get the reducer
    SRBase *red = reducers.index(i);

    // do reduction
    Array<Sphere> localSph;
    red->getSpheres(&localSph, n, surRep, filterSphere, parSphereErr);

    //  do refit if necessary
    if (useRefit)
      perSphere.optimise(&localSph, surRep);

    //  get the error in this approximation
    float maxErr = 0;
    int numSph = localSph.getSize();
    for (int j = 0; j < numSph; j++) {
      double err = eval->evalSphere(localSph.index(j));
      if (err > maxErr)
        maxErr = err;
    }

    //  decide if we should keep this set
    if (maxErr < bestErr) {
      bestReducer = i;
      bestErr = maxErr;
      spheres->clone(localSph);
    }
  }

  OUTPUTINFO("Choose to use reducer number %d for this node\n", bestReducer);
}
