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
    Optimiser that segments object and fits least error sphere to each region

    doesn't use parSph or stop below yet
*/
#ifndef _SO_PER_SPHERE_H_
#define _SO_PER_SPHERE_H_

#include "SEBase.h"
#include "SOBase.h"

class SOPerSphere : public SOBase {
public:
  const SEBase *eval;
  int numIter;

  SOPerSphere();

  //  optimise
  void optimise(Array<Sphere> *spheres, const SurfaceRep &surRep,
                float stopBelow = -1, const Sphere *parSph = NULL,
                int level = 0) const;

  //  optimise a single sphere (need to pass initial guess)
  void optimise(Sphere *s, const Array<Point3D> &pts) const;

private:
  struct OptInfo {
    const SEBase *eval;
    const Array<Point3D> *selPts;
  };
  static double fitFunc(double vals[], void *data, int *canFinish);
};

#endif
