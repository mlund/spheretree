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

#include "MSGrid.h"
#include "../Geometry/Transform3D.h"

MSGrid::MSGrid() {
  useVert = true;
  minPerTriangle = 0;
}

void MSGrid::generateSamples(Array<Surface::Point> *pts, int numSamples) const {
  CHECK_DEBUG(sur != NULL, "need surface");
  generateSamplesInternal(pts, numSamples, *sur, useVert, minPerTriangle);
}

void MSGrid::generateSamples(Array<Surface::Point> *pts, int numSamples,
                             const Surface &sur, bool useVert,
                             int minPerTriangle) {
  generateSamplesInternal(pts, numSamples, sur, useVert, minPerTriangle);
}

void MSGrid::generateSamplesInternal(Array<Surface::Point> *pts, int numSamples,
                                     const Surface &sur, bool useVert,
                                     int minPerTriangle) {
  pts->setSize(0);

  //  sample vertices
  if (useVert)
    pts->append(sur.vertices);

  //  sample faces
  Array<int> counts;
  attributeCountsToFaces(&counts, sur, numSamples);
  int numTri = counts.getSize();
  for (int i = 0; i < numTri; i++) {
    int num = counts.index(i);
    if (num < minPerTriangle)
      num = minPerTriangle;
    if (num == 0)
      continue;

    //  get triangle
    const Surface::Triangle *tr = &sur.triangles.index(i);
    Point3D p[3];
    p[0] = sur.vertices.index(tr->v[0]).p;
    p[1] = sur.vertices.index(tr->v[1]).p;
    p[2] = sur.vertices.index(tr->v[2]).p;

    //  get normal
    Vector3D n;
    sur.getTriangleNormal(&n, i);

    if (num > 1) {
      //  generate random samples on the triangle
      Array<Point3D> points;
      sampleTriangleGrid(&points, num, p);

      //  store points
      int num = points.getSize();
      for (int j = 0; j < num; j++) {
        Surface::Point *p = &pts->addItem();
        p->p = points.index(j);
        p->n = n;
      }
    } else /*if (num == 1)*/ {
      //  should we use incenter ??
      Surface::Point *s = &pts->addItem();
      s->n = n;
      s->p.x = (p[0].x + p[1].x + p[2].x) / 3.0f;
      s->p.y = (p[0].y + p[1].y + p[2].y) / 3.0f;
      s->p.z = (p[0].z + p[1].z + p[2].z) / 3.0f;
    }
  }
}

void MSGrid::sampleTriangleGrid(Array<Point3D> *pts, int n,
                                const Point3D p[3]) {
  //  transform triangle into 2D
  Vector3D v1, v2, vC;
  v1.difference(p[1], p[0]);
  v2.difference(p[2], p[0]);
  vC.cross(v1, v2);

  Transform3D tr, trI;
  tr.alignToZ(vC, p[0]);
  trI.invert(tr);

  //  make triangle
  Point2D q[3];
  for (int i = 0; i < 3; i++)
    tr.transformTo2D(&q[i], p[i]);

  Line2D l[3];
  for (int i = 0; i < 3; i++) {
    Point2D p0 = q[i];
    Point2D p1 = q[(i + 1) % 3];
    l[i].assign(p0, p1);
  }

  //  get bounding box
  Point2D pMin = Point2D::MAX;
  Point2D pMax = Point2D::MIN;
  for (int i = 0; i < 3; i++) {
    pMin.storeMin(q[i]);
    pMax.storeMax(q[i]);
  }

  // work out cell size
  float tArea = areaTriangle(p[0], p[1], p[2]);
  float rArea = (pMax.x - pMin.x) * (pMax.y - pMin.y);
  float r = (pMax.x - pMin.x) / (pMax.y - pMin.y);
  int numCells = rArea / tArea * n;
  int cellsX = ceil(sqrt(numCells * r));
  int cellsY = ceil(sqrt(numCells / r));

  //  step out grid
  float dX = (pMax.x - pMin.x) / cellsX * 0.99;
  float dY = (pMax.y - pMin.y) / cellsY * 0.99;
  for (int i = 0; i <= cellsY + 1; i++)
    for (int j = 0; j <= cellsX + 1; j++) {
      Point2D q;
      q.x = pMin.x + dX * j;
      q.y = pMin.y + dY * i;

      if (inTriangle(q, l)) {
        Point3D *p = &pts->addItem();
        trI.transformFrom2D(p, q);
      }
    }
}
