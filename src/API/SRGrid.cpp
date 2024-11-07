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

#include "SRGrid.h"
#include "RELargest.h"
#include "../BBox/BBox.h"
#include "../Geometry/Angles.h"
#include "../MinMax/Simplex.h"

SRGrid::SRGrid() {
  sphereEval = NULL;
  optimise = true;
  useQuickTest = false;
  pickMostSph = false;
}

void SRGrid::getSpheres(Array<Sphere> *spheres, int numDest,
                        const SurfaceRep &surRep, const Sphere *filterSphere,
                        float parSphErr) const {
  // CHECK_DEBUG0(numDest >= 8);
  CHECK_DEBUG0(sphereEval != NULL);

  /*  generate config guarantees coverage with 8 spheres */

  //  get the initial orientation
  double initOri[3] = {0, 0, 0};
  getOrientation(initOri, surRep);

  //  get the transform to be applied to the surface points
  Transform3D tR;
  makeRotation(&tR, initOri);

  //  get the bounding box of the transformed points
  //  and work out the size of the bounding cube
  Point3D pMin, pMax;
  getBounds(&pMin, &pMax, surRep, tR);
  double sX = pMax.x - pMin.x;
  double sY = pMax.y - pMin.y;
  double sZ = pMax.z - pMin.z;
  double d = sX;
  if (sY > d)
    d = sY;
  if (sZ > d)
    d = sZ;

  double initSize = d / 2.0f;
  if (numDest < 8)
    initSize = d;

  //  get center of one of the cubes
  Point3D pC;
  pC.x = (pMin.x + pMax.x) / 2.0f;
  pC.y = (pMin.y + pMax.y) / 2.0f;
  pC.z = (pMin.z + pMax.z) / 2.0f;

  //  transform center back to its proper position
  Point3D pCI;
  Transform3D trI;
  trI.invert(tR);
  trI.transform(&pCI, pC);

  //  setup optimiser parameters
  OptInfo optInfo;
  optInfo.pCrot = pCI;
  optInfo.surRep = &surRep;
  optInfo.limit = 1;
  optInfo.that = this;

  //  work variables
  Array<Sphere> sph;
  double curVals[10];

  //  get starting transform
  if (filterSphere && filterSphere->r > 0)
    initSize = filterSphere->r;

  optInfo.size = initSize;
  getGridTransform(curVals, initOri, &optInfo);

  //  get starting count of spheres
  double bestErr = generateSpheresAndEval(spheres, surRep, curVals, optInfo);

  if (!useQuickTest) {
    double lastVals[10], lastSize;
    int lastNum = 0, curNum = 0; //  need 0 for limit

    //  iteratively reduce the size of the grid
    while (optInfo.size > 0.1 * initSize && curNum <= numDest) {
      //  find the smallest grid to give the same number of spheres as curNum
      do {
        //  copy current info into last info
        memcpy(lastVals, curVals, 6 * sizeof(curVals[0]));
        lastNum = curNum;
        lastSize = optInfo.size;

        //  reduce size
        optInfo.size -= 0.05 * initSize;

        //  make transform
        optInfo.limit = lastNum;
        getGridTransform(curVals, initOri, &optInfo);

        //  get the spheres
        generateSpheres(&sph, surRep, curVals, optInfo);
        curNum = sph.getSize();

        OUTPUTINFO("*************************\n");
        OUTPUTINFO("SIZE = %f\n", optInfo.size);
        OUTPUTINFO("COUNT = %d\n", curNum);

      } while (curNum <= lastNum);

      //  optimise the results for fit
      double curSize = optInfo.size;
      optInfo.size = lastSize;
      if (optimise) {
        OUTPUTINFO("Optimising Fit %d\n", lastNum);
        optInfo.limit = lastNum;
        lastVals[6] = 1;
        optimiseForError(lastVals, &optInfo);
        optInfo.size *= lastVals[6];
      }

      //  evaluate error and record if best fit sofar
      double err = generateSpheresAndEval(&sph, surRep, lastVals, optInfo);
      OUTPUTINFO("Err = %f\n", err);

      if (err < bestErr ||
          (pickMostSph &&
           sph.getSize() >=
               spheres->getSize())) { //  number will be <= max allowable
        OUTPUTINFO("######################\nNEW BEST\n");
        OUTPUTINFO("Size = %f\n", optInfo.size);
        OUTPUTINFO("Num = %d\n", sph.getSize());

        bestErr = err;
        spheres->clone(sph);
      }

      //  return optInfo to old state
      optInfo.size = curSize;
    }
  } else {
    //  do smallest spheres
    optInfo.limit = numDest;
    double bestVals[10], bestSize = optInfo.size;
    memcpy(bestVals, curVals, 6 * sizeof(curVals[0]));

    while (optInfo.size > 0.1 * initSize) {
      //  reduce size
      optInfo.size -= 0.05 * initSize;

      //  make transform
      getGridTransform(curVals, initOri, &optInfo);

      //  get the spheres
      generateSpheres(&sph, surRep, curVals, optInfo);
      int count = sph.getSize();

      OUTPUTINFO("*************************\n");
      OUTPUTINFO("SIZE = %f\n", optInfo.size);
      OUTPUTINFO("COUNT = %d\n", count);

      if (count > numDest)
        break;
      else {
        double err = evalFit(sph);
        OUTPUTINFO("Error = %f\n", err);

        if (err < bestErr ||
            (pickMostSph &&
             sph.getSize() >=
                 spheres->getSize())) { //  number will be <= max allowable
          OUTPUTINFO("New Best : %d (%f)\n", count, err);
          bestErr = err;
          bestSize = optInfo.size;
          memcpy(bestVals, curVals, 6 * sizeof(curVals[0]));
        }
      }
    }

    //  optimise the results for fit
    bestVals[6] = 1;
    optInfo.limit = numDest;
    optInfo.size = bestSize;

    if (optimise) {
      OUTPUTINFO("Optimising set\n");
      optimiseForError(bestVals, &optInfo);
      optInfo.size *= bestVals[6];
    }

    //  evaluate error and record if best fit sofar
    generateSpheres(spheres, surRep, bestVals, optInfo);
    double err = evalFit(*spheres);
    OUTPUTINFO("Results : \n");
    OUTPUTINFO("     Err = %f\n", err);
    OUTPUTINFO("     Num = %d\n", spheres->getSize());
  }
}

