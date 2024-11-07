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

/*
    Sphere Generator which places spheres on a rigid grid - essentially a less
   restrictive version of the octree algorithm.
*/
#ifndef _API_RIGID_GRID_H_
#define _API_RIGID_GRID_H_

#include "SRBase.h"
#include "SEBase.h"
#include "../Geometry/Transform3D.h"

class SRGrid : public SRBase {
public:
  //  parameters
  SEBase *sphereEval;
  bool optimise, useQuickTest, pickMostSph;

  //  constructor
  SRGrid();
  void setupForLevel(int level, int degree,
                     const SurfaceRep *surRep = NULL) const {}

  //  get spheres
  void getSpheres(Array<Sphere> *spheres, int n, const SurfaceRep &surRep,
                  const Sphere *filterSphere = NULL,
                  float parSphErr = -1) const;

  //  OBB computation
  static void computeOBB(Vector3D v[3], const SurfaceRep &surRep);

private:
  //  internal
  static void makeRotation(Transform3D *tr, double vals[]);
  static void makeTransform(Transform3D *tr, const Point3D &pC, double vals[]);
  static void getOrientation(double vals[3], const SurfaceRep &surRep);
  static double computeVolume(Vector3D v[3], const SurfaceRep &surRep);

  static void convert(double vals[3], const Vector3D v[3]);
  static void getBounds(Point3D *pMin, Point3D *pMax, const SurfaceRep &surRep,
                        const Transform3D &tr);
  static void getGridDimensions(int dim[3], Point3D *cMin, const Point3D &pMin,
                                const Point3D &pMax, const Point3D &pCcube,
                                double size);
  static void generateSphereSet(Array<Sphere> *sph, const Point3D &cMin,
                                int dim[3], const Transform3D &tr, double size);
  static void generateSpheresCover(Array<Sphere> *spheres,
                                   const SurfaceRep &surRep,
                                   const Transform3D &tr, const Point3D &pCcube,
                                   double size);

  //  optimiser function - should really do better
  struct OptInfo {
    Point3D pCrot; //  center of rotation
    double size;   //  size of the cells
    const SurfaceRep *surRep;
    const SRGrid *that;
    int limit;
  };
  static double sphereNumberFunc(double vals[], void *data, int *canFinish);
  static double sphereFitFunc(double vals[], void *data, int *canFinish);
  double evalFitCover(const SurfaceRep &surRep, const Transform3D &tr,
                      const Point3D &pC, float size, int *numReqSph = NULL,
                      int maxNum = -1) const;
  double evalFit(const Array<Sphere> &sph) const;
  static void optimiseForError(double vals[6], OptInfo *optInfo);
  static void getGridTransform(double vals[6], double initOri[3],
                               OptInfo *optInf);
  static void generateSpheres(Array<Sphere> *spheres, const SurfaceRep &surRep,
                              double vals[10], const OptInfo &optInfo);
  double generateSpheresAndEval(Array<Sphere> *spheres,
                                const SurfaceRep &surRep, double vals[10],
                                const OptInfo &optInfo) const;

  //  optimiser func for minimising OBB volumn
  struct OBBOptInfo {
    const SurfaceRep *surRep;
  };
  static double obbVolFunc(double vals[], void *data, int *canFinish);
};

#endif