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

#include "Hybrid.h"
#include "SFWhite.h"
#include "SOPerSphere.h"
#include "SurfaceDiv.h"

void makeHybridNode(STSphere *node, const Array<Surface::Point> &surPts,
                    const SEBase *se) {
  Sphere boundingSphere, minErrSphere;

  //  make the bounding sphere
  SFWhite::makeSphere(&boundingSphere, surPts);
  if (boundingSphere.r <= EPSILON) {
    boundingSphere.r = -1;
    node->r = -1;
    node->c.x = node->c.y = node->c.z = 0;
    node->hasAux = false;
  } else {
    /*    //  check that bounding sphere has actually shrunk
        if (boundingSphere.r > node->r){
          //  compute new radius
          //double newR = node->r;
          //int numSur = surPts.getSize();
          //for (int i = 0; i < numSur; i++){
          //  Point3D p = surPts.index(i).p;
          //  double r = p.distance(node->c);
          //  if (r > newR)
          //    newR = r;
          //  }

          //  use new radius
          boundingSphere.c = node->c;
          boundingSphere.r = node->r; //newR;
          }*/

    //  make points
    int numPts = surPts.getSize();
    Array<Point3D> pts(numPts);
    for (int i = 0; i < numPts; i++)
      pts.index(i) = surPts.index(i).p;

    //  make the minimum error sphere
    SOPerSphere perSphere;
    perSphere.eval = se;
    minErrSphere = boundingSphere;
    perSphere.optimise(&minErrSphere, pts);

    //  store the spheres
    node->c = boundingSphere.c;
    node->r = boundingSphere.r;
    node->sAux = minErrSphere;
    node->hasAux = true;

    //  compute gain
    float errBound = se->evalSphere(boundingSphere);
    float errMin = se->evalSphere(minErrSphere);
    float rDiff = (errBound - errMin) / errBound;

    if (rDiff <= 0)
      node->hasAux = false;
    else
      node->errDec = rDiff;
  }
}

void makeSubHybrid(SphereTree *st, int node, int level,
                   const Array<Surface::Point> &pts, const SEBase *eval) {
  int numPts = pts.getSize();

  if (level >= st->levels - 1)
    return;

  //  get the set of children
  int firstChild = st->getFirstChild(node);
  Array<Sphere> children;
  for (int i = 0; i < st->degree; i++) {
    Sphere s = st->nodes.index(firstChild + i);
    if (s.r > 0)
      children.addItem() = s;
  }
  int numChildren = children.getSize();
  if (numChildren == 0)
    return;

  //  get half spaces
  SurfaceDivision surDiv;
  surDiv.setup(children, &pts);

  //  make list of points in each sphere
  Array<Array<Surface::Point> /**/> selPts;
  selPts.resize(numChildren);
  Array<bool> covered(numPts);
  covered.clear();
  for (int i = 0; i < numChildren; i++) {
    //  get sphere
    Sphere s = children.index(i);

    //  filter points
    Array<Surface::Point> *subPts = &selPts.index(i);
    for (int j = 0; j < numPts; j++) {
      Surface::Point p = pts.index(j);
      if (!covered.index(j) && surDiv.pointInRegion(p.p, i)) {
        subPts->addItem() = p;
        covered.index(j) = true;
      }
    }
  }

  //  replace the spheres with minVolume bounding spheres
  for (int i = 0; i < numChildren; i++) {
    Array<Surface::Point> *surPts = &selPts.index(i);
    Sphere *sph = &children.index(i);
    if (surPts->getSize() == 0) {
      sph->r = EPSILON;
    } else if (surPts->getSize() == 1) {
      sph->c = surPts->index(0).p;
      sph->r = EPSILON;
    } else {
      Sphere s = *sph;
      SFWhite::makeSphere(sph, *surPts);
      if (!finite(sph->r) || sph->r <= 0)
        *sph = s;
    }
  }

  //  cover uncovered points
  for (int j = 0; j < numPts; j++) {
    if (!covered.index(j)) {
      //  get point
      Surface::Point p = pts.index(j);

      //  find closest sphere
      int minI = -1;
      float minD = FLT_MAX;
      for (int i = 0; i < numChildren; i++) {
        Sphere s = children.index(i);
        float d = s.c.distance(p.p) - s.r;
        if (d < minD) {
          minI = i;
          minD = d;
        }
      }

      //  add point to sphere's list
      selPts.index(minI).addItem() = p;
    }
  }

  //  do children
  for (int i = 0; i < numChildren; i++) {
    //  get sphere
    Sphere s = children.index(i);

    //  filter points
    const Array<Surface::Point> *subPts = &selPts.index(i);

    //  make the node
    if (subPts->getSize() > 1) {
      OUTPUTINFO("Making Node : %d\n", firstChild + i);
      makeHybridNode(&st->nodes.index(firstChild + i), *subPts, eval);

      //  recurse
      makeSubHybrid(st, firstChild + i, level + 1, *subPts, eval);
    } else {
      st->initNode(firstChild + i);
    }
  }
}

void makeTreeHybrid(SphereTree *st, const Array<Surface::Point> &pts,
                    const SEBase *eval) {
  makeHybridNode(&st->nodes.index(0), pts, eval);
  makeSubHybrid(st, 0, 0, pts, eval);
}

void makeTreeHybrid(SphereTree *st, const SurfaceRep &surRep,
                    const SEBase *eval) {
  const Array<Surface::Point> *surPts = surRep.getSurPts();
  makeTreeHybrid(st, *surPts, eval);
}
