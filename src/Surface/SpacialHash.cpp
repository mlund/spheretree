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

#include "SpacialHash.h"
#include "Internal.h"
#include "../Geometry/CubeTri.h"

SpacialHash::SpacialHash() { cells = NULL; }

SpacialHash::~SpacialHash() {
  if (cells)
    delete[] cells;
  cells = NULL;
}

void SpacialHash::setup(const Point3D &pMin1, const Point3D &pMax1, int dv) {
  if (cells)
    delete[] cells;
  cells = NULL;

  //  bounds of the grid
  Point3D pMin = {pMin1.x - EPSILON_LARGE, pMin1.y - EPSILON_LARGE,
                  pMin1.z - EPSILON_LARGE};
  Point3D pMax = {pMax1.x + EPSILON_LARGE, pMax1.y + EPSILON_LARGE,
                  pMax1.z + EPSILON_LARGE};
  this->pMin = pMin;

  //  size of the bounding box
  float dX = pMax.x - pMin.x;
  float dY = pMax.y - pMin.y;
  float dZ = pMax.z - pMin.z;

  //  get the initial size of the cube
  float sX = dX / dv;
  float sY = dY / dv;
  float sZ = dZ / dv;
  float sMax = sX;
  if (sY > sMax)
    sMax = sY;
  if (sZ > sMax)
    sMax = sZ;

  //  work out the size of the cubes
  float size = sMax;
  int maxAllowCubes = dv * dv * dv;
  while (size > EPSILON_LARGE) {
    //  shrink the cube size
    float newSize = size * 0.95;

    //  work out the number of cells
    int nX = ceil(dX / newSize);
    int nY = ceil(dY / newSize);
    int nZ = ceil(dZ / newSize);

    //  do we keep it
    int newTotal = nX * nY * nZ;
    if (newTotal <= maxAllowCubes)
      size = newSize;
    else
      break;
  }

  //  work out the number of cells
  nX = ceil(dX / size);
  nY = ceil(dY / size);
  nZ = ceil(dZ / size);
  d = size;

  cells = new Array<int>[nX * nY * nZ];
}

void SpacialHash::addPoint(int i, const Point3D &p) {
  Array<int> *arr = getCell(p);
  if (arr)
    arr->addItem() = i;
  else
    OUTPUTINFO("Unable to add to cell\n");
}

void SpacialHash::getBoundedIndices(int *i, int *j, int *k,
                                    const Point3D &p) const {
  int x = (p.x - pMin.x) / d;
  int y = (p.y - pMin.y) / d;
  int z = (p.z - pMin.z) / d;

  if (x < 0)
    x = 0;
  else if (x >= nX)
    x = nX - 1;

  if (y < 0)
    y = 0;
  else if (y >= nY)
    y = nY - 1;

  if (z < 0)
    z = 0;
  else if (z >= nZ)
    z = nZ - 1;

  //  CHECK_DEBUG0(x >= 0 && x < nX);
  //  CHECK_DEBUG0(y >= 0 && y < nY);
  //  CHECK_DEBUG0(z >= 0 && z < nZ);

  *i = x;
  *j = y;
  *k = z;
}

void SpacialHash::getIndices(int *i, int *j, int *k, const Point3D &p) const {
  int x = (p.x - pMin.x) / d;
  int y = (p.y - pMin.y) / d;
  int z = (p.z - pMin.z) / d;

  CHECK_DEBUG0(x >= 0 && x < nX);
  CHECK_DEBUG0(y >= 0 && y < nY);
  CHECK_DEBUG0(z >= 0 && z < nZ);

  *i = x;
  *j = y;
  *k = z;
}

void SpacialHash::addTriangle(const Surface &sur, int index) {
  //  get the triangle
  const Surface::Triangle *tri = &sur.triangles.index(index);

  Point3D pTri[3];
  pTri[0] = sur.vertices.index(tri->v[0]).p;
  pTri[1] = sur.vertices.index(tri->v[1]).p;
  pTri[2] = sur.vertices.index(tri->v[2]).p;

  //  get the bounding box of the triangle
  Point3D pMin = Point3D::MAX, pMax = Point3D::MIN;
  for (int i = 0; i < 3; i++) {
    Point3D p = pTri[i];
    pMin.storeMin(p);
    pMax.storeMax(p);

    if (p.x < this->pMin.x || p.y < this->pMin.y || p.z < this->pMin.z ||
        p.x > this->pMin.x + this->d * this->nX ||
        p.y > this->pMin.y + this->d * this->nY ||
        p.z > this->pMin.z + this->d * this->nZ)
      OUTPUTINFO("Point %d is outside the bounding box\n", i);
  }

  //  get the indices of the bounding box
  int xMin, yMin, zMin;
  getIndices(&xMin, &yMin, &zMin, pMin);

  int xMax, yMax, zMax;
  getIndices(&xMax, &yMax, &zMax, pMax);

  //  test each cell
  int numCells = 0;
  for (int i = xMin; i <= xMax; i++) {
    for (int j = yMin; j <= yMax; j++) {
      for (int k = zMin; k <= zMax; k++) {
        //  get cell boundaries
        Point3D pMin, pMax, pC;
        getCellBounds(&pMin, &pMax, i, j, k);

        if (overlapTest(pMin, pMax, pTri)) {
          getCell(i, j, k)->addItem() = index;
          numCells++;
        }
      }
    }
  }

  if (numCells == 0)
    OUTPUTINFO("AAAARRRRGGGGG triangle has no cells\n");
}

