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

#include "SurfaceRep.h"

void SurfaceRep::setup(const Array<Surface::Point> &pts, int divs){
  if (divs < 1)
    divs = 1;

  surPts = &pts;
  int numPts = pts.getSize();

  //  get bounds
  Point3D pMin = Point3D::MAX;
  Point3D pMax = Point3D::MIN;
  for (int i = 0; i < numPts; i++){
    Point3D p = pts.index(i).p;
    pMin.storeMin(p);
    pMax.storeMax(p);
    }

  //  setup spacial hash
  Vector3D v;
  Point3D pMin1, pMax1;
  v.difference(pMax, pMin);
  v.add(&pMin1, pMin, -0.1f);
  v.add(&pMax1, pMax, 0.1f);

  sh.setup(pMin1, pMax1, divs);     //  good resolution would depend on the number of points
  for (int i = 0; i < numPts; i++)
    sh.addPoint(i, pts.index(i).p);
}

int SurfaceRep::flagContainedPoints(Array<bool> *flags, const Sphere &s, const Array<bool> *banFlags) const{
  //  make points of sphere's bounding box
  Point3D pMin, pMax;
  pMin.x = s.c.x - s.r;
  pMin.y = s.c.y - s.r;
  pMin.z = s.c.z - s.r;
  pMax.x = s.c.x + s.r;
  pMax.y = s.c.y + s.r;
  pMax.z = s.c.z + s.r;

  //  get cells from the SEADS grid
  int iMin, iMax, jMin, jMax, kMin, kMax;
  sh.getBoundedIndices(&iMin, &jMin, &kMin, pMin);
  sh.getBoundedIndices(&iMax, &jMax, &kMax, pMax);

  int total = 0;
  for (int i = iMin; i <= iMax; i++){
    for (int j = jMin; j <= jMax; j++){
      for (int k = kMin; k <= kMax; k++){
        total += processCell(flags, s, i, j, k, banFlags);
        }
      }
    }

  return total;
}

void SurfaceRep::listContainedPoints(Array<int> *list, Array<bool> *setFlags, const Sphere &s, const Array<bool> *banFlags) const{
  //  make points of sphere's bounding box
  Point3D pMin, pMax;
  pMin.x = s.c.x - s.r;
  pMin.y = s.c.y - s.r;
  pMin.z = s.c.z - s.r;
  pMax.x = s.c.x + s.r;
  pMax.y = s.c.y + s.r;
  pMax.z = s.c.z + s.r;

  //  get cells from the SEADS grid
  int iMin, iMax, jMin, jMax, kMin, kMax;
  sh.getBoundedIndices(&iMin, &jMin, &kMin, pMin);
  sh.getBoundedIndices(&iMax, &jMax, &kMax, pMax);

  list->setSize(0);
  for (int i = iMin; i <= iMax; i++){
    for (int j = jMin; j <= jMax; j++){
      for (int k = kMin; k <= kMax; k++){
        processCell(list, setFlags, s, i, j, k, banFlags);
        }
      }
    }
}

int SurfaceRep::processCell(Array<bool> *setFlags, const Sphere &s, int i, int j, int k, const Array<bool> *banFlags) const{
  //  process the points in the cell
  Array<int> *cell = sh.getCell(i, j, k);
  int numPts = cell->getSize();
  if (numPts == 0)
    return 0;

  int status = 2;
  if (numPts > 3)
    status = getCellStatus(s, i, j, k, numPts);

  int count = 0;
  if (status == 1){
    //  totally inside - set the flags  (take if out of the loop)
    if (!setFlags && !banFlags){  //  neither list
      count += numPts;
      }
    else if (!banFlags){          //  just have set list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        setFlags->index(pI) = true;
        count++;
        }
      }
    else if (!setFlags){          //  just have ban list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI))
          count++;
        }
      }
    else{                         //  have both lists
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI)){
          setFlags->index(pI) = true;
          count++;
          }
        }
      }
    }
  else if (status == 2){
    //  partially inside - test the points
    if (!setFlags && !banFlags){  //  neither list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (s.contains(surPts->index(pI).p))
          count++;
        }
      }
    else if (!banFlags){          //  just have set list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (s.contains(surPts->index(pI).p)){
          setFlags->index(pI) = true;
          count++;
          }
        }
      }
    else if (!setFlags){          //  just have ban list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI) && s.contains(surPts->index(pI).p))
          count++;
        }
      }
    else{                         //  have both lists
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI) && s.contains(surPts->index(pI).p)){
          setFlags->index(pI) = true;
          count++;
          }
        }
      }
    }

  return count;
}

void SurfaceRep::processCell(Array<int> *list, Array<bool> *setFlags, const Sphere &s, int i, int j, int k, const Array<bool> *banFlags) const{
  //  process the points in the cell
  Array<int> *cell = sh.getCell(i, j, k);
  int numPts = cell->getSize();
  if (numPts == 0)
    return;

  int status = 2;
  if (numPts > 3)
    status = getCellStatus(s, i, j, k, numPts);

  if (status == 1){
    //  totally inside - set the flags  (take if out of the loop)
    if (!setFlags && !banFlags){  //  neither list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        list->addItem() = pI;
        }
      }
    else if (!banFlags){          //  just have set list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        list->addItem() = pI;
        setFlags->index(pI) = true;
        }
      }
    else if (!setFlags){          //  just have ban list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI))
          list->addItem() = pI;
        }
      }
    else{                         //  have both lists
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI)){
          list->addItem() = pI;
          setFlags->index(pI) = true;
          }
        }
      }
    }
  else if (status == 2){
    //  partially inside - test the points
    if (!setFlags && !banFlags){  //  neither list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (s.contains(surPts->index(pI).p))
          list->addItem() = pI;
        }
      }
    else if (!banFlags){          //  just have set list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (s.contains(surPts->index(pI).p)){
          list->addItem() = pI;
          setFlags->index(pI) = true;
          }
        }
      }
    else if (!setFlags){          //  just have ban list
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI) && s.contains(surPts->index(pI).p))
          list->addItem() = pI;
        }
      }
    else{                         //  have both lists
      for (int l = 0; l < numPts; l++){
        int pI = cell->index(l);
        if (!banFlags->index(pI) && s.contains(surPts->index(pI).p)){
          list->addItem() = pI;
          setFlags->index(pI) = true;
          }
        }
      }
    }
}

//  0 == totally outside
//  1 == totally inside
//  2 == partially inside
int SurfaceRep::getCellStatus(const Sphere &s, int i, int j, int k, int numPts) const{
  //  check if cell is outside the sphere
  float rS = s.r*s.r;
  float dist = sh.getDistanceSQR(i, j, k, s.c);
  if (dist > rS)
    return 0;

  //  quick test for cell completely inside sphere
  float d = sh.getSize();
  if (sqrt(dist) < sqrt(rS) - sqrt(d*d*3))
    return 1;

  if (numPts < 12)
    return 2;

  //  get cell info
  Point3D pts[2], pT;
  sh.getCellBounds(&pts[0], &pts[1], i, j, k);

  //  check vertices
  for (int a = 0; a < 2; a++){
    pT.x = pts[a].x;
    for (int b = 0; b < 2; b++){
      pT.y = pts[b].y;
      for (int c = 0; c < 2; c++){
        pT.z = pts[c].z;

        if (!s.contains(pT))
          return 2;
        }
      }
    }

  //  corners are inside - must be total
  return 1;
}
