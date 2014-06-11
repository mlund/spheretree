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

#include "Surface.h"
#include "../Geometry/Transform3D.h"
#include "../Geometry/Line2D.h"
#include "../Geometry/TriTri.h"
#include "../Base/Defs.h"
#include "VolInt.h"
#include <stdio.h>

#ifdef USE_RHINO_IO
bool Surface::loadMesh(const CRhinoMesh &mesh){
  int baseV = vertices.getSize();
  int baseT = triangles.getSize();

  //  load vertices
  const int numVert = mesh.NumVertices();
  vertices.resize(baseV + numVert);
  for (int i = 0; i < numVert; i++){
    double x, y, z;
    mesh.GetVertex(i, x, y, z);

    Point *p = &vertices.index(baseV+i);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);
    }

  //  load faces (creating vertices)
  Vector3D V1, V2;
  const int numFace = mesh.NumFaces();
  for (i = 0; i < numFace; i++){
    int v0, v1, v2, v3;
    mesh.GetFace(i, v0, v1, v2, v3);
    Triangle *t = &triangles.addItem();
    t->f[0] = t->f[1] = t->f[2] = -1;
    t->v[0] = v0 + baseV;
    t->v[1] = v1 + baseV;
    t->v[2] = v2 + baseV;

    //  triangle normal
    V1.difference(vertices.index(t->v[1]).p, vertices.index(t->v[0]).p);
    V2.difference(vertices.index(t->v[2]).p, vertices.index(t->v[0]).p);
    t->n.cross(V1,V2);
    t->n.norm();

    //  add second triangle
    if (v2 != v3){
      Triangle *t = &triangles.addItem();
      t->f[0] = t->f[1] = t->f[2] = -1;
      t->v[0] = v0 + baseV;
      t->v[1] = v2 + baseV;
      t->v[2] = v3 + baseV;

      V1.difference(vertices.index(t->v[1]).p, vertices.index(t->v[0]).p);
      V2.difference(vertices.index(t->v[2]).p, vertices.index(t->v[0]).p);
      t->n.cross(V1,V2);
      t->n.norm();
      }
    }

  //  connection to other triangles
  setupAdjacent(baseT, triangles.getSize());

  return TRUE;
}
#endif

bool Surface::loadRhinoSurface(const char *fileName, char **err, float boxSize){
#ifdef USE_RHINO_IO
  //  read rhino file
  FILE *f = fopen(fileName, "rb");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load objects
  BOOL ok = TRUE;
  try{
    CRhinoFile RF(f, CRhinoFile::read);
    CRhinoFile::eStatus status = CRhinoFile::good;

    for(;;){
      CRhinoObject *obj = RF.Read(status);
      if (status == CRhinoFile::good){
        //  load object
        if (obj->TypeCode() == TCODE_MESH_OBJECT){
          loadMesh(*(const CRhinoMesh*)obj);
          }

        //rhfree(obj);  --  doesn't work
        delete obj;
        }
      else{
        break;
        }
      }
    }
  catch(CRhinoException e){
    // if something goes wrong, catch the exception here
    if (err){
      switch ( e.m_type ) {
      case CRhinoException::unable_to_write:
        *err = "unable_to_write";
        break;
      case CRhinoException::unable_to_read:
        *err = "unable_to_read";
        break;
      case CRhinoException::unable_to_seek:
        *err = "unable_to_seek";
        break;
      case CRhinoException::unexpected_end_of_file:
        *err = "unexpected_end_of_file";
        break;
      case CRhinoException::unexpected_value:
        *err = "unexpected_value";
        break;
      case CRhinoException::not_supported:
        *err = "not_supported";
        break;
      case CRhinoException::illegal_input:
        *err = "illegal_input";
        break;
      case CRhinoException::out_of_memory:
        *err = "out_of_memory";
        break;
      default:
        *err = "unknow exception";
        break;
        }
      }
    ok = FALSE;
    }
  fclose(f);

  //  fix neighbours
  stitchTriangles();

  //  work out vertex normals
  setupNormals(0, vertices.getSize(), 0, triangles.getSize());

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return ok;
#else
  //  3DM file format disabled
  return FALSE;
#endif
}