float SpacialHash::getDistanceSQR(int i, int j, int k, const Point3D &p) const {
  //  pMin and pMax of cell
  Point3D pMin, pMax;
  getCellBounds(&pMin, &pMax, i, j, k);

  //  closest point from AABB of cell and P
  float d = 0, s;

#define DO_AXIS(a)                                                             \
  s = 0;                                                                       \
  if (p.a < pMin.a)                                                            \
    s = pMin.a - p.a;                                                          \
  else if (p.a > pMax.a)                                                       \
    s = p.a - pMax.a;                                                          \
  d += s * s;

  DO_AXIS(x);
  DO_AXIS(y);
  DO_AXIS(z);

  return d; //  sqrt(d)
}

int SpacialHash::getCells(CellRec **pRec, const Point3D &p, bool sort) const {
  int numRec = 0;
  CellRec *rec = new CellRec[nX * nY * nZ];

  int minI = -1;
  float minD = FLT_MAX;
  for (int i = 0; i < nX; i++)
    for (int j = 0; j < nY; j++)
      for (int k = 0; k < nZ; k++) {
        if (getCell(i, j, k)->getSize()) {
          rec[numRec].i = i;
          rec[numRec].j = j;
          rec[numRec].k = k;
          float d = getDistanceSQR(i, j, k, p);
          rec[numRec].d = d;

          if (d < minD) {
            minI = numRec;
            minD = d;
          }
          numRec++;
        }
      }

  //  put first one in place
  if (sort) {
    qsort(rec, numRec, sizeof(CellRec), recCompare);
  } else {
    CellRec tmp = rec[0];
    rec[0] = rec[minI];
    rec[minI] = tmp;
  }

  *pRec = rec;
  return numRec;
}

int SpacialHash::recCompare(const void *elem1, const void *elem2) {
  CellRec *r1 = (CellRec *)elem1;
  CellRec *r2 = (CellRec *)elem2;
  float d = r1->d - r2->d;
  if (d < 0)
    return -1;
  else if (d > 0)
    return +1;
  else
    return 0;
}

Array<int> *SpacialHash::findCell(int index) {
  for (int i = 0; i < nX; i++)
    for (int j = 0; j < nY; j++)
      for (int k = 0; k < nZ; k++) {
        Array<int> *arr = getCell(i, j, k);
        if (arr->inList(index)) {
          return arr;
        }
      }
  return NULL;
}

void SpacialHash::getCellBounds(Point3D *pMin, Point3D *pMax, int i, int j,
                                int k) const {
  pMin->x = i * d + this->pMin.x;
  pMin->y = j * d + this->pMin.y;
  pMin->z = k * d + this->pMin.z;

  if (pMax) {
    pMax->x = pMin->x + d;
    pMax->y = pMin->y + d;
    pMax->z = pMin->z + d;
  }
}

void SpacialHash::getCellCenter(Point3D *pC, int i, int j, int k) const {
  Point3D pMin, pMax;
  getCellBounds(&pMin, &pMax, i, j, k);

  pC->x = (pMax.x + pMin.x) / 2.0f;
  pC->y = (pMax.y + pMin.y) / 2.0f;
  pC->z = (pMax.z + pMin.z) / 2.0f;
}

/*
    discrete stepper
*/
Array<int> *SpacialHash::Stepper::discreteSetup(const Point3D &rayOrigin,
                                                const Vector3D &rayDirn) {
  //  setup for ray stepping algorithm

  CHECK_DEBUG0(sh != NULL);

  //  work out entry point to grid
  double enter[3];
  if (!BoxIntersect(rayOrigin, rayDirn, enter))
    return NULL;

  //  get some stuff from the grid
  float size[3];
  sh->getSize(&size[0], &size[1], &size[2]);

  Point3D pMin = sh->pMin;
  double minPt[3] = {pMin.x, pMin.y, pMin.z};
  int numDivs[3] = {sh->getDimX(), sh->getDimY(), sh->getDimZ()};

  //  setup
  int i;
  double origin[3] = {rayOrigin.x, rayOrigin.y, rayOrigin.z};
  double raydir[3] = {rayDirn.x, rayDirn.y, rayDirn.z};
  for (i = 0; i < 3; i++) {
    pos[i] = (enter[i] - minPt[i]) / size[i];
    if (pos[i] >= numDivs[i])
      pos[i]--;

    if (raydir[i] < 0.0) {
      dir[i] = -1;
      out[i] = -1;
      inc[i] = -size[i] / raydir[i];
      offset[i] = ((double)pos[i] * size[i] + minPt[i] - origin[i]) / raydir[i];
    } else if (raydir[i] > 0.0) {
      dir[i] = 1;
      out[i] = numDivs[i];
      inc[i] = size[i] / raydir[i];
      offset[i] =
          ((double)(pos[i] + 1) * size[i] + minPt[i] - origin[i]) / raydir[i];
    } else {
      offset[i] = DBL_MAX;
    }
  }

  return sh->getCell(pos[0], pos[1], pos[2]);
}