/*
    generate the optimised transform
*/
void SRGrid::getGridTransform(double vals[6], double initOri[3],
                              OptInfo *optInf) {
  //  dimensions of simplex
  double sizes[10] = {0};
  sizes[0] = sizes[1] = sizes[2] = 10;
  sizes[3] = sizes[4] = sizes[5] = 0.66 * (optInf->size);

  //  initial values for optimisation
  vals[0] = initOri[0];
  vals[1] = initOri[1];
  vals[2] = initOri[2];
  vals[3] = vals[4] = vals[5] = 0;

  //  generate optimised set of spheres
  int maxFunc = 100;
  simplexMin(6, vals, sizes, EPSILON_LARGE, &maxFunc, sphereNumberFunc, optInf,
             5);
}

/*
    optimise the values for orientation, position and scale
    to reduce the error of the set of spheres
*/
void SRGrid::optimiseForError(double vals[10], OptInfo *optInfo) {
  //  dimensions of simplex
  double sizes[10] = {0};
  sizes[0] = sizes[1] = sizes[2] = 10;
  sizes[3] = sizes[4] = sizes[5] = 0.66 * optInfo->size;
  sizes[6] = -0.1;
  vals[6] = 1.0f;

  int maxFunc = 100;
  simplexMin(7, vals, sizes, EPSILON_LARGE, &maxFunc, sphereFitFunc, optInfo,
             5);
}

/*
    bits for the grid's transform
*/
void SRGrid::getOrientation(double vals[3], const SurfaceRep &surRep) {
  //  get the axes of the bounding box using Principle Component Analysis
  Vector3D v[3];
  computeOBB(v, surRep);

  //  convert
  convert(vals, v);
}

