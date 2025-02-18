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

#include "Voronoi3D.h"
#include "../Geometry/Circle.h"
#include "../Geometry/Line2D.h"
#include "../Geometry/Plane.h"
#include <limits.h>

#define MAGICK_NUM 4 //  number of forming points / neighbours
/*
int countLinks(const Voronoi3D::Vertex &v, int n){
  int c = 0;
  for (int i = 0; i < MAGICK_NUM; i++){
    if (v.n[i] == n)
      c++;
    }
  return c;
}*/
/*
void checkNeighbours(const Voronoi3D &vor, const Array<int> *deletable = NULL){
  int numVert = vor.vertices.getSize();
  for (int i = 0; i < numVert; i++){
    if (deletable && deletable->inList(i))
      continue;

    const Voronoi3D::Vertex *vert = &vor.vertices.index(i);

    for (int j = 0; j < MAGICK_NUM; j++){
      int nJ = vert->n[j];
      if (nJ < 0)
        continue;

      //  test neighbour is VALID
      if (nJ >= numVert)  // nJ < 0 ||
        OUTPUTINFO("Invalid Neighbour");
      if (nJ == i)
        OUTPUTINFO("Self Neighbour");

      //  test neighbour is unique
      for (int k = 0; k < j; k++)
        if (vert->n[k] == nJ)
          OUTPUTINFO("Duplicate Neighbour");


      const Voronoi3D::Vertex *nVert = &vor.vertices.index(nJ);
      int c1 = countLinks(*nVert, i);
      if (c1 != 1)
        OUTPUTINFO("no link back");
      }
    }
}*/

/*
    public bits
*/
void Voronoi3D::initialise(const Point3D &pC, double R) {
  //  three points on circle
  Point2D pc;
  pc.x = pC.x;
  pc.y = pC.y;
  float r = 10 * R;

  Circle c;
  c.assign(pc, r);
  Point2D p0, p1, p2;
  c.getPoint(&p0, 0);
  c.getPoint(&p1, 120);
  c.getPoint(&p2, 240);

  //  tangents to circle
  Vector2D n0, t0;
  n0.difference(p0, pc);
  t0.x = -n0.y;
  t0.y = n0.x;

  Vector2D n1, t1;
  n1.difference(p1, pc);
  t1.x = -n1.y;
  t1.y = n1.x;

  Vector2D n2, t2;
  n2.difference(p2, pc);
  t2.x = -n2.y;
  t2.y = n2.x;

  //  equation of tangent lines
  Point2D pTmp;
  Line2D l0, l1, l2;
  t0.add(&pTmp, p0);
  l0.assign(p0, pTmp);

  t1.add(&pTmp, p1);
  l1.assign(p1, pTmp);

  t2.add(&pTmp, p2);
  l2.assign(p2, pTmp);

  //  intersect tangent lines
  Point2D q0, q1, q2;
  l0.intersect(&q0, l1);
  l1.intersect(&q1, l2);
  l2.intersect(&q2, l0);

  //  position base triangle at z = pC.z - r
  Point3D Q0, Q1, Q2;
  Q0.x = q0.x;
  Q0.y = q0.y;
  Q1.x = q1.x;
  Q1.y = q1.y;
  Q2.x = q2.x;
  Q2.y = q2.y;
  Q0.z = Q1.z = Q2.z = pC.z - r;

  //  work out height of pyramid
  Line2D lB;
  lB.assign(q0, q1);
  double h = fabs(lB.distanceNorm(q2));

  //  make top point
  Point3D Qt;
  Qt.x = pC.x;
  Qt.y = pC.y;
  Qt.z = pC.z - r + h;

  initialise(Q0, Q1, Q2, Qt);

  Point3D pMin, pMax;
  pMin.x = pC.x - r;
  pMin.y = pC.y - r;
  pMin.z = pC.z - r;
  pMax.x = pC.x + r;
  pMax.y = pC.y + r;
  pMax.z = pC.z + r;
  formingHash.setup(pMin, pMax, 25);
}

void Voronoi3D::initialise(const Point3D &p0, const Point3D &p1,
                           const Point3D &p2, const Point3D &pT) {
  //  create the first vertex from the forming points
  formingPoints.resize(4);
  formingPoints.index(0).vertices.setSize(0);
  formingPoints.index(0).p = p0;
  formingPoints.index(1).vertices.setSize(0);
  formingPoints.index(1).p = p1;
  formingPoints.index(2).vertices.setSize(0);
  formingPoints.index(2).p = p2;
  formingPoints.index(3).vertices.setSize(0);
  formingPoints.index(3).p = pT;

  vertices.resize(5);
  constructVertex(4, 0, 1, 2, 3);
  Point3D pC = vertices.index(4).s.c;

  //  construct external forming points
  formingPoints.resize(8);
  formingPoints.index(4).vertices.setSize(0);
  formingPoints.index(5).vertices.setSize(0);
  formingPoints.index(6).vertices.setSize(0);
  formingPoints.index(7).vertices.setSize(0);
  projectThroughFace(&formingPoints.index(4).p, pC, p0, p1, p2);
  projectThroughFace(&formingPoints.index(5).p, pC, p0, p1, pT);
  projectThroughFace(&formingPoints.index(6).p, pC, p1, p2, pT);
  projectThroughFace(&formingPoints.index(7).p, pC, p2, p0, pT);

  //  construct external vertices
  constructVertex(0, 0, 1, 2, 4);
  constructVertex(1, 0, 1, 3, 5);
  constructVertex(2, 1, 2, 3, 6);
  constructVertex(3, 2, 0, 3, 7);

  //  construct initial edges
  joinVertices(4, 0);
  joinVertices(4, 1);
  joinVertices(4, 2);
  joinVertices(4, 3);

  //  construct outer edges
  joinVertices(0, 1);
  joinVertices(0, 2);
  joinVertices(0, 3);
  joinVertices(1, 2);
  joinVertices(1, 3);
  joinVertices(2, 3);

  //  setup forming hash ???
}