float Surface::fitIntoBox(float boxSize){
  //  calculate scale to fit into a 2*2*2 box
  float sX = 	2.0/(pMax.x - pMin.x);
  float sY = 	2.0/(pMax.y - pMin.y);
  float sZ = 	2.0/(pMax.z - pMin.z);
  float scale = sX;
  if (sY < scale)
	  scale = sY;
  if (sZ < scale)
	  scale = sZ;
  scale *= boxSize;

  float cX = (pMax.x + pMin.x) / 2.0f;
  float cY = (pMax.y + pMin.y) / 2.0f;
  float cZ = (pMax.z + pMin.z) / 2.0f;

  int numVert = vertices.getSize();
  for (int i = 0; i < numVert; i++){
    //  pointer to vertex information
    Point*p = &vertices.index(i);
    p->p.x = (p->p.x-cX)*scale;
    p->p.y = (p->p.y-cY)*scale;
    p->p.z = (p->p.z-cZ)*scale;
    }

  //  update bounds
  pMin.x = (pMin.x-cX)*scale;
  pMin.y = (pMin.y-cY)*scale;
  pMin.z = (pMin.z-cZ)*scale;
  pMax.x = (pMax.x-cX)*scale;
  pMax.y = (pMax.y-cY)*scale;
  pMax.z = (pMax.z-cZ)*scale;

  return scale;
}

int Surface::findTriangle(int tri, int v1, int v2, int mn, int mx) const{
  CHECK_DEBUG0(mn <= mx);

  Point3D p1 = vertices.index(v1).p;
  Point3D p2 = vertices.index(v2).p;

  for (int i = mn; i < mx; i++)
    if (i != tri){
      const Triangle *tr = &triangles.index(i);
      for (int j = 0; j < 3; j++){
        int u1 = j, u2 = (j+1)%3;
        Point3D q1 = vertices.index(tr->v[u1]).p;
        Point3D q2 = vertices.index(tr->v[u2]).p;

        if ((tr->v[u1] == v1 && tr->v[u2] == v2) ||
            (tr->v[u1] == v2 && tr->v[u2] == v1)){
          return i;
          }
        }
      }

  return -1;
}

void Surface::setupAdjacent(int mn, int mx){
  CHECK_DEBUG0(mn <= mx);

  for (int i = mn; i < mx; i++){
    Triangle *tr = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      int v1 = j, v2 = (j+1)%3;
      tr->f[j] = findTriangle(i, tr->v[v1], tr->v[v2], mn, mx);
      }
    }
}

//  our own format with edge adjacency
bool Surface::loadSurface(const char *fileName, float boxSize){
  FILE *f = fopen(fileName, "r");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load points
  int numPts;
  fscanf(f, "%d", &numPts);
  vertices.resize(numPts);
  for (int i = 0; i < numPts; i++){
    Point *p = &vertices.index(i);

    float x, y, z;
    fscanf(f, "%f%f%f", &x, &y, &z);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);

    fscanf(f, "%f%f%f", &x, &y, &z);
    p->n.x = x;
    p->n.y = y;
    p->n.z = z;
    }

  //  load triangle
  int numTri;
  fscanf(f, "%d", &numTri);
  triangles.resize(numTri);
  for (int j = 0; j < numTri; j++){
    Triangle *tri = &triangles.index(j);

    //  vertices
    int a, b, c;
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->v[0] = a;
    tri->v[1] = b;
    tri->v[2] = c;

    //  neighbouring faces
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->f[0] = a;
    tri->f[1] = b;
    tri->f[2] = c;

    //  triangle normal
    Vector3D v1, v2;
    v1.difference(vertices.index(tri->v[1]).p, vertices.index(tri->v[0]).p);
    v2.difference(vertices.index(tri->v[2]).p, vertices.index(tri->v[0]).p);
    tri->n.cross(v1,v2);
    tri->n.norm();
    }

  fclose(f);

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return TRUE;
}

