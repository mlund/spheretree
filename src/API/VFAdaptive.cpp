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

#include "VFAdaptive.h"
#include "SEConvex.h"
#include "../Export/POV.h"

//  constructor
VFAdaptive::VFAdaptive(){
  mt = NULL;
  eval = NULL;
  coverRep = NULL;
}

void VFAdaptive::adaptiveSample(Voronoi3D *vor, float maxErr, int maxSam, int maxSph, int minSph, const SurfaceRep *coverRep, const Sphere *filterSphere, bool countOnlyCoverSpheres, int maxLoop){
  CHECK_DEBUG0(mt != NULL);
  const SurfaceRep *rep = coverRep;
  if (rep == NULL)
    rep = this->coverRep;

  VFAdaptive::adaptiveSample(vor, *mt, rep, maxErr, maxSam, maxSph, minSph, filterSphere, eval, countOnlyCoverSpheres, maxLoop);
}

double VFAdaptive::getErr(Voronoi3D::Vertex *vert, const SEBase *eval, const MedialTester *mt, const Voronoi3D *vor){
  if (vert->err < 0){
    if (!eval){                 //  no evaluator - use convex approx
      //  get closest point
      Point3D pClose;
      mt->closestPoint(&pClose, vert);

      if (vert->flag == VOR_FLAG_UNKNOWN)
        OUTPUTINFO("GetErr : UNCATEGORISED POINT\n");

      //  evaluate fit
      float dSur = pClose.distance(vert->s.c);
      if (vert->flag == VOR_FLAG_COVER){
        vert->err = vert->s.r + dSur;    //  sphere center is outside surface 
        }
      else{
        vert->err = vert->s.r - dSur;    //  sphere center is inside surface
        if (vert->err < 0)
          vert->err = 0;
        }
      }
    else{
      vert->err = eval->evalSphere(vert->s);
      }
    }

  return vert->err;
}

int VFAdaptive::findWorstSphere(Voronoi3D *vor, const MedialTester &mt, const SEBase *eval, const Sphere *filterSphere, float *resultErr, int *numMedial, bool includeCover){
  int worstI = -1;
  float errSofar = -1;
  int numInternal = 0;

  //  find the worst approximated point
  int numVerts = vor->vertices.getSize();
  for (int i = 0; i < numVerts; i++){
    Voronoi3D::Vertex *v = &vor->vertices.index(i);
    if (filterSphere)
      if (!filterSphere->overlap(v->s))
        continue;

    if (v->flag == VOR_FLAG_INTERNAL || (includeCover && v->flag == VOR_FLAG_COVER)){
      numInternal++;
      float err = getErr(v, eval, &mt, vor);
      if (err > errSofar){
        errSofar = err;
        worstI = i;
        }
      }
    }

  if (resultErr)
    *resultErr = errSofar;
  if (numMedial)
    *numMedial = numInternal;

  return worstI;
}

void saveSpheres(Voronoi3D *vor, const MedialTester &mt, int sel){
  //  selected sphere is the one about to be replaced
  Array<int> selList, selList1;

  //  make list of spheres
  Array<Sphere> spheres;
  int numVert = vor->vertices.getSize();
  for (int i = 0; i < numVert; i++){
    Voronoi3D::Vertex *vert = &vor->vertices.index(i);
    int isNew = vert->flag == VOR_FLAG_UNKNOWN;

    if (mt.isMedial(vert)/* && (vert->flag != VOR_FLAG_COVER || i == sel)*/){
      spheres.addItem() = vert->s;

      //  stuff for displaying which sphere is about to be removed
      if (sel < 0){
	      if (isNew)
	        selList.addItem() = spheres.getSize()-1;
      }
      else if (i == sel){
	      selList.addItem() = spheres.getSize()-1;
	      }
      if (vert->flag == VOR_FLAG_COVER)
        selList1.addItem() = spheres.getSize()-1;
    }
  }

  if (selList.getSize() == 0)
    printf("NON SELECTED\n");
  else
    printf("%d SELECTED\n", selList.getSize());

  if (spheres.getSize() > 0){
    //  export scene for rendering
    static int fn = 0;
    char buffer[1024];
    sprintf(buffer, "povs/adapt-%0.6d.pov", fn++);
    printf("Saving as : %s\n");

    float selColor[3] = {1,0,0};
    float selColor1[3] = {0,1,0};
    float selColor2[3] = {1, 1, 0};
    exportSpheresPOV(buffer, spheres, 1.0/1000.0, true, NULL, &selList, sel < 0? selColor1:selColor, &selList1, selColor2);
  }
}

