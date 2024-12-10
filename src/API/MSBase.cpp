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

#include "MSBase.h"
#include "DomainSampler.h"
#include "../Base/Defs.h"

//  constructor
MSBase::MSBase() { sur = NULL; }

// setup
void MSBase::setSurface(const Surface &sur) { this->sur = &sur; }

/*
    Area of Triangle (half base * perpendicular height) p0->p1 is the base
*/
float MSBase::areaTriangle(const Point3D &p0, const Point3D &p1,
                           const Point3D &p2) {
  //  size of sides
  Vector3D v1, v2;
  v1.difference(p1, p0);
  v2.difference(p2, p0);
  float A = v2.mag();
  float B = v1.mag();

  //  prependicular height
  v1.norm();
  v2.norm();
  float d = v1.dot(v2);
  if (d < -1)
    d = -1;
  else if (d > 1)
    d = 1;
  float theta = acos(d);
  float H = A * sin(theta);

  float AREA = B * H / 2.0f;
  return AREA;
  /*
    //  vectors for vertices
    Vector3D V0(p0), V1(p1), V2(p2);
    Vector3D V20, V01, V12;
    V20.cross(V2, V0);
    V01.cross(V0, V1);
    V12.cross(V1, V2);

    Vector3D Va, Vb;
    Va.difference(p1, p0);
    Vb.difference(p2, p0);

    Vector3D N;
    N.cross(Va, Vb);
    N.norm();

    V20.add(V01);
    V20.add(V12);

    float AREA1 = fabs(N.dot(V20)) * 0.5;
    OUTPUTINFO("Area1 = %f\n", AREA1);
    return AREA1;*/
}

void MSBase::attributeCountsToFaces(Array<int> *counts, const Surface &sur,
                                    int numSam) {
  /*
    Sample triangulated surface,
    probability of placing a point in a triangle
    is proportional to the area of the triangle
  */

  //  setup areas
  int numTris = sur.triangles.getSize();
  SamplerInfo *infos = new SamplerInfo[numTris];
  try {
    for (int i = 0; i < numTris; i++) {
      const Surface::Triangle *tr = &sur.triangles.index(i);
      float area = areaTriangle(sur.vertices.index(tr->v[1]).p,
                                sur.vertices.index(tr->v[2]).p,
                                sur.vertices.index(tr->v[0]).p);
      infos[i].val = area;
    }

    sampleDomain(counts, infos, numTris, numSam);
  } catch (...) {
    delete[] infos;
    throw;
  }

  delete[] infos;
}

bool MSBase::inTriangle(const Point2D &p, const Line2D l[3]) {
  for (int i = 0; i < 3; i++) {
    if (l[i].distance(p) < -EPSILON_LARGE)
      return FALSE;
  }
  return TRUE;
}
