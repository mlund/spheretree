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

#ifndef _SPACIAL_HASH_H_
#define _SPACIAL_HASH_H_

#include "../Storage/Array.h"
#include "../Geometry/Point3D.h"
#include "Surface.h"

class SpacialHash {
public:
private:
  Array<int> *cells;
  Point3D pMin;   //  minimum point in the grid
  float d;        //  size of the cubes
  int nX, nY, nZ; //  number of cells in each dimension

public:
  SpacialHash();
  ~SpacialHash();

  //  setup
  void setup(const Point3D &pMin, const Point3D &pMax, int dv);

  //  add elements
  void addPoint(int i, const Point3D &p);
  void addTriangle(const Surface &sur, int index);
  void addSphere(const Sphere &s, int index);

  //  GET
  __inline int getDimX() const { return nX; }

  __inline int getDimY() const { return nY; }

  __inline int getDimZ() const { return nZ; }

  __inline int getNumCells() const { return nX * nY * nZ; }

  __inline Array<int> *getCell(int i) const { return &cells[i]; }

  __inline Array<int> *getCell(int x, int y, int z) const {
    CHECK_DEBUG0(x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ);

    return &cells[x * nY * nZ + y * nZ + z];
  }

  __inline void getMin(Point3D *pMin) const { *pMin = this->pMin; }

  __inline void getSize(float *pX, float *pY, float *pZ) const {
    *pX = this->d;
    *pY = this->d;
    *pZ = this->d;
  }

  __inline float getSize() const { return d; }

  struct CellRec {
    int i, j, k;
    float d; //  square distance from point
  };
  int getCells(CellRec **rec, const Point3D &p, bool ordered = false) const;

  Array<int> *findCell(int index);
  void getBoundedIndices(int *i, int *j, int *k, const Point3D &p) const;
  void getIndices(int *i, int *j, int *k, const Point3D &p) const;

  //  cell info
  void getCellBounds(Point3D *pMin, Point3D *pMax, int i, int j, int k) const;
  void getCellCenter(Point3D *pC, int i, int j, int k) const;

  //  info about the grid
  float getDistanceSQR(int i, int j, int k, const Point3D &p) const;

  //  discrete stepper
  class Stepper {
  public:
    const SpacialHash *sh;

    Stepper() { sh = NULL; };
    Array<int> *discreteSetup(const Point3D &rayOrigin, const Vector3D &rayDir);
    Array<int> *discreteStep();

  private:
    int pos[3], dir[3];
    double offset[3], inc[3], out[3];
    bool BoxIntersect(const Point3D &rayOrigin, const Vector3D &rayDirn,
                      double entry[3]) const;
  };

private:
  Array<int> *getCell(const Point3D &p) const;
  static int recCompare(const void *elem1, const void *elem2);

  friend class Stepper;
};

#endif