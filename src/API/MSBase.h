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
    Interface for Mesh Samplers.  Generates a set of Surface Points distributed
   across a Surface mesh.
*/
#ifndef _API_MESH_SAMPLER_H_
#define _API_MESH_SAMPLER_H_

#include "../Surface/Surface.h"
#include "../Geometry/Point2D.h"
#include "../Geometry/Line2D.h"

class MSBase {
public:
  //  constructor
  MSBase();

  // setup
  void setSurface(const Surface &sur);

  //  sample
  virtual void generateSamples(Array<Surface::Point> *pts,
                               int numSamples) const = 0;

protected:
  const Surface *sur;
  static float areaTriangle(const Point3D &p0, const Point3D &p1,
                            const Point3D &p2);
  static void attributeCountsToFaces(Array<int> *counts, const Surface &sur,
                                     int numSamples);
  static bool inTriangle(const Point2D &p, const Line2D l[3]);
};

#endif