void SRGrid::convert(double vals[3], const Vector3D v[3]) {
  //  make the transform to align these basis vectors with the basis vectors
  //  of the world co-ordinate space
  Vector3D zAxis = v[2]; //  align Z axes
  double r = sqrt(zAxis.x * zAxis.x + zAxis.z * zAxis.z);
  double aX = DEG(atan2(zAxis.y, r));        //  angle of rotation about X axis
  double aY = -DEG(atan2(zAxis.x, zAxis.z)); //  angle of rotation about Y axis

  //  make transform
  Transform3D tr;
  vals[0] = aX;
  vals[1] = aY;
  vals[2] = 0;
  makeRotation(&tr, vals);

  //  work out rotation about Z axis
  Vector3D xAxis;
  tr.transform(&xAxis, v[0]);
  double aZ = -DEG(atan2(xAxis.y, xAxis.x)); //  angle of rotation about Z axis
  vals[0] = aX;
  vals[1] = aY;
  vals[2] = aZ;
}

/*
    Build a rotation matrix from the angles contained in vals
    X, Y, Z angles in degrees
*/
void SRGrid::makeRotation(Transform3D *tr, double vals[]) {
  //  make rotation transform
  Transform3D trX, trY, trZ;
  trX.identity();
  trX.setAngleX(vals[0]);
  trY.identity();
  trY.setAngleY(vals[1]);
  trZ.identity();
  trZ.setAngleZ(vals[2]);

  trY.compose(trX);
  trY.compose(trZ);
  tr->assign(trY);
}

/*
    Build the rotation (about pC) and translations from vals
    X, Y, Z, dX, dY, dZ
*/
void SRGrid::makeTransform(Transform3D *tr, const Point3D &pC, double vals[]) {
  Transform3D trR;
  makeRotation(&trR, vals);

  //  make translation transforms for rotating about pC
  Transform3D trO, trT;
  trO.identity();
  trO.setTranslate(-pC.x, -pC.y, -pC.z);
  trT.identity();
  trT.setTranslate(pC.x, pC.y, pC.z);

  //  compose transforms;
  trO.compose(trR);
  trO.compose(trT);
  tr->assign(trO);

  //  offset vector
  tr->index(0, 3) += vals[3];
  tr->index(1, 3) += vals[4];
  tr->index(2, 3) += vals[5];
}

/*
  Generate a cube which covers the set of transformed surface points
*/
void SRGrid::getBounds(Point3D *pMin, Point3D *pMax, const SurfaceRep &surRep,
                       const Transform3D &tr) {
  //  setup bounds
  (*pMin) = Point3D::MAX;
  (*pMax) = Point3D::MIN;

  const Array<Surface::Point> *surPts = surRep.getSurPts();

  //  get bounds of transformed points
  int numPts = surPts->getSize();
  for (int i = 0; i < numPts; i++) {
    Point3D pT;
    tr.transform(&pT, surPts->index(i).p);

    pMin->storeMin(pT);
    pMax->storeMax(pT);
  }
}

/*
    Generate a set of spheres to cover the surface points
    using the given orientation etc.

    pCcube is the center of one of the cubes - not the center of the bounding
   cube cMin is the center of the first cube in the grid
*/
void SRGrid::getGridDimensions(int dim[3], Point3D *cMin, const Point3D &pMin,
                               const Point3D &pMax, const Point3D &pCcube,
                               double size) {
  //  concervative coverage
  int numX1 = ceil((pCcube.x - pMin.x) / size);
  int numX2 = ceil((pMax.x - pCcube.x) / size);
  int numY1 = ceil((pCcube.y - pMin.y) / size);
  int numY2 = ceil((pMax.y - pCcube.y) / size);
  int numZ1 = ceil((pCcube.z - pMin.z) / size);
  int numZ2 = ceil((pMax.z - pCcube.z) / size);
  dim[0] = numX1 + numX2 + 1;
  dim[1] = numY1 + numY2 + 1;
  dim[2] = numZ1 + numZ2 + 1;

  //  work out the position of the first sphere
  cMin->x = pCcube.x - size * numX1;
  cMin->y = pCcube.y - size * numY1;
  cMin->z = pCcube.z - size * numZ1;
}

