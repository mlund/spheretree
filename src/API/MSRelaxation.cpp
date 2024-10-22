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

#include "MSRelaxation.h"
#include "DomainSampler.h"
#include "../Geometry/Transform3D.h"
#include "../Base/Defs.h"

//  constructor
MSRelaxation::MSRelaxation() {
  sur = NULL;

  useVert = true;
  maxIters = 50;
  edgeFrac = 0.5f;
}

MSRelaxation::MSRelaxation(const Surface &sur) {
  setSurface(sur);

  useVert = true;
  edgeFrac = 0.1f;
  maxIters = 50;
}

//  setup
void MSRelaxation::setSurface(const Surface &sur) { setSurface(sur); }

//  sample
void MSRelaxation::generateSamples(Array<Surface::Point> *pts,
                                   int numSamples) const {
  CHECK_DEBUG(sur != NULL, "Surface required : use setup");
  CHECK_DEBUG0(numSamples > 0);
  generateSamples(pts, numSamples, *sur, useVert, edgeFrac);
}

//  internals
void MSRelaxation::generateSamples(Array<Surface::Point> *pts, int numSamples,
                                   const Surface &sur, bool useVert,
                                   float edgeFrac, int maxIter) {
  pts->setSize(0);

  //  deal with vertices
  if (useVert)
    pts->append(sur.vertices);

  //  deal with edges
  int edgeNum = edgeFrac * numSamples;
  if (edgeNum > 0)
    sampleEdges(pts, edgeNum, sur);

  //  sample faces
  Array<int> counts;
  attributeCountsToFaces(&counts, sur, numSamples);

  int numTri = counts.getSize();
  for (int i = 0; i < numTri; i++) {
    int num = counts.index(i);

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
      points.resize(num);
      for (int j = 0; j < num; j++)
        randomTriangleSample(&points.index(j), p[0], p[1], p[2]);

      //  do relaxation
      relaxTriangle(&points, p, maxIter);

      //  store points
      for (int j = 0; j < num; j++) {
        Surface::Point *p = &pts->addItem();
        p->p = points.index(j);
        p->n = n;
      }
    } else {
      //  should we use incenter ??
      Surface::Point *s = &pts->addItem();
      s->n = n;
      s->p.x = (p[0].x + p[1].x + p[2].x) / 3.0f;
      s->p.y = (p[0].y + p[1].y + p[2].y) / 3.0f;
      s->p.z = (p[0].z + p[1].z + p[2].z) / 3.0f;
    }
  }
}

/*
Subject 6.04: What are barycentric coordinates?

        Let p1, p2, p3 be the three vertices (corners) of a closed
    triangle T (in 2D or 3D or any dimension).  Let t1, t2, t3 be real
    numbers that sum to 1, with each between 0 and 1:  t1 + t2 + t3 = 1,
    0 <= ti <= 1.  Then the point p = t1*p1 + t2*p2 + t3*p3 lies in
    the plane of T and is inside T.  The numbers (t1,t2,t3) are called the
    barycentric coordinates of p with respect to T. They uniquely identify
    p.  They can be viewed as masses placed at the vertices whose
    center of gravity is p.
        For example, let p1=(0,0), p2=(1,0), p3=(3,2).  The
    barycentric coordinates (1/2,0,1/2) specify the point
    p = (0,0)/2 + 0*(1,0) + (3,2)/2 = (3/2,1), the midpoint of the p1-p3
    edge.
        If p is joined to the three vertices, T is partitioned
    into three triangles.  Call them T1, T2, T3, with Ti not incident
    to pi.  The areas of these triangles Ti are proportional to the
    barycentric coordinates ti of p.

    Reference:
    [Coxeter, Intro. to Geometry, p.217].

Subject 6.05: How can I generate a random point inside a triangle?

        Use barycentric coordinates (see item 53) .  Let A, B, C be the
    three vertices of your triangle.  Any point P inside can be expressed
    uniquely as P = aA + bB + cC, where a+b+c=1 and a,b,c are each >= 0.
    Knowing a and b permits you to calculate c=1-a-b.  So if you can
    generate two random numbers a and b, each in [0,1], such that
    their sum <=1, you've got a random point in your triangle.
        Generate random a and b independently and uniformly in [0,1]
    (just divide the standard C rand() by its max value to get such a
    random number.) If a+b>1, replace a by 1-a, b by 1-b.  Let c=1-a-b.
    Then aA + bB + cC is uniformly distributed in triangle ABC: the reflection
    step a=1-a; b=1-b gives a point (a,b) uniformly distributed in the triangle
    (0,0)(1,0)(0,1), which is then mapped affinely to ABC.
    Now you have barycentric coordinates a,b,c.  Compute your point
    P = aA + bB + cC.

    Reference: [Gems I], Turk, pp. 24-28.
*/
void MSRelaxation::randomTriangleSample(Point3D *p, const Point3D &p0,
                                        const Point3D &p1, const Point3D &p2) {
  //  generate random variables such that a+b+c == 1, where a, b, c >= 0
  float a = rand() / (float)RAND_MAX;
  float b = rand() / (float)RAND_MAX;
  if (a + b > 1) {
    a = 1.0f - a;
    b = 1.0f - b;
  }
  float c = 1.0f - a - b;

  //  compute point P = aA + bB + cC
  p->x = a * p0.x + b * p1.x + c * p2.x;
  p->y = a * p0.y + b * p1.y + c * p2.y;
  p->z = a * p0.z + b * p1.z + c * p2.z;
}