bool Surface::loadMinimalSurface(const char *fileName, float boxSize){
  FILE *f = fopen(fileName, "r");
  if (!f)
    return FALSE;

  //  initialise bounding box
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  //  load points
  int numPts;
  fscanf(f, "%d", &numPts);
  vertices.resize(numPts);
  for (int i = 0; i < numPts; i++){
    Point *p = &vertices.index(i);

    //  load position
    float x, y, z;
    fscanf(f, "%f%f%f", &x, &y, &z);
    p->p.x = x;
    p->p.y = y;
    p->p.z = z;

    pMin.storeMin(p->p);
    pMax.storeMax(p->p);

    //  no normal info
    }

  //  load triangle
  int numTri;
  fscanf(f, "%d", &numTri);
  triangles.resize(numTri);
  for (int j = 0; j < numTri; j++){
    Triangle *tri = &triangles.index(j);

    //  load vertices
    int a, b, c;
    fscanf(f, "%d%d%d", &a, &b, &c);
    tri->v[0] = a;
    tri->v[1] = b;
    tri->v[2] = c;

    //  no neighbours
    }
  fclose(f);

  //  work out vertex normals
  setupNormals(0, vertices.getSize(), 0, triangles.getSize());

  //  work out adjacency info
  setupAdjacent(0, triangles.getSize());

  if (boxSize > 0)
    fitIntoBox(boxSize);

  return TRUE;
}

bool Surface::saveSurface(const char *fileName, float scale){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return FALSE;

  //  dump vertices
  int numPoints = vertices.getSize();
  fprintf(f, "%d\n", numPoints);
  for (int i = 0; i < numPoints; i++){
    Point p = vertices.index(i);
    if (!fprintf(f, "%f %f %f %f %f %f\n", p.p.x*scale, p.p.y*scale, p.p.z*scale, p.n.x, p.n.y, p.n.z)){
      fclose(f);
      return FALSE;
      }
    }

  //  dump triangles
  int numTris = triangles.getSize();
  fprintf(f, "%d\n", numTris);
  for (int j = 0; j < numTris; j++){
    Triangle t = triangles.index(j);
    if (!fprintf(f, "%d %d %d %d %d %d\n", t.v[0], t.v[1], t.v[2], t.f[0], t.f[1], t.f[2])){
      fclose(f);
      return FALSE;
      }
    }

  fclose(f);
  return TRUE;
}

bool Surface::saveHavokSurface(const char *fileName){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return FALSE;

  //  dump vertices
  int numPoints = vertices.getSize();
  fprintf(f, "%d\n", numPoints);
  for (int i = 0; i < numPoints; i++){
    Point p = vertices.index(i);
    if (!fprintf(f, "%f %f %f\n", p.p.x, p.p.y, p.p.z)){
      fclose(f);
      return FALSE;
      }
    }

  //  dump triangles
  int numTris = triangles.getSize();
  fprintf(f, "%d\n", numTris);
  for (int j = 0; j < numTris; j++){
    Triangle t = triangles.index(j);
    if (!fprintf(f, "3 %d %d %d\n", t.v[0], t.v[1], t.v[2])){
      fclose(f);
      return FALSE;
      }
    }

  fclose(f);
  return TRUE;
}

void Surface::getTriangleNormal(Vector3D *n, int trI) const{
  const Triangle *tri = &triangles.index(trI);
  *n = tri->n;
/*
  Point3D pts[3];
  for (int j = 0; j < 3; j++)
    pts[j] = vertices.index(tri->v[j]).p;

  Vector3D v1, v2, vC;
  v1.difference(pts[1], pts[0]);
  v2.difference(pts[2], pts[0]);
  n->cross(v1, v2);
  n->norm();
*/
}

void Surface::getVertexNormal(Vector3D *n, int vertex) const{
  *n = vertices.index(vertex).n;
}