Array<int> *SpacialHash::Stepper::discreteStep() {
  CHECK_DEBUG0(sh != NULL);

  //  advance
  if (offset[0] <= offset[1] && offset[0] <= offset[2]) {
    pos[0] += dir[0];
    offset[0] += inc[0];
    if (pos[0] == out[0])
      return NULL; //  off end of grid
  } else if (offset[1] <= offset[2]) {
    pos[1] += dir[1];
    offset[1] += inc[1];
    if (pos[1] == out[1])
      return NULL; //  off end of grid
  } else {
    pos[2] += dir[2];
    offset[2] += inc[2];
    if (pos[2] == out[2])
      return NULL; //  off end of grid
  }

  return sh->getCell(pos[0], pos[1], pos[2]);
}

bool SpacialHash::Stepper::BoxIntersect(const Point3D &rayOrigin,
                                        const Vector3D &rayDirn,
                                        double entry[3]) const {
  //  check against bounding box
  Point3D pMin = sh->pMin;
  if (rayOrigin.x < pMin.x || rayOrigin.x > pMin.x + sh->d * sh->nX ||
      rayOrigin.y < pMin.y || rayOrigin.y > pMin.y + sh->d * sh->nY ||
      rayOrigin.z < pMin.z || rayOrigin.z > pMin.z + sh->d * sh->nZ) {

    int i;
    double t, t1, t2, tf = HUGE_VAL, tn = -HUGE_VAL, tmp[3];
    double dir[3] = {rayDirn.x, rayDirn.y, rayDirn.z};
    double origin[3] = {rayOrigin.x, rayOrigin.y, rayOrigin.z};
    double minPt[3] = {pMin.x, pMin.y, pMin.z};
    double maxPt[3] = {pMin.x + sh->d * sh->nX, pMin.y + sh->d * sh->nY,
                       pMin.z + sh->d * sh->nZ};

    for (i = 0; i < 3; i++) {
      if (dir[i] == 0.0) {
        if ((origin[i] < minPt[i]) || (origin[i] > maxPt[i])) {
          return false;
        }
      } else {
        t1 = (minPt[i] - origin[i]) / dir[i];
        t2 = (maxPt[i] - origin[i]) / dir[i];

        if (t1 > t2) {
          t = t2;
          t2 = t1;
          t1 = t;
        }
        if (t1 > tn) {
          tn = t1;
        }
        if (t2 < tf) {
          tf = t2;
        }
        if (tn > tf) {
          return false;
        }
        if (tf < 0.0) {
          return false;
        }
      }
    }

    //  compute the entry point
    entry[0] = origin[0] + tn * dir[0];
    entry[1] = origin[1] + tn * dir[1];
    entry[2] = origin[2] + tn * dir[2];
  } else {
    //  inside so just use ray origin
    entry[0] = rayOrigin.x;
    entry[1] = rayOrigin.y;
    entry[2] = rayOrigin.z;
  }

  return true;
}

void SpacialHash::addSphere(const Sphere &s, int index) {
  //  make cube
  Point3D pMin = {s.c.x - s.r, s.c.y - s.r, s.c.z - s.r};
  Point3D pMax = {s.c.x + s.r, s.c.y + s.r, s.c.z + s.r};

  if (pMin.x < this->pMin.x || pMin.y < this->pMin.y || pMin.z < this->pMin.z ||
      pMax.x > this->pMin.x + this->d * this->nX ||
      pMax.y > this->pMin.y + this->d * this->nY ||
      pMax.z > this->pMin.z + this->d * this->nZ) {
    OUTPUTINFO("Sphere is outside the bounding box\n");
    return;
  }

  //  get the indices of the bounding box
  int xMin, yMin, zMin;
  getIndices(&xMin, &yMin, &zMin, pMin);

  int xMax, yMax, zMax;
  getIndices(&xMax, &yMax, &zMax, pMax);

  //  test each cell
  for (int i = xMin; i <= xMax; i++) {
    for (int j = yMin; j <= yMax; j++) {
      for (int k = zMin; k <= zMax; k++) {
        Point3D pMin, pMax;
        getCellBounds(&pMin, &pMax, i, j, k);
        if (s.intersect(pMin, pMax))
          getCell(i, j, k)->addItem() = index;
      }
    }
  }
}

Array<int> *SpacialHash::getCell(const Point3D &p) const {
  int x = (p.x - pMin.x) / d;
  int y = (p.y - pMin.y) / d;
  int z = (p.z - pMin.z) / d;
  return getCell(x, y, z);
}
