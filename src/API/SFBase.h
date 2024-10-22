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
    Interface for SphereFit objects.  These sphere fit objects are to fit a
   bounding sphere around a set of surface sample points.
*/
#ifndef _API_SPHERE_FIT_H_
#define _API_SPHERE_FIT_H_

#include "../Geometry/Sphere.h"
#include "../Surface/Surface.h"
#include "SEBase.h"

class SFBase {
public:
  //  fit the sphere around all the samples in a set
  bool fitSphere(Sphere *s, const Array<Surface::Point> &points) const;

  //  fit the sphere around a sub set of the samples
  bool fitSphere(Sphere *s, const Array<Surface::Point> &points,
                 const Array<int> &inds) const;

  //  implementation
  virtual bool fitSphere(Sphere *s, const Array<Point3D> &points) const = 0;

protected:
  //  utilities
  static void convertPoints(Array<Point3D> *dest,
                            const Array<Surface::Point> &points);
  static void convertPoints(Array<Point3D> *dest,
                            const Array<Surface::Point> &points,
                            const Array<int> &inds);
};

double refitSphere(Sphere *s, const Array<Surface::Point> &pts,
                   const Array<int> &inds, const SEBase *se, const SFBase *sf,
                   const Point3D *p1, const Point3D *p2 = NULL);

#endif
