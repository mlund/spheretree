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

#include "Clusters.h"
#include "../Geometry/Plane.h"
#include "../Base/Defs.h"

void getClusters(Array<Array<int> /**/> *clusters, const Surface &sur,
                 float tol) {
  int numTri = sur.triangles.getSize();

  Array<bool> clustered;
  clustered.resize(numTri);
  clustered.clear();

  clusters->setSize(0);
  int numUnclustered = numTri;
  while (numUnclustered) {
    //  find the first unclustered triangle
    int i;
    for (i = 0; i < numTri; i++)
      if (!clustered.index(i))
        break;

    //  make a fresh cluster
    Array<int> *newCluster = &clusters->addItem();
    newCluster->resize(1);
    newCluster->index(0) = i;
    clustered.index(i) = true;
    numUnclustered--;

    //  get plane for the triangle
    const Surface::Triangle *tri = &sur.triangles.index(i);
    Point3D p0 = sur.vertices.index(tri->v[0]).p;
    Point3D p1 = sur.vertices.index(tri->v[1]).p;
    Point3D p2 = sur.vertices.index(tri->v[2]).p;
    Plane clusterPlane;
    clusterPlane.assign(p0, p1, p2);

    //  setup the list of neighbours to be considered
    Array<int> neighbours;
    for (int j = 0; j < 3; j++) {
      int triJ = tri->f[j];
      if (!clustered.index(triJ))
        neighbours.addItem() = triJ;
    }

    //  process neighbours and work out way out
    while (neighbours.getSize()) {
      int triN = neighbours.index(0);
      neighbours.removeItem(0);

      if (!clustered.index(triN)) {
        //  do planar check
        const Surface::Triangle *tri = &sur.triangles.index(triN);
        int j;
        for (j = 0; j < 3; j++)
          if (fabs(clusterPlane.dist(sur.vertices.index(tri->v[j]).p)) > tol)
            break;

        if (j == 3) {
          //  add to cluster
          newCluster->addItem() = triN;
          clustered.index(triN) = true;
          numUnclustered--;

          //  add neighbours for processing
          for (int k = 0; k < 3; k++) {
            int triK = tri->f[k];
            if (triK >= 0 &&
                !clustered.index(
                    triK)) // could also check inList but clustered.index
              neighbours.addItem() = triK; // later on is more efficient
          }
        }
      }
    }
  }

  OUTPUTINFO("%d clusters\n", clusters->getSize());
}

bool hasEdge(const Array<int> &cluster, const Surface &sur, int v1, int v2) {
  int numTri = cluster.getSize();
  for (int i = 0; i < numTri; i++) {
    const Surface::Triangle *tri = &sur.triangles.index(cluster.index(i));
    for (int j = 0; j < 3; j++) {
      int u1 = tri->v[j];
      int u2 = tri->v[(j + 1) % 3];

      if (u1 == v1 && u2 == v2 || u1 == v2 && u2 == v1)
        return true;
    }
  }

  return false;
}

bool areNeighbours(const Array<Array<int> /**/> &clusters, const Surface &sur,
                   int I1, int I2) {
  //  look to see if there is a common edge
  const Array<int> *c1 = &clusters.index(I1);
  const Array<int> *c2 = &clusters.index(I2);

  int numTri = c1->getSize();
  for (int i = 0; i < numTri; i++) {
    const Surface::Triangle *tri = &sur.triangles.index(c1->index(i));
    for (int j = 0; j < 3; j++) {
      int v1 = tri->v[j];
      int v2 = tri->v[(j + 1) % 3];
      if (hasEdge(*c2, sur, v1, v2))
        return true;
    }
  }

  return false;
}

void getClusterInfo(ClusterInfo *inf, const Surface &sur, float tol) {
  //  make the clusters
  getClusters(&inf->clusters, sur, tol);
  int numClusters = inf->clusters.getSize();

  //  get neighbourhood information for clusters
  inf->neighbours.resize(numClusters);
  for (int i = 0; i < numClusters; i++)
    inf->neighbours.index(i).setSize(0);

  for (int i = 0; i < numClusters; i++)
    for (int j = i + 1; j < numClusters; j++)
      if (areNeighbours(inf->clusters, sur, i, j)) {
        inf->neighbours.index(i).addItem() = j;
        inf->neighbours.index(j).addItem() = i;
      }
}