/*
    Relaxation algorithm to improve spacing of points
*/
float MSRelaxation::getWeight(const Vector2D &v, float r) {
  //  prevent massive weighting
  float mag = v.mag();
  if (mag > r)
    return 0;
  else {
    return (r / mag) / 100;
  }
}

//  force applied leaving from p1 to p
void MSRelaxation::accumulateForce(Vector2D *vDest, const Point2D &p,
                                   const Point2D &p1, float radius) {
  //  vector to point
  Vector2D v;
  v.difference(p, p1);

  //  weighting function
  float weight = getWeight(v, radius);
  if (finite(weight)) {
    //  direction only
    v.norm();

    //  accumulate translation
    v.scale(weight);
    vDest->add(v);
  }
}

//  force applied leaving edge inward towards p
void MSRelaxation::accumulateForce(Vector2D *vDest, const Point2D &p,
                                   const LineSeg &ls, const Line2D &l,
                                   float radius) {
  //  project onto edge
  Point2D pProj;
  l.project(&pProj, p);

  //  vector to point
  Vector2D v;
  v.difference(p, pProj);

  //  weighting function
  float weight = 2.5 * getWeight(v, radius);
  if (finite(weight)) {
    //  inward vector
    Vector2D vL;
    vL.x = ls.p0.y - ls.p1.y;
    vL.y = ls.p1.x - ls.p0.x;
    vL.norm();

    //  accumulate translation
    vL.scale(weight);
    vDest->add(vL);
  }
}

