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
    Surface Sampler which samples a mesh using simple grid placement of
    points within triangles.
*/
#ifndef _API_GRID_MESH_SAMPLER_H_
#define _API_GRID_MESH_SAMPLER_H_

#include "MSBase.h"

class MSGrid : public MSBase {
public:
  //  parameters
  bool useVert;
  int minPerTriangle;

  //  constructor
  MSGrid();

  //  generate
  virtual void generateSamples(Array<Surface::Point> *pts,
                               int numSamples) const;
  static void generateSamples(Array<Surface::Point> *pts, int numSamples,
                              const Surface &sur, bool useVert,
                              int minPerTriangle = 0);

private:
  static void generateSamplesInternal(Array<Surface::Point> *pts,
                                      int numSamples, const Surface &sur,
                                      bool useVert, int minPerTriangle);
  static void sampleTriangleGrid(Array<Point3D> *pts, int n,
                                 const Point3D p[3]);
};

#endif
