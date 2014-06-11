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

#include "MedialTester.h"

MedialTester::MedialTester(){
  useLargeCover = true;
}

bool MedialTester::isMedial(Voronoi3D::Vertex *vert) const{
  int flag = getFlag(vert);
  //return (flag == VOR_FLAG_INTERNAL && insideBounds(vert->s.c)) || (flag == VOR_FLAG_COVER);
  return (flag == VOR_FLAG_INTERNAL || flag == VOR_FLAG_COVER);
}

char MedialTester::getFlag(Voronoi3D::Vertex *vert) const{
  if (vert->flag == VOR_FLAG_UNKNOWN)
    testInternal(vert);

  return vert->flag;
}

void MedialTester::blockMedial(Voronoi3D::Vertex *vert) const{
  if (vert->flag == VOR_FLAG_INTERNAL)
    vert->flag = VOR_FLAG_OLDINTERNAL;
  else if (vert->flag == VOR_FLAG_COVER)
    vert->flag = VOR_FLAG_OLDCOVER;
}

void MedialTester::closestPoint(Point3D *p, Voronoi3D::Vertex *vert) const{
  if (vert->closestTri < 0){
    //  hasn't been tested - do closest point check
    ClosestPointInfo inf;
    inf.type = DIST_TYPE_INVALID;
    ::getClosestPoint(&inf, vert->s.c, *sur, faceHash);
    CHECK_DEBUG0(inf.type != DIST_TYPE_INVALID);
    vert->closestPt = inf.pClose;
    vert->closestTri = inf.triangle;
    }

  CHECK_DEBUG0(vert->closestTri >= 0);
  *p = vert->closestPt;

  //Point3D pCloseBounds;
  //if (getClosestBounds(&pCloseBounds, vert->s.c, &vert->closestPt))
  //  *p = pCloseBounds;
}

void MedialTester::closestPointNormal(Point3D *p, Vector3D *v, Voronoi3D::Vertex *vert) const{
  closestPoint(p, vert);

  //  what happens to this if the closest point is on bounds ???  --  FIX
  sur->getNormal(v, *p, vert->closestTri);
}

void MedialTester::testInternal(Voronoi3D::Vertex *vert) const{
  CHECK_DEBUG0(vert->flag == VOR_FLAG_UNKNOWN);

  ClosestPointInfo inf;
  inf.type = DIST_TYPE_INVALID;
  if (vt.insideSurface(vert->s.c, &inf))
    vert->flag = VOR_FLAG_INTERNAL;
  else
    vert->flag = VOR_FLAG_EXTERNAL;

  if (inf.type != DIST_TYPE_INVALID){
    vert->closestPt = inf.pClose;
    vert->closestTri = inf.triangle;
    }
  else{
    vert->closestTri = -1;
    }
}

int MedialTester::processMedial(Voronoi3D *vor, const SurfaceRep *surRep, const Sphere *filterSphere, bool countOnlyCoverSpheres) const{
  //  get internal vertices
  int numVert = vor->vertices.getSize();
  for (int i = 0; i < numVert; i++){
    //  find out if the sphere is internal
    Voronoi3D::Vertex *vert = &vor->vertices.index(i);
    if (vert->flag == VOR_FLAG_UNKNOWN){
      if (filterSphere && !filterSphere->overlap(vert->s))
        continue;

      //  hasn't already been checked
      ClosestPointInfo inf;
      inf.type = DIST_TYPE_INVALID;
      if (vt.insideSurface(vert->s.c, &inf))
        vert->flag = VOR_FLAG_INTERNAL;
      else
        vert->flag = VOR_FLAG_EXTERNAL;

      if (inf.type != DIST_TYPE_INVALID){
        vert->closestPt = inf.pClose;
        vert->closestTri = inf.triangle;
        }
      }
    }

  if (surRep){
    //  clear old coverage flags
    for (int i = 0; i < numVert; i++){
      Voronoi3D::Vertex *vert = &vor->vertices.index(i);
      if (vert->flag == VOR_FLAG_COVER)
        vert->flag = VOR_FLAG_EXTERNAL;
      }

    //  get surface points
    const Array<Surface::Point> *surPts = surRep->getSurPts();
    int numPts = surPts->getSize();

    Array<bool> covered(numPts, 12);
    covered.clear();

    //  mark covered points
    int numCover = 0;
    for (i = 0; i < numVert; i++){
      //  get the vertex
      Voronoi3D::Vertex *vert = &vor->vertices.index(i);
      if (vert->flag == VOR_FLAG_INTERNAL /*|| vert->flag == VOR_FLAG_COVER*/){
        //  check against filter - speedup ???
        if (filterSphere && !filterSphere->overlap(vert->s))
          continue;
        if (surRep->flagContainedPoints(&covered, vert->s) > 0)
          numCover++;
        }
      }

    //  deal with the uncovered points
    for (i = 0; i < numPts; i++){
      if (covered.index(i))
        continue;

      Point3D pTest = surPts->index(i).p;

      //  get the closest cell
      int cell = vor->findCellBrute(pTest);
      const Voronoi3D::FormingPoint *form = &vor->formingPoints.index(cell);
      int numFormingVerts = form->vertices.getSize();

      //  pick the best sphere from the vertices around the forming point
      int newVI = -1;
      if (useLargeCover){
        float maxD = -REAL_MAX;
        for (int j = 0; j < numFormingVerts; j++){
          int vI = form->vertices.index(j);
          Voronoi3D::Vertex *vert = &vor->vertices.index(vI);
          if (vert->flag != VOR_FLAG_OLDCOVER && vert->s.contains(pTest)){
            //  get the closest point
            Point3D pClose;
            closestPoint(&pClose, vert);
            float v = vert->s.r - vert->s.c.distance(pClose);
            if (v > maxD){
              maxD = v;
              newVI = vI;
              }
            }
          }
        }
      else{
        float minD = REAL_MAX;
        for (int j = 0; j < numFormingVerts; j++){
          int vI = form->vertices.index(j);
          Voronoi3D::Vertex *vert = &vor->vertices.index(vI);
          if (vert->flag != VOR_FLAG_OLDCOVER && vert->s.contains(pTest)){
            //  get the closest point
            Point3D pClose;
            closestPoint(&pClose, vert);
            float v = vert->s.r + vert->s.c.distance(pClose);
            if (v < minD){
              minD = v;
              newVI = vI;
              }
            }
          }
        }

      //  make the vertex medial
      if (newVI >= 0){
        Voronoi3D::Vertex *vert = &vor->vertices.index(newVI);
        if (vert->flag != VOR_FLAG_INTERNAL){
          vert->flag = VOR_FLAG_COVER;
          surRep->flagContainedPoints(&covered, vert->s);
          numCover++;
          }
        }
      }
    return numCover;
    }
  else
    return -1;
}