//  get normal of point in triangle using bi-linear interpolation
void Surface::getNormal(Vector3D *n, const Point3D &p, int tri) const{
  //  get points
  const Triangle *triangle = &triangles.index(tri);
  const Point *p0 = &vertices.index(triangle->v[0]);
  const Point *p1 = &vertices.index(triangle->v[1]);
  const Point *p2 = &vertices.index(triangle->v[2]);

  //  get plane transform
  Vector3D v1, v2, vC;
  v1.difference(p1->p, p0->p);
  v2.difference(p2->p, p0->p);
  vC.cross(v1, v2);
  Transform3D tr;
  tr.alignToZ(vC, p0->p);

  //  transform points to plane
  Point2D P0, P1, P2, Pt;
  tr.transformTo2D(&P0, p0->p);
  tr.transformTo2D(&P1, p1->p);
  tr.transformTo2D(&P2, p2->p);
  tr.transformTo2D(&Pt, p);

  //  get intersection point
  Line2D l0(P0, P1), l2(Pt, P2);
  Point2D pI;
  l0.intersect(&pI, l2);

  //  interpolate pI's value between P0 & P1
  float d01 = pI.distance(P0) / P1.distance(P0);
  Vector3D vI;
  vI.x = (1-d01)*p0->n.x + d01*p1->n.x;
  vI.y = (1-d01)*p0->n.y + d01*p1->n.y;
  vI.z = (1-d01)*p0->n.z + d01*p1->n.z;

  //  interpolate Pt's value from P2 and pI
  float d = Pt.distance(P2) / pI.distance(P2);
  n->x = (1-d)*p2->n.x + d*vI.x;
  n->y = (1-d)*p2->n.y + d*vI.y;
  n->z = (1-d)*p2->n.z + d*vI.z;
}

void Surface::getEdgeNormal(Vector3D *n, int tri, int edge) const{
  const Triangle *tr = &triangles.index(tri);

  Vector3D n1, n2;
  getTriangleNormal(&n1, tri);
  getTriangleNormal(&n2, tr->f[edge]);

  n->add(n1, n2);
  n->norm();
}

void Surface::findNeighbours(Array<int> *tris, int vertex, int startTri) const{
  //  find start triangle
  if (startTri < 0){
    int numTri = triangles.getSize();
    for (int i = 0; i < numTri; i++){
      const Triangle *tr = &triangles.index(i);
      if (tr->v[0] == vertex || tr->v[1] == vertex || tr->v[2] == vertex)
        break;
      }
    if (i != numTri)
      startTri = i;
    else
      return;
    }

  //  add start triangle to list
  tris->resize(0);
  tris->addItem() = startTri;

  int l = 0;
  while (l < tris->getSize()){
    int tI = tris->index(l);
    const Triangle *tr = &triangles.index(tI);
    for (int i = 0; i < 3; i++){
      int nI = tr->f[i];
      if (nI >= 0 && !tris->inList(nI)){
        const Triangle *trN = &triangles.index(nI);
        if (trN->v[0] == vertex || trN->v[1] == vertex || trN->v[2] == vertex)
          tris->addItem() = nI;
        }
      }
    l++;
    }
}

void Surface::setupNormals(int mn, int mx, int tn, int tx){
  //  init normals
  for (int i = mn; i < mx; i++){
    Point *p = &vertices.index(i);
    p->n.x = 0;
    p->n.y = 0;
    p->n.z = 0;
    }

  //  sum up normals from faces
  int numTri = triangles.getSize();
  for (i = tn; i < tx; i++){
    Vector3D n;
    getTriangleNormal(&n, i);

    const Triangle *t = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      Point *p = &vertices.index(t->v[j]);
      p->n.x += n.x;
      p->n.y += n.y;
      p->n.z += n.z;
      }
    }

  //  normalise normals
  for (i = mn; i < mx; i++)
    vertices.index(i).n.norm();
}

