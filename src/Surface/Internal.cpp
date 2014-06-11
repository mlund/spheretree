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

#include "Internal.h"
#include "../Geometry/Transform3D.h"
#include "../Geometry/LineSeg.h"
#include "../Geometry/Plane.h"
#include "../Geometry/RayBox.h"
#include "../Geometry/Ray.h"
#include "InternalGem.h"

bool project3D(Point3D *p, const Point3D &q, const Point3D &p0, const Point3D &p1){
  //  make vector to project onto
  Vector3D v0;
  v0.difference(p1, p0);
  float len0 = v0.mag();

  //  normalise the vector
  v0.scale(1.0f/len0);

  //  make vector to project
  Vector3D v1;
  v1.difference(q, p0);

  //  do projection
  float t = v1.dot(v0);

  //  test in segment
  if (t < 0 || t > len0){
    return false;
    }
  else{
    //  work out point
    v0.add(p, p0, t);
    return true;
    }
}

/*
    finding the closest point on a triangle
*/
double distanceToTriangle(ClosestPointInfo *inf, const Point3D &pTest, const Point3D P[3]){
  //  normal to plane
  Vector3D v1, v2, vC;
  v1.difference(P[1], P[0]);
  v2.difference(P[2], P[0]);
  vC.cross(v1, v2);

  //  transforms to get into 2D
  double minD = REAL_MAX;

  //  test against triangle
  float t;
  if (intersectTriangle(pTest, vC, &t, P[2], P[1], P[0])){
    inf->type = FACE;
    vC.add(&inf->pClose, pTest, t);
    minD = inf->pClose.distanceSQR(pTest);
    }
  else{
    //  project p1 onto each edge
    for (int i = 0; i < 3; i++){
      Point3D pt;
      if (project3D(&pt, pTest, P[i], P[(i+1)%3])){
        double d = pt.distanceSQR(pTest);
        if (d < minD){
          minD = d;
          inf->num = i;
          inf->type = EDGE;
          inf->pClose = pt;
          }
        }
      }

    //  test against each vertex
    for (i = 0; i < 3; i++){
      double d = P[i].distanceSQR(pTest);
      if (d < minD){
        inf->num = i;
        inf->type = VERTEX;
        inf->pClose = P[i];
        minD = d;
        }
      }
    }

  return sqrt(minD);
}

double distanceToTriangleSPECIAL(ClosestPointInfo *inf, double minDS, const Point3D &pTest, const Vector3D &nTri, const Point3D P[3], const int pI[3]){
  //  transforms to get into 2D
  double minD = minDS;

  //  test against triangle
  float t;
  if (intersectTriangle(pTest, nTri, &t, P[2], P[1], P[0])){
    float d = t*t;               //  distance squared
    if (d < minD){
      inf->type = FACE;
      nTri.add(&inf->pClose, pTest, t);
      minD = d;
      }
    }
  else{
    //  project p1 onto each edge
    for (int i = 0; i < 3; i++){
      int i1 = (i+1)%3;

      if (pI[i] < pI[i1]){            //    only need to consider each point once
        Point3D pt;
        if (project3D(&pt, pTest, P[i], P[i1])){
          double d = pt.distanceSQR(pTest);
          if (d < minD){
            minD = d;
            inf->num = i;
            inf->type = EDGE;
            inf->pClose = pt;
            }
          }
        }
      }

    //  vertices are checked outside
    }

  return minD;
}

void getClosestPointINTERNAL(ClosestPointInfo *inf, double *minD, const Point3D &pTest, const Surface &sur, double stopBelowSqr, const Array<int> *tris, Array<bool> *triangleFlags, Array<bool> *vertexFlags){
  int numTris = tris->getSize();
  for (int i = 0; i < numTris; i++){
    int tI = tris->index(i);
    if (triangleFlags->index(tI))
      continue;
    triangleFlags->index(tI) = true;

    //  get points
    Point3D pts[3];
    const Surface::Triangle *tri = &sur.triangles.index(tI);
    for (int j = 0; j < 3; j++){
      int vI = tri->v[j];
      pts[j] = sur.vertices.index(vI).p;

      //  check vertex
      if (!vertexFlags->index(vI)){
        vertexFlags->index(vI) = true;

        double d = pts[j].distanceSQR(pTest);
        if (d < *minD){
          *minD = d;
          inf->type = VERTEX;
          inf->triangle = tI;
          inf->num = j;
          inf->pClose = pts[j];
          }
        }
      }

    //  get distance
    double d = distanceToTriangleSPECIAL(inf, *minD, pTest, tri->n, pts, tri->v);

    //  store closest
    if (d < *minD){
      inf->triangle = tI;
      *minD = d;
      }

    //  opt for dodec test
    if (*minD < stopBelowSqr)
      return;
    }
}