void SRGrid::generateSphereSet(Array<Sphere> *sph, const Point3D &cMin,
                               int dim[3], const Transform3D &tr, double size) {
  //  generate spheres and transform back to model space
  Transform3D trI;
  trI.invert(tr);
  double radius = sqrt(3.0f * size * size / 4.0f);

  sph->setSize(0);
  for (int x = 0; x < dim[0]; x++) {
    double X = cMin.x + x * size;
    for (int y = 0; y < dim[1]; y++) {
      double Y = cMin.y + y * size;
      for (int z = 0; z < dim[2]; z++) {
        double Z = cMin.z + z * size;

        Point3D cT, c = {X, Y, Z};
        trI.transform(&cT, c);

        Sphere *s = &sph->addItem();
        s->c = cT;
        s->r = radius;
      }
    }
  }
}

void SRGrid::generateSpheresCover(Array<Sphere> *spheres,
                                  const SurfaceRep &surRep,
                                  const Transform3D &tr, const Point3D &pCcube,
                                  double size) {
  //  get bounds of transformed points
  Point3D pMin, pMax;
  getBounds(&pMin, &pMax, surRep, tr);

  //  and the number of spheres in each dimension & position of first sphere
  int dim[3];
  Point3D cMin;
  getGridDimensions(dim, &cMin, pMin, pMax, pCcube, size);

  //  generate the spheres
  generateSphereSet(spheres, cMin, dim, tr, size);
}

void SRGrid::generateSpheres(Array<Sphere> *spheres, const SurfaceRep &surRep,
                             double vals[10], const OptInfo &optInfo) {
  //  generate transform
  Transform3D tR;
  makeTransform(&tR, optInfo.pCrot, vals);

  //  make spheres
  Point3D pCcube;
  pCcube.x = optInfo.pCrot.x - optInfo.size / 2;
  pCcube.y = optInfo.pCrot.y - optInfo.size / 2;
  pCcube.z = optInfo.pCrot.z - optInfo.size / 2;

  //  generate the set of spheres
  Array<Sphere> tmpSph;
  generateSpheresCover(&tmpSph, surRep, tR, pCcube, optInfo.size);

  //  elimate the dead spheres
  RELargest elimin;
  elimin.reduceSpheres(spheres, tmpSph, surRep, -1);
}

double SRGrid::generateSpheresAndEval(Array<Sphere> *spheres,
                                      const SurfaceRep &surRep, double vals[10],
                                      const OptInfo &optInfo) const {
  //  generate transform
  Transform3D tR;
  makeTransform(&tR, optInfo.pCrot, vals);

  //  make spheres
  Point3D pCcube;
  pCcube.x = optInfo.pCrot.x - optInfo.size / 2;
  pCcube.y = optInfo.pCrot.y - optInfo.size / 2;
  pCcube.z = optInfo.pCrot.z - optInfo.size / 2;

  //  generate the set of spheres
  Array<Sphere> tmpSph;
  generateSpheresCover(&tmpSph, surRep, tR, pCcube, optInfo.size);

  //  elimate the dead spheres
  RELargest elimin;
  elimin.reduceSpheres(spheres, tmpSph, surRep, -1);

  return evalFit(*spheres);
}

/*
    optimiser function to get the minimum number of sphere to cover the surface
   points
*/
double SRGrid::sphereNumberFunc(double vals[], void *data, int *canFinish) {
  const SRGrid::OptInfo *optInfo = (OptInfo *)data;

  //  generate transform
  Transform3D tR;
  makeTransform(&tR, optInfo->pCrot, vals);

  //  make spheres
  Point3D pCcube;
  pCcube.x = optInfo->pCrot.x - optInfo->size / 2;
  pCcube.y = optInfo->pCrot.y - optInfo->size / 2;
  pCcube.z = optInfo->pCrot.z - optInfo->size / 2;

  Array<Sphere> trSph;
  generateSpheresCover(&trSph, *optInfo->surRep, tR, pCcube, optInfo->size);

  //  attribute points to spheres
  Array<int> counts;
  Array<Sphere> destSph;

  RELargest elimin;
  elimin.reduceSpheres(&destSph, trSph, *optInfo->surRep, -1, &counts);

  //  compute coverage metric
  long metric = 0, count = 0;
  int numSph = counts.getSize();
  for (int i = 0; i < numSph; i++) {
    int num = counts.index(i);
    if (num > 0) {
      metric += num * num;
      count++;
    }
  }

  if (destSph.getSize() <= optInfo->limit)
    *canFinish = 1;

  return -metric / (count * count);
}