void MSRelaxation::relaxTriangle(Array<Point3D> *srcPts, const Point3D p[3],
                                 int maxIter) {
  int numPts = srcPts->getSize();

  //  compute the repulsion radius for the triangle (as in Turk)
  float a = MSBase::areaTriangle(p[0], p[1], p[2]);
  float r = 2 * sqrt(a / numPts);

  //  transform triangle into 2D
  Vector3D v1, v2, vC;
  v1.difference(p[1], p[0]);
  v2.difference(p[2], p[0]);
  vC.cross(v1, v2);

  Transform3D tr;
  tr.alignToZ(vC, p[0]);

  Point2D q[3];
  for (int i = 0; i < 3; i++)
    tr.transformTo2D(&q[i], p[i]);

  Line2D l[3];
  LineSeg ls[3];
  for (int i = 0; i < 3; i++) {
    ls[i].p0 = q[i];
    ls[i].p1 = q[(i + 1) % 3];
    ls[i].getLine(&l[i]);
  }

  // transform points to 2D
  Array<Point2D> pts;
  pts.allocate(numPts);
  for (int i = 0; i < numPts; i++)
    tr.transformTo2D(&pts.index(i), srcPts->index(i));

  //  translations to apply to points
  Array<Vector2D> vOff;
  vOff.allocate(numPts);
  for (int iter = 0; iter < maxIter; iter++) {
    //  compute translation
    for (int pt = 0; pt < numPts; pt++) {
      //  initialise translation
      Vector2D vTmp = Vector2D::ZERO;
      Point2D p = pts.index(pt);

      // compute forces from edges
      for (int ed = 0; ed < 3; ed++)
        accumulateForce(&vTmp, p, ls[ed], l[ed], r /*weight*10*/);

      //  compute forces from other points
      for (int pt2 = 0; pt2 < numPts; pt2++)
        if (pt != pt2)
          accumulateForce(&vTmp, p, pts.index(pt2), r /*weight*/);

      //  store translation
      vOff.index(pt) = vTmp;
    }

    //  apply translations
    for (int pt = 0; pt < numPts; pt++) {
      Vector2D v;
      v = vOff.index(pt);

      //  transform point
      Point2D p0, p1;
      p0 = pts.index(pt);
      v.add(&p1, p0);

      if (inTriangle(p1, l)) {
        //  point is fine - use it as is
        pts.index(pt) = p1;
      } else {
        //  intersect with edges, use forward edge
        Line2D lTest;
        lTest.assign(p0, p1);

        Point2D maxP = p0;
        REAL maxD = REAL_MAX;
        for (int i = 0; i < 3; i++) {
          //  intersection with edge (will exist)
          Point2D pI;
          l[i].intersect(&pI, lTest);
          Vector2D vI;
          vI.difference(pI, p0);

          if (vI.dot(v) > 0 && ls[i].isInSegment(pI)) {
            maxP = pI;
            break;
          }
        }

        // p0 = maxP;

        //  go half way to boundary
        p0.x = (maxP.x + p0.x) / 2.0;
        p0.y = (maxP.y + p0.y) / 2.0;
        pts.index(pt) = p0;
      }
    }
  }

  //  transform points back to 3D
  Transform3D trI;
  trI.invert(tr);
  for (int i = 0; i < numPts; i++)
    trI.transformFrom2D(&srcPts->index(i), pts.index(i));
}

//  internals for edges
void MSRelaxation::sampleEdges(Array<Surface::Point> *pts, int numSamples,
                               const Surface &sur) {
  int numPts = sur.vertices.getSize();
  int numTriangles = sur.triangles.getSize();

  //  get edges
  Array<Edge> edges;

  //  create edges from triangles
  for (int i = 0; i < numTriangles; i++) {
    const Surface::Triangle *tr = &sur.triangles.index(i);

    for (int j = 0; j < 3; j++) {
      int v1 = tr->v[j];
      int v2 = tr->v[(j + 1) % 3];
      if (v1 < v2) {
        //  create new edge
        Edge *e = &edges.addItem();
        e->v1 = v1;
        e->v2 = v2;

        //  get normal for edge
        sur.getEdgeNormal(&e->nE, i, j);
      }
    }
  }

  //  generate edge lengths
  int numEdges = edges.getSize();
  SamplerInfo *infos = new SamplerInfo[numEdges];
  for (int i = 0; i < numEdges; i++) {
    const Edge *e = &edges.index(i);
    Point3D p0 = sur.vertices.index(e->v1).p;
    Point3D p1 = sur.vertices.index(e->v2).p;
    infos[i].val = p0.distance(p1);
  }

  // generate edge point counts
  Array<int> counts;
  sampleDomain(&counts, infos, numEdges, numSamples);
  delete[] infos;

  //  sample edges
  for (int i = 0; i < numEdges; i++) {
    int n = counts.index(i);
    if (n != 0) {
      //  get edge info
      const Edge *e = &edges.index(i);
      Point3D p0 = sur.vertices.index(e->v1).p;
      Point3D p1 = sur.vertices.index(e->v2).p;

      //  first point on edge
      Surface::Point *ip = &pts->addItem();
      ip->p = p0;
      ip->n = sur.vertices.index(e->v1).n;

      //  other points
      for (int j = 1; j < n; j++) {
        //  fraction of line segment
        float frac = j / (float)n;

        //  linear interp
        Surface::Point *ip = &pts->addItem();
        ip->p.x = p0.x * frac + p1.x * (1 - frac);
        ip->p.y = p0.y * frac + p1.y * (1 - frac);
        ip->p.z = p0.z * frac + p1.z * (1 - frac);
        ip->n = e->nE;
      }
    }
  }
}