double getClosestPointOrdered(ClosestPointInfo *inf, const Point3D &pTest, const Surface &s, const SpacialHash &faceHash, float stopBelow){
  //  get the dimensions and size of the grid
  int nX = faceHash.getDimX();
  int nY = faceHash.getDimY();
  int nZ = faceHash.getDimZ();
  float cellSize = faceHash.getSize();

  //  flags for used triangles
  Array<bool> flags(s.triangles.getSize());
  flags.clear();

  //  flags for whether point has been tested
  Array<bool> vertexFlags(s.vertices.getSize());
  vertexFlags.clear();

  //  get cells from the hash
  SpacialHash::CellRec *recs;
  int numCells = faceHash.getCells(&recs, pTest);

  //  initialise the closest point algorithm
  inf->num = 0;
  inf->triangle = 0;
  inf->type = DIST_TYPE_INVALID;
  double minD = REAL_MAX;
  double stopBelowSqr = -1;
  if (stopBelow > 0)
    stopBelowSqr = stopBelow*stopBelow;

  for (int i = 0; i < numCells; i++){
    SpacialHash::CellRec rec = recs[i];
    if (rec.d <= minD){
      Array<int> *cell = faceHash.getCell(rec.i, rec.j, rec.k);
      getClosestPointINTERNAL(inf, &minD, pTest, s, stopBelowSqr, cell, &flags, &vertexFlags);
      if (minD < stopBelowSqr){
        delete recs;
        return sqrt(minD);
        }
      }
    }

  delete recs;
  return sqrt(minD);
}