void Surface::stitchTriangles(){
  //  fix up triangles
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      if (t->f[j] < 0){
        //  points from unmatched edge
        int v0 = t->v[j], v1 = t->v[(j+1)%3];
        Point3D p0 = vertices.index(v0).p;
        Point3D p1 = vertices.index(v1).p;

        //  search for match
        REAL minD = REAL_MAX;
        int minT = -1, minV1 = -1, minV2 = -1, minE = -1;
        for (int tri = 0; tri < numTri; tri++){
          if (tri == i)
            continue;

          //  candidate match
          Triangle *t1 = &triangles.index(tri);
          for (int e = 0; e < 3; e++){
            if (t1->f[e] >= 0)
              continue;

            //  points from unmatched edge
            Point3D q0 = vertices.index(t1->v[e]).p;
            Point3D q1 = vertices.index(t1->v[(e+1)%3]).p;

            //  forward edge
            float d = p0.distance(q0) + p1.distance(q1);
            if (d < minD){
              minD = d;
              minT = tri;
              minV1 = t1->v[e];
              minV2 = t1->v[(e+1)%3];
              minE = e;
              }

            //  reverse edge
            d = p0.distance(q1) + p1.distance(q0);
            if (d < minD){
              minD = d;
              minT = tri;
              minV2 = t1->v[e];
              minV1 = t1->v[(e+1)%3];
              minE = e;
              }
            }
          }
//      CHECK_DEBUG0(minT >= 0);

        if (minT >= 0){        
          //OUTPUTINFO("minD = %f\n", minD);

          //  link triangles
          t->f[j] = minT;
          triangles.index(minT).f[minE] = i;

          //  merge first vertex
          if (minV1 != v0){
            remapVertex(minV1, v0);

            int lastV = vertices.getSize()-1;
            if (minV1 != lastV){
              vertices.index(minV1) = vertices.index(lastV);
              remapVertex(lastV, minV1);
              if (minV2 == lastV)
                minV2 = minV1;
              if (v1 == lastV)
                v1 = minV1;
              }
            vertices.setSize(lastV);
            }

          if (minV2 != v1){
            remapVertex(minV2, v1);
            int lastV = vertices.getSize()-1;
            if (minV2 != lastV){
              vertices.index(minV2) = vertices.index(lastV);
              remapVertex(lastV, minV2);
              }
            vertices.setSize(lastV);
            }
          }
        }
      }
    }
}

void Surface::remapVertex(int s, int d){
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);

    for (int j = 0; j < 3; j++){
      if (t->v[j] == s)
        t->v[j] = d;
      }
    }
}

bool Surface::vertexHasRef(int s){
  int numTri = triangles.getSize();
  for (int i = 0; i < numTri; i++){
    Triangle *t = &triangles.index(i);

    for (int j = 0; j < 3; j++){
      if (t->v[j] == s)
        return true;
      }
    }
  return false;
}

void Surface::getBoundingSphere(Sphere *s) const{
  int numPts = vertices.getSize();

  //  first pass - find maxima/minima points for the 3 axes
  Point3D xMin, xMax, yMin, yMax, zMin, zMax;
  xMin.x = yMin.y = zMin.z = REAL_MAX;
  xMax.x = yMax.y = zMax.z = REAL_MIN;
  for (int i = 0; i < numPts; i++){
    Point3D p = vertices.index(i).p;

    if (p.x < xMin.x)
      xMin = p;
    if (p.x > xMax.x)
      xMax = p;

    if (p.y < yMin.y)
      yMin = p;
    if (p.y > yMax.y)
      yMax = p;

    if (p.z < zMin.z)
      zMin = p;
    if (p.z > zMax.z)
      zMax = p; 
    }

  //  x-span is square distance between xmin & xmax
  float xspan = xMin.distanceSQR(xMax);
  float yspan = yMin.distanceSQR(yMax);
  float zspan = zMin.distanceSQR(zMax);

  //  dMin and dMax are the maximally separated pair
  float maxSpan = xspan;
  Point3D dMin = xMin, dMax = xMax;
  if (yspan > maxSpan){
    maxSpan = yspan;
    dMin = yMin;
    dMax = yMax;
    }
  if (zspan > maxSpan){
    maxSpan = zspan;
    dMin = zMin;
    dMax = zMax;
    }

  //  work out center and radius of sphere
  Point3D pC;
  pC.x = (dMin.x + dMax.x)/2.0f;
  pC.y = (dMin.y + dMax.y)/2.0f;
  pC.z = (dMin.z + dMax.z)/2.0f;
  float rCS = pC.distanceSQR(dMax);
  float rC = sqrt(rCS);

  //  second pass - increment current sphere
  for (i = 0; i < numPts; i++){
    Point3D p = vertices.index(i).p;

    float oldToPS = pC.distanceSQR(p);
    if (oldToPS > rCS){
      //  this point is outside the sphere
      float oldToP = sqrt(oldToPS);
      rC = (rC + oldToP)/2.0;
      rCS = rC*rC;
      float oldToNew = oldToP - rC;

      //  new sphere center
      pC.x = (rC*pC.x + oldToNew*p.x)/oldToP;
      pC.y = (rC*pC.y + oldToNew*p.y)/oldToP;
      pC.z = (rC*pC.z + oldToNew*p.z)/oldToP;
      }
    }

  s->c = pC;
  s->r = rC;
}