bool Voronoi3D::hasClosePoint(const Point3D &p, float dist) const {
  //  make cube
  Point3D pMin = {p.x - dist, p.y - dist, p.z - dist};
  Point3D pMax = {p.x + dist, p.y + dist, p.z + dist};

  //  get the indices of the bounding box
  int xMin, yMin, zMin;
  formingHash.getBoundedIndices(&xMin, &yMin, &zMin, pMin);

  int xMax, yMax, zMax;
  formingHash.getBoundedIndices(&xMax, &yMax, &zMax, pMax);

  //  test each cell
  float dS = dist * dist;
  for (int i = xMin; i <= xMax; i++) {
    for (int j = yMin; j <= yMax; j++) {
      for (int k = zMin; k <= zMax; k++) {
        const Array<int> *cellInds = formingHash.getCell(i, j, k);
        int numPts = cellInds->getSize();
        for (int l = 0; l < numPts; l++) {
          int pI = cellInds->index(l);
          if (formingPoints.index(pI).p.distanceSQR(p) < dS)
            return true;
        }
      }
    }
  }

  return false;
}

bool Voronoi3D::insertPoint(const Point3D &p, const Vector3D &n,
                            int startVertex, const Tester *tester) {
  //  co-incident check
  if (hasClosePoint(p, EPSILON_LARGE))
    return false;

  //  add to forming points  --  need to have it there to check continuity etc.
  int fpI = formingPoints.addIndex();
  FormingPoint *fp = &formingPoints.index(fpI);
  fp->vertices.setSize(0);
  fp->p = p;
  fp->v = n;

  //  flags for deletion set
  Array<bool> flags;
  flags.resize(vertices.getSize());
  flags.clear();

  //  find deletable pairs
  Array<int> deletable;
  deletableSearch(deletable, flags, p, startVertex, false); //  GRB 6 Jan 2003
  int numDeletable = deletable.getSize();
  if (numDeletable == 0) {
    formingPoints.setSize(formingPoints.getSize() - 1);
    return false;
  }

  //  add forming point to hash
  formingHash.addPoint(fpI, p);

  //  for pair of points make new vertex from the
  //  3 shared points, i.e. on either side of edge
  int numVert = vertices.getSize();
  for (int i = 0; i < numDeletable; i++) {
    int d = deletable.index(i);
    const Vertex *v = &vertices.index(d);

    for (int j = 0; j < MAGICK_NUM; j++) {
      int u = v->n[j];
        if (u < 0 || u >= vertices.getSize()) {
            OUTPUTINFO("Invalid vertex neighbour index %d in vertex %d's \n", u, d);
            continue; // Skip invalid forming point indices
        }
      // CHECK_DEBUG0(flags.index(u) == deletable.inList(u));
      if (flags.index(u))
        continue;

      //  we now have a pair (Vd and Vu)
      int common[3], nCom;
      nCom = sharedFormingPoints(common, d, u);
      CHECK_DEBUG0(nCom == 3);

      //  form new vertex
      int vNewI = vertices.addIndex();
      constructVertex(vNewI, formingPoints.getSize() - 1, common[0], common[1],
                      common[2], u);

      //  setup link to p.u
      addNeighbour(vNewI, u);

      //  replace p.u's reference to v with reference to vNew
      // CHECK_DEBUG0(u != vNewI && d != vNewI);
      replaceNeighbour(&vertices.index(u), d, vNewI);

      //  invalidate d's reference to u  --  done later
      // replaceNeighbour(&vertices.index(d), u, -1);
    }
  }

  //  link new vertices which contain common (new vertices are between numVert
  //  and newNumVert)
  int newNumVert = vertices.getSize();
  for (int v1 = numVert; v1 < newNumVert; v1++) {
    for (int v2 = v1 + 1; v2 < newNumVert; v2++) {
      //  count common points
      int s[3];
      int numCommon = sharedFormingPoints(s, v1, v2);

      if (numCommon == 3)
        joinVertices(v1, v2);
    }
  }

  //  need to clear the neighbours for the deletable vertices
  //  -- ADDED 13 NOV 2002 FOR STABILITY (GRB)
  for (int i = 0; i < numDeletable; i++) {
    int vI = deletable.index(i);
    Voronoi3D::Vertex *vert = &vertices.index(vI);
    for (int j = 0; j < MAGICK_NUM; j++)
      vert->n[j] = -1;
  }

  //  remove deletable vertices
  for (int i = 0; i < numDeletable; i++)
    deleteVertex(i, deletable);

  return true;
}