//  NOT WORKING YET
double getClosestPoint(ClosestPointInfo *inf, const Point3D &pTest, const Surface &s, const SpacialHash &faceHash, float stopBelow){
  //  get the dimensions and size of the grid
  int nX = faceHash.getDimX();
  int nY = faceHash.getDimY();
  int nZ = faceHash.getDimZ();
  float cellSize = faceHash.getSize();

  //  work out the MAX ring number  (doesn't really need to be small)
  int maxRing = nX;
  if (nY > maxRing)
    maxRing = nY;
  if (nZ > maxRing)
    maxRing = nZ;

  //  get the cell 
  int Cx, Cy, Cz;
  faceHash.getBoundedIndices(&Cx, &Cy, &Cz, pTest);

  //  flags for used triangles
  Array<bool> flags(s.triangles.getSize());
  flags.clear();

  //  flags for whether point has been tested
  Array<bool> vertexFlags(s.vertices.getSize());
  vertexFlags.clear();

  //  initialise the closest point algorithm
  inf->num = 0;
  inf->triangle = 0;
  inf->type = DIST_TYPE_INVALID;
  double minD = REAL_MAX;
  double stopBelowSqr = -1;
  if (stopBelow > 0)
    stopBelowSqr = stopBelow*stopBelow;

  //  process the first cell
  Array<int> *cell = faceHash.getCell(Cx, Cy, Cz);
  if (cell->getSize())
    getClosestPointINTERNAL(inf, &minD, pTest, s, stopBelowSqr, cell, &flags, &vertexFlags);

#define DOCELL() {   \
          Array<int> *cell = faceHash.getCell(x, y, z);  \
          if (cell->getSize() && faceHash.getDistanceSQR(x, y, z, pTest) < minD){ \
            getClosestPointINTERNAL(inf, &minD, pTest, s, stopBelowSqr, cell, &flags, &vertexFlags); \
            if (minD < stopBelowSqr) \
              return sqrt(minD); \
            } } \

  //  process rings
  int x, y, z;
  for (int ring = 1; ring <= maxRing; ring++){
    //  check for terminate of ring
    double d = (ring-1)*cellSize;
    if (d*d > minD)
      return sqrt(minD);  //  done

    //  get clamped bounds for the ring
    int minX = Cx - ring;
    if (minX < 0)
      minX = 0;

    int minY = Cy - ring;
    if (minY < 0)
      minY = 0;

    int minZ = Cz - ring;
    if (minZ < 0)
      minZ = 0;

    int maxX = Cx + ring;
    if (maxX >= nX)
      maxX = nX-1;

    int maxY = Cy + ring;
    if (maxY >= nY)
      maxY = nY-1;

    int maxZ = Cz + ring;
    if (maxZ >= nZ)
      maxZ = nZ-1;

    //  top
    y = Cy - ring;
    if (y >= 0){
      for (x = minX; x <= maxX; x++)
        for (z = minZ; z <= maxZ; z++)
          DOCELL();
      }

    //  bottom
    y = Cy + ring;
    if (y < nY){
      for (x = minX; x <= maxX; x++)
        for (z = minZ; z <= maxZ; z++)
          DOCELL();
      }

    //  work out the starting points
    int localMinY = Cy - ring + 1;    //  top and bottom already done
    if (localMinY < minY)
      localMinY = minY;

    int localMaxY = Cy + ring - 1;
    if (localMaxY > maxY)
      localMaxY = maxY;

    int localMinZ = Cz - ring;
    if (localMinZ < minZ)
      localMinZ = minZ;

    int localMaxZ = Cz + ring;
    if (localMaxZ > maxZ)
      localMaxZ = maxZ;

    //  left
    x = Cx - ring;
    if (x >= 0){
      for (y = localMinY; y <= localMaxY; y++)
        for (z = localMinZ; z <= localMaxZ; z++)
          DOCELL();
      }

    //  right
    x = Cx + ring;
    if (x < nX){
      for (y = localMinY; y <= localMaxY; y++)
        for (z = localMinZ; z <= localMaxZ; z++)
          DOCELL();
      }

    //  work out the starting points
    int localMinX = Cx - ring + 1;    //  left and right already done
    if (localMinX < minX)
      localMinX = minX;

    int localMaxX = Cx + ring - 1;
    if (localMaxX > maxX)
      localMaxX = maxX;

    //  front
    z = Cz - ring;
    if (z > 0){
      for (x = localMinX; x <= localMaxX; x++)
        for (y = localMinY; y <= localMaxY; y++)
          DOCELL();
      }

    //  back
    z = Cz + ring;
    if (z < nZ){
      for (x = localMinX; x <= localMaxX; x++)
        for (y = localMinY; y <= localMaxY; y++)
          DOCELL();
      }
    }

  return sqrt(minD);
}

double getClosestPointOLD(ClosestPointInfo *inf, const Point3D &pTest, const Surface &s, const SpacialHash &faceHash, float stopBelow){
  //  get from the hash - they are not in order and distances are squared
  SpacialHash::CellRec *recs;
  int numCells = faceHash.getCells(&recs, pTest);

  //  flags for used triangles
  Array<bool> flags(s.triangles.getSize());
  flags.clear();

  //  flags for whether point has been tested
  Array<bool> vertexFlags(s.vertices.getSize());
  vertexFlags.clear();

  //  find closest bit of surface
  inf->num = 0;
  inf->triangle = 0;
  inf->type = DIST_TYPE_INVALID;
  double minD = REAL_MAX;
  double stopBelowSqr = -1;
  if (stopBelow > 0)
    stopBelowSqr = stopBelow*stopBelow;

  for (int i = 0; i < numCells; i++){
    /*&& recs[i].d <= minD + EPSILON_LARGE*/  //  --  for when the cells were sorted

    SpacialHash::CellRec rec = recs[i];
    if (rec.d > minD)                         //  --  unsorted cells
      continue;

    Array<int> *tris = faceHash.getCell(rec.i, rec.j, rec.k);
    int numTris = tris->getSize();
    if (numTris > 0){
      getClosestPointINTERNAL(inf, &minD, pTest, s, stopBelowSqr, tris, &flags, &vertexFlags);

      if (minD < stopBelowSqr){
        //  opt for dodec test
        delete recs;
        return sqrt(minD);
        }
      }
    }

  delete recs;
  return sqrt(minD);
}

