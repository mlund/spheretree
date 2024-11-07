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

#include "VFGapCross.h"
#include "../Geometry/TriTri.h"
#include "../Geometry/Line2D.h"
#include "../Surface/Internal.h"

//  constructor
VFGapCross::VFGapCross() { sur = NULL; }

VFGapCross::VFGapCross(const Surface &sur) { setSurface(sur); }

//  surface
void VFGapCross::setSurface(const Surface &sur) { this->sur = &sur; }

//  fix
void VFGapCross::fixVoronoi(Voronoi3D *vor) const {
  CHECK_DEBUG(vor != NULL, "NULL voronoi");
  CHECK_DEBUG(sur != NULL, "No Surface : use setSurface\n");

  fixGapCrossers(vor, *sur);
}

void VFGapCross::fixGapCrossers(Voronoi3D *vor, const Surface &sur) {
  ClusterInfo clusterInf;
  getClusterInfo(&clusterInf, sur);

  int startI = 0;
  while (true) {
    Array<Surface::Point> extraPoints;
    findGapPoints(&extraPoints, *vor, sur, clusterInf, startI);
    OUTPUTINFO("%d Extra Points Needed\n", extraPoints.getSize());

    if (extraPoints.getSize() != 0) {
      int oldSize = vor->formingPoints.getSize();
      vor->randomInserts(extraPoints);
      startI = oldSize;

      int newForming = vor->formingPoints.getSize() - oldSize;
      OUTPUTINFO("%d new forming points\n", newForming);
      if (newForming <= 0)
        break;
    } else
      break;
  }
}

void VFGapCross::intersectFaceCluster(Array<int> *tris, const VoronoiFace &face,
                                      const Array<int> &cluster,
                                      const Surface &sur,
                                      const Voronoi3D &vor) {
  tris->setSize(0);

  //  test each cluster triangle for intersection
  int numTris = cluster.getSize();
  for (int i = 0; i < numTris; i++) {
    //  make triangle
    const Surface::Triangle *tri = &sur.triangles.index(cluster.index(i));
    Point3D clusterTri[3];
    clusterTri[0] = sur.vertices.index(tri->v[0]).p;
    clusterTri[1] = sur.vertices.index(tri->v[1]).p;
    clusterTri[2] = sur.vertices.index(tri->v[2]).p;

    //  test against each triangle of face
    Point3D p0 = vor.vertices.index(face.vertices.index(0)).s.c;
    int numVert = face.vertices.getSize();
    int j;
    for (j = 0; j < numVert; j++) {
      //  make triangle
      Point3D faceTri[3];
      faceTri[0] = p0;
      faceTri[1] = vor.vertices.index(face.vertices.index(j)).s.c;
      faceTri[2] = vor.vertices.index(face.vertices.index((j + 1) % 3)).s.c;

      if (trianglesIntersect(faceTri, clusterTri))
        break;
    }

    if (j != numVert)
      tris->addItem() = i;
  }
}

bool VFGapCross::inTriangle(const Point3D &p, const Point3D tri[3]) {
  //  plane test
  Plane pl;
  pl.assign(tri[0], tri[1], tri[2]);
  if (fabs(pl.dist(p)) > 1E-2f)
    return false;

  //  normal of triangle
  Vector3D v1, v2, vN;
  v1.difference(tri[1], tri[0]);
  v2.difference(tri[2], tri[0]);
  vN.cross(v1, v2);

  //  transform into 2D
  Transform3D tr;
  tr.alignToZ(vN, tri[0]);
  Point2D q, v[3];
  tr.transformTo2D(&q, p);
  tr.transformTo2D(&v[0], tri[0]);
  tr.transformTo2D(&v[1], tri[1]);
  tr.transformTo2D(&v[2], tri[2]);

  for (int i = 0; i < 3; i++) {
    Line2D l;
    l.assign(v[i], v[(i + 1) % 3]);
    if (l.distance(q) < 0)
      return false;
  }
  return true;
}

bool VFGapCross::inCluster(Point3D &p, const Array<int> &cluster,
                           const Surface &sur) {
  int numTri = cluster.getSize();
  for (int i = 0; i < numTri; i++) {
    //  make triangle
    const Surface::Triangle *tri = &sur.triangles.index(cluster.index(i));
    Point3D clusterTri[3];
    clusterTri[0] = sur.vertices.index(tri->v[0]).p;
    clusterTri[1] = sur.vertices.index(tri->v[1]).p;
    clusterTri[2] = sur.vertices.index(tri->v[2]).p;

    if (inTriangle(p, clusterTri))
      return true;
  }
  return false;
}

bool VFGapCross::inClusterOrNeighbour(Point3D &p, const ClusterInfo &clusterInf,
                                      const Surface &sur, int clusterNum) {
  if (inCluster(p, clusterInf.clusters.index(clusterNum), sur))
    return true;
  else {
    //  check against neighbours
    const Array<int> *neigh = &clusterInf.neighbours.index(clusterNum);
    int numNeigh = neigh->getSize();
    for (int i = 0; i < numNeigh; i++)
      if (inCluster(p, clusterInf.clusters.index(neigh->index(i)), sur))
        return true;
  }

  return false;
}