//  check to make sure the new vertices will be favourable
bool Voronoi3D::validNewVertices(const Array<int> &deletable,
                                 const Tester *tester) const {
  if (!tester)
    return true;

  //  for pair of points make new vertex from the
  //  3 shared points, i.e. on either side of edge
  int numVert = vertices.getSize();
  int numDeletable = deletable.getSize();
  for (int i = 0; i < numDeletable; i++) {
    int d = deletable.index(i);
    const Vertex *v = &vertices.index(d);

    for (int j = 0; j < MAGICK_NUM; j++) {
      int u = v->n[j];
        if (u < 0 || u >= vertices.getSize()) {
            OUTPUTINFO("Invalid vertex neighbour index %d in vertex %d's \n", u, d);
            continue; // Skip invalid forming point indices
        }
      if (deletable.inList(u))
        continue;

      //  we now have a pair (Vd and Vu)
      int common[3], nCom;
      nCom = sharedFormingPoints(common, d, u);
      CHECK_DEBUG0(nCom == 3);

      //  form new vertex
      Sphere s;
      constructVertexSphere(&s, formingPoints.getSize() - 1, common[0],
                            common[1], common[2], u);
      if (!tester->test(s))
        return false;
    }
  }

  return true;
}

/*
     debug bits
*/
void Voronoi3D::dump(const char *fileName) const {
  FILE *f = fopen(fileName, "w");

  fprintf(f, "Forming Points\n");
  int num = formingPoints.getSize();
  for (int i = 0; i < num; i++) {
    Point3D p = formingPoints.index(i).p;
    fprintf(f, "\t%d\t%f %f\t%f\n", i, p.x, p.y, p.z);
  }

  fprintf(f, "Vertices Points\n");
  num = vertices.getSize();
  for (int i = 0; i < num; i++) {
    const Vertex *v = &vertices.index(i);
    fprintf(f, "\t%d  %f %f %f\n", i, v->s.c.x, v->s.c.y, v->s.c.z);
    fprintf(f, "\t\t%d %d %d %d\n", v->f[0], v->f[1], v->f[2], v->f[3]);
    fprintf(f, "\t\t%d %d %d %d\t\t%d\n", v->n[0], v->n[1], v->n[2], v->n[3]);
  }
}

/*
    vertex stuff
*/
bool Voronoi3D::constructVertexSphere(Sphere *s, int f0, int f1, int f2, int f3,
                                      int bV) const {
  /*
    points of tetrahedral
  */
  Point3D p0 = formingPoints.index(f0).p;
  Point3D p1 = formingPoints.index(f1).p;
  Point3D p2 = formingPoints.index(f2).p;
  Point3D p3 = formingPoints.index(f3).p;
  int forming[4] = {f0, f1, f2, f3};

  /*
      code wripped from web
  */
  double xba, yba, zba, xca, yca, zca, xda, yda, zda;
  double balength, calength, dalength;
  double xcrosscd, ycrosscd, zcrosscd;
  double xcrossdb, ycrossdb, zcrossdb;
  double xcrossbc, ycrossbc, zcrossbc;
  double denominator;
  double xcirca, ycirca, zcirca;

  /* Use coordinates relative to point `a' of the tetrahedron. */
  xba = p1.x - p0.x;
  yba = p1.y - p0.y;
  zba = p1.z - p0.z;
  xca = p2.x - p0.x;
  yca = p2.y - p0.y;
  zca = p2.z - p0.z;
  xda = p3.x - p0.x;
  yda = p3.y - p0.y;
  zda = p3.z - p0.z;

  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba + zba * zba;
  calength = xca * xca + yca * yca + zca * zca;
  dalength = xda * xda + yda * yda + zda * zda;

  /* Cross products of these edges. */
  xcrosscd = yca * zda - yda * zca;
  ycrosscd = zca * xda - zda * xca;
  zcrosscd = xca * yda - xda * yca;
  xcrossdb = yda * zba - yba * zda;
  ycrossdb = zda * xba - zba * xda;
  zcrossdb = xda * yba - xba * yda;
  xcrossbc = yba * zca - yca * zba;
  ycrossbc = zba * xca - zca * xba;
  zcrossbc = xba * yca - xca * yba;

  /* Calculate the denominator of the formulae. */
  double numerator = (xba * xcrosscd + yba * ycrosscd + zba * zcrosscd);
  if (!finite(numerator) || fabs(numerator) <= EPSILON) {
    if (bV >= 0) {
      //  use fallback vertex
      // OUTPUTINFO("Construct Vertex - Falling Back on Undeletable Point\n");
      s->c = vertices.index(bV).s.c;

      s->r = 0;
      for (int i = 0; i < MAGICK_NUM; i++) {
        float r = s->c.distanceSQR(formingPoints.index(forming[i]).p);
        if (r > s->r)
          s->r = r;
      }
      s->r = sqrt(s->r);
    } else {
      return false;
    }
  } else {
    denominator = 0.5 / numerator;

    /* Calculate offset (from `a') of circumcenter. */
    xcirca = (balength * xcrosscd + calength * xcrossdb + dalength * xcrossbc) *
             denominator;
    ycirca = (balength * ycrosscd + calength * ycrossdb + dalength * ycrossbc) *
             denominator;
    zcirca = (balength * zcrosscd + calength * zcrossdb + dalength * zcrossbc) *
             denominator;

    //  put in vertex
    s->c.x = xcirca + p0.x;
    s->c.y = ycirca + p0.y;
    s->c.z = zcirca + p0.z;
    s->r = s->c.distance(formingPoints.index(f0).p);
  }

  return true;
}