double getClosestPointBrute(ClosestPointInfo *inf, const Point3D &pTest, const Surface &s){
  double minD = REAL_MAX;
  int numTri = s.triangles.getSize();
  for (int i = 0; i < numTri; i++){
    //  get points
    Point3D pts[3];
    const Surface::Triangle *tri = &s.triangles.index(i);
    for (int k = 0; k < 3; k++)
      pts[k] = s.vertices.index(tri->v[k]).p;

    //  get distance
    ClosestPointInfo curInf;
    double d = distanceToTriangle(&curInf, pTest, pts);
    if (d < minD){
      minD = d;
      *inf = curInf;
      inf->triangle = i;
      }
    }

  return minD;
}

int findNeighbour(const Surface &sur, const Surface::Triangle &tri, int e[2]){
  for (int i = 0; i < 3; i++){
    int trI = tri.f[i];
    if (trI >= 0){
      //  look at neighbour
      const Surface::Triangle *testTri = &sur.triangles.index(trI);

      //  if it has both vertices from e then it is the neighbour
      bool has[2] = {false, false};
      for (int j = 0; j < 3; j++){
        if (testTri->v[j] == e[0])
          has[0] = true;
        if (testTri->v[j] == e[1])
          has[1] = true;
        }

      if (has[0] && has[1])
        return trI;
      }
    }

  return -1;
}

bool insideSurfaceClosest(const Point3D &pTest, const Surface &s, const SpacialHash &faceHash, ClosestPointInfo *inf, float stopBelow, bool allowQuickTest){
  if (inf)
    inf->type = DIST_TYPE_INVALID;

  //  quick bounding box test
  if (allowQuickTest){
    if (pTest.x < s.pMin.x || pTest.x > s.pMax.x ||
        pTest.y < s.pMin.y || pTest.y > s.pMax.y ||
        pTest.z < s.pMin.z || pTest.z > s.pMax.z){
      return false;
      }
    }

  ClosestPointInfo localClosestInf;
  if (!inf)
    inf = &localClosestInf;
  float dist = getClosestPoint(inf, pTest, s, faceHash, stopBelow);

  if (dist < stopBelow){
    //  optimise for dodec
    return true;
    }

  //  vector to point on surface
  Vector3D v;
  v.difference(pTest, inf->pClose);
  v.norm();

  if (inf->type == FACE){
    //  face test 
    Vector3D n;
    s.getTriangleNormal(&n, inf->triangle);
    double dt = n.dot(v);
    return dt <= 0;
    }
  else if (inf->type == EDGE){
    //  edge test
    const Surface::Triangle *tri = &s.triangles.index(inf->triangle);

    //  edge will be between vertices v[num] and v[(num+1)%3]
    int e[2];
    e[0] = tri->v[inf->num];
    e[1] = tri->v[(inf->num+1)%3];

    int neigh = findNeighbour(s, *tri, e);
    if (neigh >= 0){
      //  make a plane for one of the triangles
      Vector3D n1;
      s.getTriangleNormal(&n1, inf->triangle);
      Point3D p1 = s.vertices.index(e[0]).p;
      Plane pl1;
      pl1.assign(n1, p1);

      //  get the point from the other triangle which is not part of edge
      const Surface::Triangle *tri2 = &s.triangles.index(neigh);
      for (int i = 0; i < 3; i++){
        if (tri2->v[i] != e[0] && tri2->v[i] != e[1])
          break;
        }
      CHECK_DEBUG0(i != 3);
      Point3D p2 = s.vertices.index(e[1]).p;
      
      //  get signed distance to plane
      float dist = pl1.dist(p2);

      //  need normal for second triangle
      Vector3D n2;
      s.getTriangleNormal(&n2, neigh);

      if (dist <= 0.0f){
        //  faces form convex spike, back facing to both
        return v.dot(n1) <= 0 && v.dot(n2) <= 0;
        }
      else{
        //  faces form concavity, back facing to either
        return v.dot(n1) <= 0 || v.dot(n2) <= 0;
        }
      }
    else{
      OUTPUTINFO("HHHHHMMMMMMM loose edge\n");
      return false;  //  only one triangle on edge - use face ??
      }
    }
  else{// if (minType == VERTEX)
    // chosen triangle
    const Surface::Triangle *tri = &s.triangles.index(inf->triangle);

    //  chosen vertex
    int vI = tri->v[inf->num];
    Vector3D n;
    s.getVertexNormal(&n, vI);
    return n.dot(v) <= 0;

/*
    //  get all faces
    Array<int> tris;
    s.findNeighbours(&tris, vI, inf->triangle);

    //  behind test for all faces
    int numTri = tris.getSize();
    for (int i = 0; i < numTri; i++){
      Vector3D n;
      s.getTriangleNormal(&n, tris.index(i));
      double dt = n.dot(v);
      if (dt > 0)
        return false;
      }

    //  must be behind all
    return true;*/
    }
}