void Surface::copy(const Surface &other){
  pMin = other.pMin;
  pMax = other.pMax;

  int numVerts = other.vertices.getSize();
  vertices.resize(numVerts);
  vertices.copy(other.vertices);

  int numTris = other.triangles.getSize();
  triangles.resize(numTris);
  triangles.copy(other.triangles);
}

bool Surface::hasEdge(const Triangle *tri, int e[2]){
  bool has[2] = {false, false};
  for (int i = 0; i < 3; i++){
    if (tri->v[i] == e[0])
      has[0] = true;
    if (tri->v[i] == e[1])
      has[1] = true;
    }
  return has[0] && has[1];
}

bool Surface::hasVertex(const Triangle *tri, int v){
  return (tri->v[0] == v || tri->v[1] == v || tri->v[2] == v);
}

void Surface::testSurface(int *badVerts, int *badNeighs, int *openEdges, int *multEdges, int *badFaces) const{
  *badVerts = 0;
  *badNeighs = 0;
  *openEdges = 0;
  *multEdges = 0;
  *badFaces = 0;

  int numTri = triangles.getSize();
  int numVert = vertices.getSize();

  //  check for open edges and bad vertices
  OUTPUTINFO("Testing Open Edges...\n");
  for (int i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      if (tri->v[j] < 0 || tri->v[j] >= numVert)
        (*badVerts)++;
      if (tri->f[j] < 0 || tri->f[j] >= numTri || tri->f[j] == i)
        (*openEdges)++;
      }
    }

  //  check for multiple edges
  OUTPUTINFO("Testing Multi/Bad Edges...\n");
  for (i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    for (int j = 0; j < 3; j++){
      int e[2];
      e[0] = tri->v[j];
      e[1] = tri->v[(j+1)%3];

      //  count triangles with this edge
      int numWithEdge = 0;
      for (int k = 0; k < numTri; k++){
        if (k == i)
          continue;

        const Triangle *tri = &triangles.index(k);
        if (hasEdge(tri, e))
          numWithEdge++;
        }

      //  work out what is wrong
      if (numWithEdge > 1){
        //  too many neighbours
        (*multEdges)++;
        }
      else if (numWithEdge == 1){
        //  check that the neighbour has a reference back to us
        const Triangle *tri1 = &triangles.index(tri->f[j]);
        if ((tri1->f[0] != i) && (tri1->f[1] != i) && (tri1->f[2] != i))
          (*badNeighs)++;
        }
      }
    }

  //  check each triangle for intersections
  OUTPUTINFO("Testing Intersecting Faces...\n");
  for (i = 0; i < numTri; i++){
    const Triangle *tri = &triangles.index(i);
    Point3D t1[3];
    t1[0] = vertices.index(tri->v[0]).p;
    t1[1] = vertices.index(tri->v[1]).p;
    t1[2] = vertices.index(tri->v[2]).p;

    for (int j = i+1; j < numTri; j++){
      const Triangle *tri1 = &triangles.index(j);
      if (hasVertex(tri1, tri->v[0]) || hasVertex(tri1, tri->v[1]) || hasVertex(tri1, tri->v[2]))
        continue;

      Point3D t2[3];
      t2[0] = vertices.index(tri1->v[0]).p;
      t2[1] = vertices.index(tri1->v[1]).p;
      t2[2] = vertices.index(tri1->v[2]).p;

      if (trianglesIntersect(t1, t2))
        (*badFaces)++;
      }
    }
}

//  mass properties - taken from mirtich's code
void Surface::computeMassProperties(REAL *mass, REAL com[3], REAL it[3][3]) const{
  ::computeMassProperties(mass, com, it, *this);
}

//  compute bounding box
void Surface::setupBoundingBox(){
  pMin = Point3D::MAX;
  pMax = Point3D::MIN;

  int numVert = vertices.getSize();
  for (int i = 0; i < numVert; i++){
    Point3D p = vertices.index(i).p;
    pMin.storeMin(p);
    pMax.storeMax(p);
    }  
}