bool Voronoi3D::constructVertex(int s, int f0, int f1, int f2, int f3, int bV,
                                bool sF) {
    if (s < 0 || s >= vertices.getSize()) {
        OUTPUTINFO("Invalid vertex index in constructVertex: %d", s);
        return false;
    }
  Vertex *newVert = &vertices.index(s);
  newVert->flag = VOR_FLAG_UNKNOWN;
  newVert->closestPt.x = newVert->closestPt.y = newVert->closestPt.z = acos(10);
  newVert->closestTri = -1;
  newVert->s.r = -1;
  newVert->err = -1;

  //  store forming points of the vertex
  newVert->f[0] = f0;
  newVert->f[1] = f1;
  newVert->f[2] = f2;
  newVert->f[3] = f3;

  //  initialise neighbours
  newVert->n[0] = -1;
  newVert->n[1] = -1;
  newVert->n[2] = -1;
  newVert->n[3] = -1;

  // CHECK_DEBUG0(f0 != f1 && f0 != f2 && f0 != f3);
  // CHECK_DEBUG0(f1 != f2 && f1 != f3);
  // CHECK_DEBUG0(f2 != f3);

  //  make the sphere
  if (!constructVertexSphere(&newVert->s, f0, f1, f2, f3, bV))
    return false;

  if (sF) {
    //  add to forming points lists
    for (int i = 0; i < MAGICK_NUM; i++) {
        int formingIndex = newVert->f[i];
        if (formingIndex < 0 || formingIndex >= formingPoints.getSize()) {
            OUTPUTINFO("Invalid forming point index %d in constructVertex\n", formingIndex);
            continue; // Skip invalid forming points
        }
        FormingPoint *f = &formingPoints.index(formingIndex);
      // CHECK_DEBUG0(!f->vertices.inList(s));
      f->vertices.addItem() = s;
    }
  }

  return true;
}

void Voronoi3D::addNeighbour(Vertex *vert, int n) {
  CHECK_DEBUG0(n < vertices.getSize());

  //  find the first unused neighbour
  int i;
  for (i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] < 0)
      break;

  CHECK_DEBUG(i < MAGICK_NUM, "Not able to add neighbour");
  vert->n[i] = n;
  // resetFlag(vert);
}

void Voronoi3D::addNeighbour(int dest, int n) {
  CHECK_DEBUG0(dest != n);
  CHECK_DEBUG0(n < vertices.getSize());
    if (dest == n) {
        OUTPUTINFO("Attempted to add self as neighbour: %d \n", dest);
        return; // Prevent self-neighboring
    }
  if (n != -1 && (n < 0 || n >= vertices.getSize())) {
        OUTPUTINFO("Attempted to add invalid neighbour index: %d \n", n);
        return; // Early exit or handle the error as appropriate
  }
  //  find the first unused neighbour
  Vertex *vert = &vertices.index(dest);
  int i;
  for (i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] < 0)
      break;

  if (i < MAGICK_NUM) {
      vert->n[i] = n;
  } else {
      OUTPUTINFO("Unable to add neighbour %d: No available slot in Vertex.n \n", n);

  }
    // resetFlag(vert); // Uncomment if necessary
}

bool Voronoi3D::isNeighbour(int v, int n) const {
    if (v < 0 || v >= vertices.getSize()) {
        OUTPUTINFO("Invalid vertex index in isNeighbour: %d \n", v);
        return false;
    }

    if (n != -1 && (n < 0 || n >= vertices.getSize())) {
        OUTPUTINFO("Invalid neighbour index in isNeighbour: %d \n", n);
        return false;
    }

    const Vertex *vert = &vertices.index(v);
  return isNeighbour(vert, n);
}

bool Voronoi3D::isNeighbour(const Vertex *vert, int n) const {
    if (n != -1 && (n < 0 || n >= vertices.getSize())) {
        OUTPUTINFO("Invalid neighbour index in isNeighbour: %d \n", n);
        return false;
    }

    for (int i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] == n)
      return true;

  return false;
}

void Voronoi3D::joinVertices(int i, int j) {
  CHECK_DEBUG(i != j, "Can't join to itself");

  addNeighbour(i, j);
  addNeighbour(j, i);
}

int Voronoi3D::findClosestVertex(const Point3D &p) const {
  int minI = -1;
  double minD = REAL_MAX;
  int n = vertices.getSize();
  for (int i = 4; i < n; i++) {
    double d = vertices.index(i).s.c.distanceSQR(p);
    if (d < minD) {
      minD = d;
      minI = i;
    }
  }

  return minI;
}

void Voronoi3D::replaceNeighbour(Vertex *v, int oldN, int newN) {
    // Allow only valid indices or -1
    if (newN != -1 && (newN < 0 || newN >= vertices.getSize())) {
        OUTPUTINFO("Attempted to replace with invalid neighbour index: %d \n", newN);
        return; // Early exit or handle the error as appropriate
    }

    bool replaced = false;
    for (int i = 0; i < MAGICK_NUM; i++) {
        if (v->n[i] == oldN) {
            v->n[i] = newN;
            replaced = true;
            break;
        }
    }

    if (!replaced) {
        OUTPUTINFO("Old neighbour %d not found when attempting to replace with %d \n", oldN, newN);
        // Optionally, handle the situation (e.g., log an error or add the new neighbor)
    }

  // resetFlag(v);
}

/*
    forming points
*/
int Voronoi3D::sharedFormingPoints(int s[3], int vI, int vJ) const {
  const Vertex *v1 = &vertices.index(vI);
  return sharedFormingPoints(s, *v1, vJ);
}

int Voronoi3D::sharedFormingPoints(int s[3], const Vertex &v1, int vJ) const {
  const Vertex *v2 = &vertices.index(vJ);

  int sI = 0;
  for (int i = 0; i < MAGICK_NUM; i++) {
    int nI = v1.f[i];
    for (int j = 0; j < MAGICK_NUM; j++) {
      if (nI == v2->f[j]) {
        //  common forming point
        if (sI < 3)
          s[sI++] = v1.f[i];
        else
          return sI;
      }
    }
  }

  return sI;
}