bool insideSurfaceCrossing(const Point3D &pTest, const Surface &s, const SpacialHash &faceHash){
  //  check against bounding box
  if (pTest.x < s.pMin.x || pTest.x > s.pMax.x ||
      pTest.y < s.pMin.y || pTest.y > s.pMax.y ||
      pTest.z < s.pMin.z || pTest.z > s.pMax.z)
    return false;

  //  generate the random ray
  Vector3D testRay;
  testRay.x = rand()/(float)RAND_MAX;
  testRay.y = rand()/(float)RAND_MAX;
  testRay.z = rand()/(float)RAND_MAX;
  testRay.norm();

  //  setup SEADS grid stepper
  SpacialHash::Stepper stepper;
  stepper.sh = &faceHash;
  Array<int> *cell = stepper.discreteSetup(pTest, testRay);

  //  flags for which triangles have been tested
  int numTri = s.triangles.getSize();
  Array<bool> done;
  done.resize(numTri);
  done.clear();

  //  start walking and testing
  int count = 0;
  while(cell) {
    //  test the triangles
    int numInCell = cell->getSize();
    for (int l = 0; l < numInCell; l++){
      int triN = cell->index(l);
      if (!done.index(triN)){
        done.index(triN) = true;

        const Surface::Triangle *tri = &s.triangles.index(triN);
        const Point3D *p0 = &s.vertices.index(tri->v[0]).p;
        const Point3D *p1 = &s.vertices.index(tri->v[1]).p;
        const Point3D *p2 = &s.vertices.index(tri->v[2]).p;

        float t = -1;
        if (intersectTriangle(pTest, testRay, &t, *p2, *p1, *p0) && t >= 0)
          count++;
        }
      }

    cell = stepper.discreteStep();
    }

  return (count%2) != 0;
}

bool insideSurfaceCrossingIter(const Point3D &p, const Surface &s,  const SpacialHash &faceHash, float mult){
  //  check against bounding box
  if (p.x < s.pMin.x || p.x > s.pMax.x ||
      p.y < s.pMin.y || p.y > s.pMax.y ||
      p.z < s.pMin.z || p.z > s.pMax.z){
    return false;
    }

  //  if we get 10 tests and they don't agree then we are ON the mesh
  //  would it be better to use the distance to surface first ??
  int inCount = 0, outCount = 0;
  while ((inCount+outCount < 1) || ((inCount < outCount*mult && outCount < inCount*mult) && inCount+outCount < 10)){
    for (int i = 0; i < 2; i++){
      bool in = insideSurfaceCrossing(p, s, faceHash);
      if (in)
        inCount++;
      else
        outCount++;
      }
    }

  if (inCount != 0 && outCount != 0)
    OUTPUTINFO("In = %d, out = %d\n", inCount, outCount);

  bool in = inCount > outCount;
  return in;
}

bool insideSurface(const Point3D &p, const Surface &s, const SpacialHash &faceHash, const SpacialHash &faceHashHiRes, ClosestPointInfo *inf, float stopBelow, bool allowQuickTest){
  if (inf){
    inf->type = DIST_TYPE_INVALID;
    inf->pClose.x = inf->pClose.y = inf->pClose.z = acos(10);
    }

  //  using crossings test
  bool in = insideSurfaceCrossingIter(p, s, faceHashHiRes);
/*
  bool inGem = insideSurfaceGem(p, s);

  if (inGem != in){
    OUTPUTINFO("Point classified differently %d %d\n", in, inGem);

    ClosestPointInfo cp;
    getClosestPoint(&cp, p, s, faceHash);
    float d = p.distance(cp.pClose);

    OUTPUTINFO("\tD = %f\n", d);
    }*/

  //  using closest point test
  //bool in1 = insideSurfaceClosest(p, s, faceHash, inf, stopBelow, allowQuickTest);

  //  may need to get the closest point
  if (!allowQuickTest && inf && inf->type == DIST_TYPE_INVALID)
    getClosestPoint(inf, p, s, faceHash);

  return in;
}