void VFAdaptive::adaptiveSample(Voronoi3D *vor, const MedialTester &mt, const SurfaceRep *coverRep, float maxErr, int maxSam, int maxSph, int minSph, const Sphere *filterSphere, const SEBase *eval, bool countOnlyCoverSpheres, int maxLoops){
  if (maxErr < 0.0f)
    return;

  //  find the worst approximated point
  float errSofar;
  findWorstSphere(vor, mt, eval, filterSphere, &errSofar, NULL, false);
  OUTPUTINFO("Initial Error Sofar : %f\n", errSofar);

  //  label the vertices
  int numCover = mt.processMedial(vor, coverRep, filterSphere, countOnlyCoverSpheres);

  //  work out maximum loops alowed
  if (maxLoops <= 0){
    if (maxSam > 0)
      maxLoops = maxSam*3;
    else if (maxSph)
      maxLoops = maxSph*3;
    }

  //  find the starting error
  int loop = 0;
  while (true){
    loop++;

    //  find the worst approximated point
    int numInt = 0;
    float worstD = -1;
    int worstI = findWorstSphere(vor, mt, eval, filterSphere, &worstD, &numInt, true);
    if (countOnlyCoverSpheres)
      numInt = numCover;
    if (worstI < 0){
      //  clear the ban flags and try again
      int numVert = vor->vertices.getSize();
      for (int i = 0; i < numVert; i++){
        Voronoi3D::Vertex *vert = &vor->vertices.index(i);
        if (filterSphere && !vert->s.overlap(*filterSphere))
          continue;
        vor->resetFlag(vert);
        }

      //  update medial/coverage spheres
      numCover = mt.processMedial(vor, coverRep, filterSphere);
      worstI = findWorstSphere(vor, mt, eval, filterSphere, &worstD, &numInt, true);
      if (worstI < 0)
          break;  //  nothing more we can do
      }

    //  enable this to output POV files
    //saveSpheres(vor, mt, worstI);

    if ((loop %25) ==0)
      OUTPUTINFO("NumSpheres : %4d  \tErr : %10.6f  \terrSofar : %10.6f\n", numInt, worstD, errSofar);

    if (errSofar < 0)                       //  first iteration to have some spheres
      errSofar = worstD;  

    //  is it worth continuing ?? 
    if (worstD < maxErr && numInt > minSph)
      break;
    else if ((maxSam > 0 && vor->formingPoints.getSize()-9 >= maxSam*2) ||
             (maxSph > 0 && numInt >= maxSph*2) ||
             (maxLoops > 0 && loop > maxLoops)){
        //  normally we would wait until we get back down to the correct error
        //  but this CAN mean the algorithm will NEVER terminate
        //  so guard against that happening

        //  remove BAD cover spheres i.e. those with error > minError sofar
        //  otherwise this will cause major problems for Merge/Burst
        int numVerts = vor->vertices.getSize();
        for (int i = 0; i < numVerts; i++){
          Voronoi3D::Vertex *v = &vor->vertices.index(i);
          if (v->flag == VOR_FLAG_COVER){
            float err = getErr(v, eval, &mt, vor);
            if (err > errSofar)
              v->flag = VOR_FLAG_EXTERNAL;
            }
          }

        break;
        }
    else if (worstD <= errSofar || worstD <= EPSILON_LARGE){
      if (worstD > EPSILON_LARGE)
        errSofar = worstD;
      else
        errSofar = EPSILON_LARGE;

      if ((maxSam > 0 && vor->formingPoints.getSize()-9 >= maxSam) ||
          (maxSph > 0 && numInt >= maxSph) ||
          (maxLoops > 0 && loop > maxLoops))
        break;
      }

    //  flag the vertex so that we cannot consider it in the future
    //  just in case the vertex isn't removed
    Voronoi3D::Vertex *v = &vor->vertices.index(worstI);
    mt.blockMedial(v);

    //  get the closest surface point to the vertex 
    Vector3D n;
    Point3D pClose;
    mt.closestPointNormal(&pClose, &n, v);

    //  check if the new point will improve the approximation
    if (pClose.distanceSQR(v->s.c) > v->s.r*v->s.r)
      OUTPUTINFO("WARNING : the closest point is further away than it should be\n");

    //  add the point to the Voronoi diagram
    vor->insertPoint(pClose, n, worstI);

    // save off the results of adding the new sphere
    //  uncomment this to save POV files
    //saveSpheres(vor, mt, -1);

    //  update medial/coverage spheres
    numCover = mt.processMedial(vor, coverRep, filterSphere);
    }
}