/*
    vertex deletion
*/
void Voronoi3D::remap(Array<int> &list, int oldV, int newV, int sI) const {
  // CHECK_DEBUG0(list.inList(oldV));

  int s = list.getSize();
  for (int i = sI; i < s; i++) {
    if (list.index(i) == oldV) {
      list.index(i) = newV;
      break;
    }
  }

  // CHECK_DEBUG0(i != s);  //  used for remapping item in delete list if it is
  // there
}

void Voronoi3D::deleteVertex(int dI, Array<int> &deletable) {
    if (dI < 0 || dI >= deletable.getSize()) {
        OUTPUTINFO("Invalid deletable index in deleteVertex: %d      \n", dI);
        return;
    }
  //  remove vertex from forming point's list
  int n = deletable.index(dI);
    if (n < 0 || n >= vertices.getSize()) {
        OUTPUTINFO("Invalid vertex index to delete in deleteVertex: %d \n", n);
        return;
    }
  CHECK_DEBUG0(n < vertices.getSize());

  Vertex *v = &vertices.index(n);
  for (int i = 0; i < MAGICK_NUM; i++) {
    FormingPoint *f = &formingPoints.index(v->f[i]);
    /*
        //  count occurances of n in f->vertices
        int count = 0;
        for (int l = 0; l < f->vertices.getSize(); l++)
          if (f->vertices.index(l) == n)
            count++;
        if (count > 1)
          OUTPUTINFO("Count = %d\n", count);
    */
    //  scan for n
    int j, numForm = f->vertices.getSize();
    for (j = 0; j < numForm; j++) {
      if (f->vertices.index(j) == n)
        break;
    }
    CHECK_DEBUG0(j != numForm);
      if (j == numForm) {
          OUTPUTINFO("Vertex %d not found in forming point %d's list during deletion \n", n, v->f[i]);
          continue;
      }
    //  remove n
    f->vertices.index(j) = f->vertices.index(numForm - 1);
    f->vertices.setSize(numForm - 1);
  }

  /*
  //  check no forming point contains vertex number n
  for (int a = 0; a < formingPoints.getSize(); a++)
    if (formingPoints.index(a).vertices.inList(n))
      OUTPUTINFO("AAAARRRRRRRGGGGHHHHHH : forming point still has refs to
  deleted vertex\n");

  // nothing should require N as it's neighbour either
  for (int b = 0; b < vertices.getSize(); b++){
    if (!deletable.inList(b)){
      const Vertex *v = &vertices.index(b);
      for (int i = 0; i < MAGICK_NUM; i++)
        if (v->n[i] == n)
          OUTPUTINFO("AAAAAARRRRGGGGHHHHHH : someone still has N as a
  neighbour\n");
        }
      }
  */

  //  remove from list of vertices
  int lastV = vertices.getSize() - 1;
  if (n != lastV) {
    //  move last entry into free slot
    Vertex *rV = &vertices.index(lastV);
    vertices.index(n) = vertices.index(lastV);

    //  remap forming point's list
    for (int i = 0; i < MAGICK_NUM; i++) {
      FormingPoint *f = &formingPoints.index(rV->f[i]);
      remap(f->vertices, lastV, n);
    }

    //  remap neighbours
    for (int i = 0; i < MAGICK_NUM; i++) {
      int nI = rV->n[i];
      if (nI <
          0) //  if node is to be delete then it will have -1 for neighbours
        continue; //  so we won't call remap -- ADDED 13 NOV 2002 FOR STABILITY
                  //  (GRB)

      CHECK_DEBUG0(nI <= lastV);
        if (nI == lastV) {
            // Prevent a vertex from being its own neighbor
            OUTPUTINFO("Vertex %d cannot be a neighbor to itself after deletion \n", nI);
            rV->n[i] = -1;
            continue;
        }

        if (nI >= vertices.getSize()) {
            OUTPUTINFO("Neighbor index out of bounds after deletion: %d \n", nI);
            rV->n[i] = -1;
            continue;
        }

      Vertex *vN = &vertices.index(nI);
      replaceNeighbour(vN, lastV, n);
    }

    //  remap deletable points
    remap(deletable, n, -1);

    // if (deletable.inList(lastV))  //  only need to prevent assert !!!
    remap(deletable, lastV, n, dI);
  }

  //  shrink vertex list
  vertices.resize(lastV);

  /*
  //  check no forming point contains vertex number lastV
  for (a = 0; a < formingPoints.getSize(); a++)
    if (formingPoints.index(a).vertices.inList(lastV))
      OUTPUTINFO("AAAARRRRRRRGGGGHHHHHH : some forming points still want
  lastV\n");

  // nothing should require N as it's neighbour either
  for (b = 0; b < vertices.getSize(); b++){
    const Vertex *v = &vertices.index(b);
    for (int i = 0; i < MAGICK_NUM; i++)
      if (v->n[i] == lastV)
        OUTPUTINFO("AAAAAARRRRGGGGHHHHHH : someone still has lastV as a
  neighbour\n");
    }*/
    for (int i = 0; i < vertices.getSize(); i++) {
        Vertex *vert = &vertices.index(i);
        for (int j = 0; j < MAGICK_NUM; j++) {
            if (vert->n[j] == lastV) {
                // Replace with the new index or set to -1 if invalid
                vert->n[j] = (n < vertices.getSize()) ? n : -1;
            }
            // Additionally, validate the neighbor index
            if (vert->n[j] >= vertices.getSize()) {
                OUTPUTINFO("Invalid neighbor index %d found in vertex %d's neighbor list. Setting to -1.\n", vert->n[j], i);
                vert->n[j] = -1;
            }
        }
    }
}

