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

#include "VoronoiFace.h"
#include "../Storage/Queue.h"

//  find other cells using atleast one common vertex
void findNeighbourCells(Array<int> *cells, int cellNum, const Voronoi3D &vor) {
  cells->setSize(0);
  const Voronoi3D::FormingPoint *fp = &vor.formingPoints.index(cellNum);

  int numVerts = fp->vertices.getSize();
  for (int i = 0; i < numVerts; i++) {
    int vI = fp->vertices.index(i);
    const Voronoi3D::Vertex *v = &vor.vertices.index(vI);

    for (int j = 0; j < 4; j++) {
      int fpI = v->f[j];
      if (fpI != cellNum && !cells->inList(fpI))
        cells->addItem() = fpI;
    }
  }
}

bool getFace(VoronoiFace *face, int c1, int c2, const Voronoi3D &vor) {
  //  get common vertices
  const Voronoi3D::FormingPoint *fp1 = &vor.formingPoints.index(c1);
  const Voronoi3D::FormingPoint *fp2 = &vor.formingPoints.index(c2);

  Array<int> commonVerts;
  int numFP1verts = fp1->vertices.getSize();
  for (int i = 0; i < numFP1verts; i++) {
    int vI = fp1->vertices.index(i);
    if (fp2->vertices.inList(vI))
      commonVerts.addItem() = vI;
  }

  //  form loop
  face->vertices.resize(1);
  face->vertices.index(0) = commonVerts.index(0);
  while (true) {
    //  find next vertex
    int numVerts = face->vertices.getSize();
    int currentV = face->vertices.index(numVerts - 1);
    const Voronoi3D::Vertex *v = &vor.vertices.index(currentV);

    int i;
    for (i = 0; i < 4; i++) {
      int nI = v->n[i];
      if (!face->vertices.inList(nI) && commonVerts.inList(nI)) {
        break;
      }
    }

    if (i == 4) {
      break; //  no more - done
    } else {
      face->vertices.addItem() = v->n[i];
    }
  }

  if (face->vertices.getSize() < 3) {
    return false;
  } else {
    //  make plane
    Point3D p0 = vor.vertices.index(face->vertices.index(0)).s.c;
    Point3D p1 = vor.vertices.index(face->vertices.index(1)).s.c;
    Point3D p2 = vor.vertices.index(face->vertices.index(2)).s.c;
    face->plane.assign(p0, p1, p2);
    return true;
  }
}