void VFGapCross::addSnaps(Array<Surface::Point> *pts, const Point3D &pProj,
                          const Vector3D &clusterNorm, const Array<int> &intTri,
                          const Surface &sur) {
  int numTri = intTri.getSize();

  // float bestD = FLT_MAX;
  // Surface::Point bestIP;
  // bestIP.n = clusterNorm;

  for (int i = 0; i < numTri; i++) {
    //  make the triangle
    int triNum = intTri.index(i);
    const Surface::Triangle *tri = &sur.triangles.index(triNum);
    Point3D p[3];
    p[0] = sur.vertices.index(tri->v[0]).p;
    p[1] = sur.vertices.index(tri->v[1]).p;
    p[2] = sur.vertices.index(tri->v[2]).p;

    //  find the closest point
    ClosestPointInfo inf;
    float d = distanceToTriangle(&inf, pProj, p);

    //  add sample point
    Surface::Point *ins = &pts->addItem();
    ins->p = inf.pClose;
    ins->n = clusterNorm;
    // if (d < bestD){
    //   bestD = d;
    //   bestIP.p = inf.pClose;
    //   }
  }

  // pts->addItem() = bestIP;
}

void VFGapCross::findGapPoints(Array<Surface::Point> *pts, const Voronoi3D &vor,
                               const Surface &sur,
                               const ClusterInfo &clusterInf, int startIndex) {
  pts->setSize(0);
  if (startIndex < 8)
    startIndex = 8;

  int numClusters = clusterInf.clusters.getSize();
  int numForm = vor.formingPoints.getSize();
  for (int i = startIndex; i < numForm; i++) {
    //  get forming point
    Point3D pI = vor.formingPoints.index(i).p;

    //  get neighbouring forming points
    Array<int> cells;
    findNeighbourCells(&cells, i, vor);
    int numCells = cells.getSize();

    //  process faces between neighbouring cells (excl duplicates)
    for (int j = 0; j < numCells; j++) {
      int neigh = cells.index(j);
      if (neigh > i || neigh < startIndex && neigh > 8) {
        //  get forming point
        Point3D pJ = vor.formingPoints.index(neigh).p;

        //  generate the face
        VoronoiFace face;
        if (getFace(&face, i, neigh, vor)) {
          //  test against each cluster
          for (int k = 0; k < numClusters; k++) {
            //  generate list of triangles that the face intersects
            Array<int> intersectTris;
            intersectFaceCluster(&intersectTris, face,
                                 clusterInf.clusters.index(k), sur, vor);

            //  face intersects the cluster
            if (intersectTris.getSize()) {
              bool pIcont = inClusterOrNeighbour(pI, clusterInf, sur, k);
              bool pJcont = inClusterOrNeighbour(pJ, clusterInf, sur, k);

              if (!pIcont || !pJcont) {
                //  get clusters first triangle
                const Surface::Triangle *tri =
                    &sur.triangles.index(clusterInf.clusters.index(k).index(0));

                //  get cluster plane
                Point3D p0, p1, p2;
                p0 = sur.vertices.index(tri->v[0]).p;
                p1 = sur.vertices.index(tri->v[1]).p;
                p2 = sur.vertices.index(tri->v[2]).p;

                Plane clusterPl;
                clusterPl.assign(p0, p1, p2);
                Vector3D clusterNorm;
                clusterPl.getNormal(&clusterNorm);

                //  project pI & pJ onto the cluster's plane
                Point3D pIproj, pJproj;
                bool pI1cont = false, pJ1cont = false;

                if (!pIcont) {
                  clusterPl.projectOnto(&pIproj, pI, clusterNorm);
                  pI1cont =
                      inCluster(pIproj, clusterInf.clusters.index(k), sur);
                }
                if (!pJcont) {
                  clusterPl.projectOnto(&pJproj, pJ, clusterNorm);
                  pJ1cont =
                      inCluster(pJproj, clusterInf.clusters.index(k), sur);
                }

                //  decide which point to project
                if (!pIcont && pI1cont) {
                  //  use projection of Pi
                  Surface::Point *ins = &pts->addItem();
                  ins->p = pIproj;
                  ins->n = clusterNorm;
                } else if (!pJcont && pJ1cont) {
                  //  use projection of Pj
                  Surface::Point *ins = &pts->addItem();
                  ins->p = pJproj;
                  ins->n = clusterNorm;
                } else if (!pIcont) {
                  //  snap pIproj to each triangle in cluster
                  addSnaps(pts, pIproj, clusterNorm, intersectTris, sur);
                } else /*if (!pJcont)*/ {
                  //  snap pJproj to each triangle in cluster
                  addSnaps(pts, pJproj, clusterNorm, intersectTris, sur);
                }
              }
            }
          }
        }
      }
    }
  }
}