/*
    misc
*/
void Voronoi3D::projectThroughFace(Point3D *newP, const Point3D &p,
                                   const Point3D &p0, const Point3D &p1,
                                   const Point3D &p2) const {
  //  distance from plane
  Plane pl;
  pl.assign(p0, p1, p2);
  double d = pl.dist(p);

  //  project along normal
  Vector3D n;
  pl.getNormal(&n);
  n.add(newP, p, -25 * d);
}

/*
    search stuff
*/
//  does adding vI to the set keep it valid - assume it is already valid
bool Voronoi3D::validDeletionSet(const Array<int> &deletable,
                                 const Array<bool> &flags, int vI) const {
  bool ok = canConnect(deletable, vI);

  //  other criteria
  const Vertex *v = &vertices.index(vI);
  for (int i = 0; i < MAGICK_NUM && ok; i++) {
    int fI = v->f[i];

    //  deletable and non-deletable vertices cell are connected
    Array<int> delNodes, nodes;
    const FormingPoint *f = &formingPoints.index(fI);
    int numVert = f->vertices.getSize();
    for (int j = 0; j < numVert; j++) {
      int vC = f->vertices.index(j);
      if (flags.index(vC)) {
        delNodes.addItem() = vC;
      } else if (vC != vI) {
        nodes.addItem() = vC;
      }
    }

    ok = nodes.getSize() != 0 && isConnected(nodes) && canConnect(delNodes, vI);
  }

  return ok;
}

bool Voronoi3D::isConnected(const Array<int> &nodes) const {
  //  check that all the nodes form a connected sub graph
  int numNodes = nodes.getSize();
  if (numNodes <= 1)
    return TRUE;

  //  setup initial graph (containing arbitaary point)
  Array<bool> inGraph;
  inGraph.resize(numNodes);
  inGraph.clear();
  inGraph.index(0) = true;
  int numUsed = 1;

  //  while there are more to be connected
  while (numUsed != numNodes) {
    bool added = FALSE; //  have we managed to progress this iteration ?

    //  find points to add to graph
    for (int i = 0; i < numNodes; i++) {
      if (!inGraph.index(i)) {
        //  get vertex
        int vI = nodes.index(i);
        const Vertex *v = &vertices.index(vI);

        //  try to find neighbour in graph
        int j;
        for (j = 0; j < numNodes; j++) {
          if (i == j)
            continue;

          int nI = nodes.index(j);
          if (inGraph.index(j) && isNeighbour(v, nI))
            break;
        }

        if (j != numNodes) {
          added = true;            //  made some progress
          inGraph.index(i) = true; //  add to graph
          numUsed++;
        }
      }
    }

    if (!added)
      break; //  no progress - terminate
  }

  return numUsed == numNodes;
}

bool Voronoi3D::canConnect(const Array<int> &list, int newV) const {
  int num = list.getSize();
  if (num == 0) {
    return true;
  } else {
    for (int i = 0; i < num; i++)
      if (isNeighbour(list.index(i), newV))
        return true;
  }

  return false;
}

double Voronoi3D::getValue(int vI, const Point3D &p) {
  //  compute distance to forming points and new point
  /*  const Vertex *v = &vertices.index(vI);
    double dF = -1;
    for (int i = 0; i < 4; i++){
      double d = formingPoints.index(v->f[i]).p.distanceSQR(v->s.c);
      if (d > dF)
        dF = d;
      }

    //  vertex is closer to p than forming points
    double dP = v->s.c.distanceSQR(p);
    return sqrt(dF) - sqrt(dP);
  */
  const Vertex *v = &vertices.index(vI);
  return v->s.r - v->s.c.distance(p);
}

