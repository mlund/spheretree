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
    Hubbard's relaxation technique for sampling the faces of the mesh
    also need to sample the edges as this technique prevents points
    being placed there.
*/
#ifndef _API_RELAXATION_MESH_SAMPLER_H_
#define _API_RELAXATION_MESH_SAMPLER_H_

#include "MSBase.h"
#include "../Geometry/Vector2D.h"
#include "../Geometry/Line2D.h"
#include "../Geometry/LineSeg.h"

class MSRelaxation : public MSBase {
public:
  //  parameters
  bool useVert;
  float edgeFrac;
  int maxIters;

  //  constructor
  MSRelaxation();
  MSRelaxation(const Surface &sur);

  //  setup
  void setSurface(const Surface &sur);

  //  sample
  virtual void generateSamples(Array<Surface::Point> *pts,
                               int numSamples) const = 0;
  static void generateSamples(Array<Surface::Point> *pts, int numSamples,
                              const Surface &sur, bool useVert,
                              float edgeFrac = 0.1f, int maxIter = 50);

private:
  //  internals for face relaxation
  static void randomTriangleSample(Point3D *p, const Point3D &p0,
                                   const Point3D &p1, const Point3D &p2);
  static float getWeight(const Vector2D &v, float rad);
  static void accumulateForce(Vector2D *vDest, const Point2D &p,
                              const Point2D &p1, float weight);
  static void accumulateForce(Vector2D *vDest, const Point2D &p,
                              const LineSeg &ls, const Line2D &l, float weight);
  static void relaxTriangle(Array<Point3D> *srcPts, const Point3D p[3],
                            int maxIter);

  //  internals for edges
  struct Edge {
    int v1, v2;
    Vector3D nE;
  };
  static void sampleEdges(Array<Surface::Point> *pts, int numPts,
                          const Surface &surface);
};

#endif
