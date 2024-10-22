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
    Interface for Redundency Eliminator, filters a set of sphere so that it
   contains only the necessary spheres to cover a set of points.  Derived
   classes will use various heuristics to achieve this.

    Returns FALSE if the surface can't be covered using at most "maxNum" spheres
*/
#ifndef _API_REDUNDENCY_ELIMINATOR_H_
#define _API_REDUNDENCY_ELIMINATOR_H_

#include "../Geometry/Sphere.h"
#include "../Storage/Array.h"
#include "../Surface/Surface.h"
#include "../Surface/SurfaceRep.h"

class REBase {
public:
  REBase();

  virtual bool setup(const Array<Sphere> &src, const SurfaceRep &surRep);
  virtual void tidyUp();

  //  implementation of RE algorithm
  virtual bool reduceSpheres(Array<int> *inds, int maxNum = -1,
                             Array<int> *destCounts = NULL,
                             double maxMet = DBL_MAX,
                             Array<double> *mets = NULL) const = 0;

  //  this is only needed to be virtual so VC6 will find it
  virtual bool reduceSpheres(Array<Sphere> *dest, const Array<Sphere> &src,
                             const SurfaceRep &surRep, int maxNum = -1,
                             Array<int> *counts = NULL,
                             Array<int> *list = NULL);

  void getSpheres(Array<Sphere> *sph, const Array<int> &inds) const;

protected:
  const Array<Sphere> *srcSpheres;
  const SurfaceRep *surRep;
};

#endif