void Voronoi3D::deletableSearch(Array<int> &deletable, Array<bool> &flags,
                                const Point3D &p, int startVertex,
                                bool testALL) { //  GRB 6 Jan 2003
  int numVert = vertices.getSize();

  //  scan for highest value
  int bestI = -1;
  double bestV = REAL_MIN;
  if (startVertex < 0) {
    for (int i = 0; i < numVert; i++) {
      double v = getValue(i, p);
      if (v > bestV) {
        bestV = v;
        bestI = i;
      }
    }
    // CHECK_DEBUG(bestV >= 0, "Should be at least one deletable vertex");
    if (bestV <= 0)
      return;
  } else {
    bestI = startVertex;
    bestV = getValue(bestI, p);
  }

  //  flags for used vertices (in deletable list or queue)
  Array<bool> used;
  used.resize(numVert);
  used.clear();

  //  priority queue type thing (as array)
  Array<PriorityQueue> queue;
  PriorityQueue *pqI = &queue.addItem();
  pqI->i = bestI;
  pqI->v = bestV;
  used.index(bestI) = true;

  //  record if we started this using testALL  (Jan 2003 GRB)
  bool initTA = testALL;

  //  greedy insert
  deletable.resize(0);
  while (queue.getSize() != 0) {
    Array<PriorityQueue> illegal; //  illegally deletable vertices
    bool gotValid = FALSE;

    while (queue.getSize() != 0 && !gotValid) {
      //  find the best entry in queue
      int bestI = -1;
      double bestV = REAL_MIN;
      int numQ = queue.getSize();
      for (int i = 0; i < numQ; i++) {
        const PriorityQueue *pqI = &queue.index(i);
        if (pqI->v > bestV) {
          bestV = pqI->v;
          bestI = i; //  **  entry number in queue
        }
      }

      //  get entry from queue
      CHECK_DEBUG0(bestI != -1);
      PriorityQueue pqI = queue.index(bestI);
      queue.removeItem(bestI);

      //  check for valid deletable set -- validDeletionSet uses HUBBARD's
      //  method extended Jan 2003 (GRB) to only check marginally deletable
      //  vertices once we find a marginally deletable vertex we need to start
      //  checking vertices to make sure ;)
      bool needTest = testALL || (pqI.v < 1E-1);

      //  do check is necessary
      bool validAddition = true;
      if (needTest) {
        // OUTPUTINFO("Using Deletion Test\n");
        validAddition = validDeletionSetNew(deletable, flags, pqI.i);
        if (!validAddition) {
          // OUTPUTINFO("Turning On Deletion Test From Now On");
          testALL = true; //  turn on test for rest of this insertion
        }
      }

      if (validAddition) {
        //  add to deletable list
        // CHECK_DEBUG0(!deletable.inList(vI));
        deletable.addItem() = pqI.i;
        flags.index(pqI.i) = true;
        used.index(pqI.i) = true;

        //  add positive neighbours to queue
        const Vertex *v = &vertices.index(pqI.i);
        for (int i = 0; i < MAGICK_NUM; i++) {
          int vN = v->n[i];
            if (vN < 0 || vN >= vertices.getSize()) {
                OUTPUTINFO("Invalid vertex neighbour index %d in vertex %d's ", vN, pqI.i);
                continue; // Skip invalid forming point indices
            }
          if (!used.index(vN)) {
            float v = getValue(vN, p);
            if (v >= 0) {
              used.index(vN) = true;
              PriorityQueue *pqI = &queue.addItem();
              pqI->i = vN;
              pqI->v = v;
            }
          }
        }

        //  signal that we have one
        gotValid = TRUE;
        break;
      } else {
        illegal.addItem() = pqI;
      }
    }

    if (!gotValid)
      break; //  couldn't add more - so we're done

    //  transfer illegal ones back into list
    queue.append(illegal);
  }

  //  check the deletable set is complete --  DON'T USE THIS BIT FOR HUBBARD
  // if (!validDeletionSetNew(deletable, flags, -1, TRUE))
  //  deletable.setSize(0);

  //  Jan 2003 (GRB) if the final set if not valid we may have
  //  fucked up if we weren't checking all the vertices for this
  //  insertion so we need to rerun
  if (!initTA) {
    bool valid = validDeletionSetNew(deletable, flags, -1, TRUE);
    if (!valid) {
      // OUTPUTINFO("RE-RUNNING SEARCH WITH FULL TESTING\n");

      deletable.setSize(0);
      deletableSearch(deletable, flags, p, startVertex, true);

      // if (deletable.getSize()){
      //   OUTPUTINFO("RE-RUN FOUND A VALID SET !! \n");
      //   }
    }
  }
}

//  will we be able to update the Voronoi diagram with this set
bool Voronoi3D::validDeletionSetNew(Array<int> &deletable, Array<bool> &flags,
                                    int vI, bool complete) {
  //  look in deletableList for node
  // CHECK_DEBUG0(vI < 0 || (!deletable.inList(vI) && !flags.index(vI)));

  //  add to list for testing
  int numVert = vertices.getSize();
  if (vI >= 0) {
    deletable.addItem() = vI;
    flags.index(vI) = true;
  }

  //  check that voronoi can be updated
  bool ok = true;

  //  for pair of points (Vd and Vu)
  //  make new vertex from the 3 shared forming points
  int numDeletable = deletable.getSize();
  for (int i = 0; i < numDeletable && ok; i++) {
    int d = deletable.index(i);
    const Vertex *v = &vertices.index(d);

    for (int j = 0; j < MAGICK_NUM && ok; j++) {
      int u = v->n[j];
        if (u < 0 || u >= vertices.getSize()) {
            OUTPUTINFO("Invalid vertex neighbour index %d in vertex %d's \n", u, d);
            continue; // Skip invalid forming point indices
        }
      if (flags.index(u))
        continue;

      //  we now have a pair (Vd and Vu)
      int common[3];
      int nCom = sharedFormingPoints(common, d, u);
      if (nCom != 3) {
        ok = false;
      } else {
        //  make new vertex (don't care about location etc.)
        int vNewI = vertices.addIndex();
        Vertex *newV = &vertices.index(vNewI);
        newV->n[1] = newV->n[2] = newV->n[3] = -1;
        newV->f[0] = formingPoints.getSize() - 1;
        newV->f[1] = common[0];
        newV->f[2] = common[1];
        newV->f[3] = common[2];

        //  make U a neighbour of new vertex
        newV->n[0] = u;

        //  try replace p.u's reference to d with reference to vNew
        ok = canReplaceNeighbour(&vertices.index(u), d, vNewI);
      }
    }
  }

  if (ok) {
    //  link new vertices which contain common (new vertices are between numVert
    //  and newNumVert)
    int newNumVert = vertices.getSize();
    for (int v1 = numVert; v1 < newNumVert && ok; v1++) {
      for (int v2 = v1 + 1; v2 < newNumVert && ok; v2++) {
        //  count common points
        int s[3];
        int numCommon = sharedFormingPoints(s, v1, v2);

        if (numCommon == 3)
          ok = canAddNeighbour(v1, v2) && canAddNeighbour(v2, v1);
      }
    }
  }

  //  check all new vertices have exactly enough neighbours
  if (ok && complete) {
    int newNumVert = vertices.getSize();
    for (int i = numVert; i < newNumVert && ok; i++) {
      const Vertex *v = &vertices.index(i);
      if (v->n[MAGICK_NUM - 1] < 0)
        ok = false;
    }
  }

  //  remove temportary vertices and points
  vertices.resize(numVert);
  if (vI >= 0) {
    deletable.resize(deletable.getSize() - 1);
    flags.index(vI) = false;
  }

  return ok;
}