/*
    evaluate the given solution for error
*/
double SRGrid::evalFit(const Array<Sphere> &sph) const {
  float worstErr = 0;
  int numSph = sph.getSize();
  for (int i = 0; i < numSph; i++) {
    float err = sphereEval->evalSphere(sph.index(i));
    if (err > worstErr)
      worstErr = err;
  }

  return worstErr;
}

double SRGrid::evalFitCover(const SurfaceRep &surRep, const Transform3D &tr,
                            const Point3D &pCcube, float size, int *numReqSph,
                            int maxNum) const {
  //  generate the set of spheres
  Array<Sphere> sph1, sph2;
  generateSpheresCover(&sph1, surRep, tr, pCcube, size);

  //  elimate the dead spheres
  RELargest elimin;
  elimin.reduceSpheres(&sph2, sph1, surRep, -1);
  int numSph = sph2.getSize();
  if (numReqSph)
    *numReqSph = numSph;
  if (numSph > maxNum && maxNum > 0)
    return REAL_MAX;

  return evalFit(sph2);
}

/*
    optimisation function to improve fit once we have the rough layout of the
   grid
*/
double SRGrid::sphereFitFunc(double vals[], void *data, int *canFinish) {
  if (vals[6] < 0.0f || vals[6] > 1.0f)
    return REAL_MAX;

  SRGrid::OptInfo *optInfo = (OptInfo *)data;

  double initSize = optInfo->size;
  optInfo->size *= vals[6];

  Array<Sphere> sph;
  double err = optInfo->that->generateSpheresAndEval(&sph, *optInfo->surRep,
                                                     vals, *optInfo);

  optInfo->size = initSize;
  if (sph.getSize() > optInfo->limit)
    err = REAL_MAX;

  return err;
}

/*
    OBB computation
*/
double SRGrid::computeVolume(Vector3D v[3], const SurfaceRep &surRep) {
  const Array<Surface::Point> *pts = surRep.getSurPts();

  int numPts = pts->getSize();
  if (numPts == 0)
    return 0;

  double minT[3] = {0, 0, 0};
  double maxT[3] = {0, 0, 0};
  Point3D q = pts->index(0).p;
  for (int i = 1; i < numPts; i++) {
    Vector3D n;
    n.difference(pts->index(i).p, q);

    for (int j = 0; j < 3; j++) {
      double t = v[j].dot(n);
      if (t > maxT[j])
        maxT[j] = t;
      if (t < minT[j])
        minT[j] = t;
    }
  }

  double l0 = maxT[0] - minT[0];
  double l1 = maxT[1] - minT[1];
  double l2 = maxT[2] - minT[2];

  return l0 * l1 * l2;
}

void SRGrid::computeOBB(Vector3D v[3], const SurfaceRep &surRep) {
  const Array<Surface::Point> *pts = surRep.getSurPts();

  //  get obb
  computeOBBinert(v, *pts);

  //  convert to angles
  double vals[3];
  convert(vals, v);

  // do optimise
  double sizes[3] = {10, 10, 10};
  OBBOptInfo inf;
  inf.surRep = &surRep;
  int maxFunc = 100;
  simplexMin(3, vals, sizes, EPSILON_LARGE, &maxFunc, obbVolFunc, &inf, 4);

  //  make transform
  Transform3D tr;
  makeRotation(&tr, vals);
  Transform3D trI;
  trI.invertAffine(tr);

  //  transform basis vectors
  trI.transform(&v[0], Vector3D::X);
  trI.transform(&v[1], Vector3D::Y);
  trI.transform(&v[2], Vector3D::Z);
}

//  optimiser func for minimising OBB volumn
double SRGrid::obbVolFunc(double vals[], void *data, int *canFinish) {
  OBBOptInfo *inf = (OBBOptInfo *)data;

  //  make transform
  Transform3D tr;
  makeRotation(&tr, vals);
  Transform3D trI;
  trI.invertAffine(tr);

  //  transform basis vectors
  Vector3D v[3];
  trI.transform(&v[0], Vector3D::X);
  trI.transform(&v[1], Vector3D::Y);
  trI.transform(&v[2], Vector3D::Z);
  double vol = computeVolume(v, *inf->surRep);

  return vol;
}