//  test AND set - as we need to know we have enough room for all
//  only ever carried out on new vertices anyways
bool Voronoi3D::canAddNeighbour(int dest, int n) {
  CHECK_DEBUG0(dest != n);
  CHECK_DEBUG0(n < vertices.getSize());

  Vertex *vert = &vertices.index(dest);

  //  check N is not already a neighbour
  for (int i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] == n)
      return false;

  //  find the first unused neighbour
  int i;
  for (i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] < 0)
      break;

  //  if there is a blank space we can use it :)
  if (i < MAGICK_NUM) {
    vert->n[i] = n;
    return true;
  } else {
    return false;
  }
}

//  just test
bool Voronoi3D::canReplaceNeighbour(Vertex *vert, int oldN, int newN) {
  CHECK_DEBUG0(newN < vertices.getSize());

  //  check N is not already a neighbour
  for (int i = 0; i < MAGICK_NUM; i++)
    if (vert->n[i] == newN)
      return false;

  //  need oldN to be neighbour
  for (int i = 0; i < MAGICK_NUM; i++) {
    if (vert->n[i] == oldN)
      return true;
  }
  return false;
}

/*
    Random insert
*/
void Voronoi3D::randomInserts(Array<Surface::Point> &pts, int maxNum,
                              ProcessorMonitor *mon) {
  int numUsed = 0;
  int numSamples = pts.getSize();
  while (numUsed < numSamples) {
    //  update progress meter
    if (mon && !mon->setProgress(100.0f * numUsed / (float)numSamples))
      return;

    //  get index
    int numLeft = numSamples - numUsed;
    int sI = rand() % numLeft;

    const Surface::Point *pt = &pts.index(sI);
    bool added = insertPoint(pt->p, pt->n);

    //  swap with last unused element
    int lastUnused = numLeft - 1;
    if (lastUnused >= 0 && lastUnused != sI) {
      Surface::Point s = pts.index(sI);
      pts.index(sI) = pts.index(lastUnused);
      pts.index(lastUnused) = s;
    }
    numUsed++;

    if (maxNum > 0 && formingPoints.getSize() - 9 > maxNum)
      break;
  }
}

/*
    reset the cover and internal flags for adaptive sampler
*/
void Voronoi3D::resetFlag(Vertex *v) {
  if (v->flag == VOR_FLAG_OLDINTERNAL)
    v->flag = VOR_FLAG_INTERNAL;
  else if (v->flag == VOR_FLAG_OLDCOVER)
    v->flag = VOR_FLAG_EXTERNAL;
}

bool Voronoi3D::getCloserForming(int *newForming, float *newDistance,
                                 const Point3D &pTest, int currentForming,
                                 float currentDistance) const {
    if (currentForming < 0 || currentForming >= formingPoints.getSize()) {
        OUTPUTINFO("Invalid currentForming index in getCloserForming: %d \n", currentForming);
        return false;
    }
  //  current forming point
  const Voronoi3D::FormingPoint *form = &formingPoints.index(currentForming);

  //  for each neighbour's forming points find the closest
  int numVerts = form->vertices.getSize();
  for (int i = 0; i < numVerts; i++) {
    //  get vertex
    int vI = form->vertices.index(i);
    if (vI < 0 || vI >= vertices.getSize()) {
        OUTPUTINFO("Invalid vertex index %d in getCloserForming\n", vI);
        continue; // Skip invalid vertex indices
    }
    const Voronoi3D::Vertex *vert = &vertices.index(vI);

    //  forming points for vertex
    for (int j = 0; j < 4; j++) {
      int fI = vert->f[j];
      if (fI < 0 || fI >= formingPoints.getSize()) {
          OUTPUTINFO("Invalid forming point index %d in vertex %d's forming points\n", fI, vI);
          continue; // Skip invalid forming point indices
      }
      const Voronoi3D::FormingPoint *form = &formingPoints.index(fI);

      float d = form->p.distanceSQR(pTest);
      if (d < currentDistance) {
        *newForming = fI;
        *newDistance = d;
        return true;
      }
    }
  }

  return false;
}

int Voronoi3D::findCell(const Point3D &pTest, float *dist) const {
  int numForming = formingPoints.getSize();
  if (numForming < 250) {
    return findCellBrute(pTest, dist);
  } else {
    int currentForming = 0; // rand() % vor.formingPoints.getSize();
    float currentDistance =
        formingPoints.index(currentForming).p.distanceSQR(pTest);

    int newForming = 0;
    float newDistance = REAL_MAX;
    while (getCloserForming(&newForming, &newDistance, pTest, currentForming,
                            currentDistance)) {
      currentForming = newForming;
      currentDistance = newDistance;
    }

    if (dist)
      *dist = currentDistance;

    return currentForming;
  }
}

int Voronoi3D::findCellBrute(const Point3D &pTest, float *dist) const {
  int closestI = -1;
  float closestD = REAL_MAX;
  int numForming = formingPoints.getSize();
  for (int i = 0; i < numForming; i++) {
    float d = formingPoints.index(i).p.distanceSQR(pTest);
    if (d < closestD) {
      closestI = i;
      closestD = d;
    }
  }

  if (dist)
    *dist = closestD;

  return closestI;
}
