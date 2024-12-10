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

#include "BBox.h"
#include "eigen.h"
#include "MVBB/gdiam.h"
#include "qHull/qhull_a.h"

//  surface normals won't mean much
void getConvexHull(Array<Point3D> *dest, const Array<Surface::Point> &src) {
  //  allocate co-ordinates of the vertices - as qHull doesn't store it
  int numPts = src.getSize();
  coordT *array = new coordT[numPts * 3];
  for (int i = 0; i < numPts; i++) {
    Point3D p = src.index(i).p;
    array[i * 3] = p.x;
    array[i * 3 + 1] = p.y;
    array[i * 3 + 2] = p.z;
  }

  //  setup qhull to make the convex hull
  qh_init_A(stdin, stdout, stderr, 0, NULL);
  qh_init_B(array, numPts, 3, false);
  qh_qhull();

  //  read the vertices from the convex hull
  vertexT *vertex;
  dest->setSize(0);
  FORALLvertices {
    if (vertex->deleted)
      continue;

    Point3D *p = &dest->addItem();
    p->x = vertex->point[0];
    p->y = vertex->point[1];
    p->z = vertex->point[2];
  }

  //  tidy up qhull
  int curlong, totlong;
  qh_freeqhull(!qh_ALL);
  qh_memfreeshort(&curlong, &totlong);

  delete array;
}

//  using covariance matrix
void computeOBBcovar(Vector3D V[3], const Array<Surface::Point> &inPts) {
  Array<Point3D> pts;
  getConvexHull(&pts, inPts);

  //  compute the mean of the points
  Point3D u = Point3D::ZERO;
  int numVert = pts.getSize();
  for (int i = 0; i < numVert; i++) {
    //  get the point
    Point3D p = pts.index(i); //.p;

    //  add to mean
    u.x += p.x / numVert;
    u.y += p.y / numVert;
    u.z += p.z / numVert;
  }

  //  compute the covariance matrix
  double c0[3], c1[3], c2[3], *c[3];
  c[0] = c0;
  c[1] = c1;
  c[2] = c2;

  for (int j = 0; j < 3; j++) {
    for (int k = j; k < 3; k++) {
      float sum = 0;
      for (int i = 0; i < numVert; i++) {
        //  get point
        Point3D p = pts.index(i); //.p;

        // compute p`
        float pP[3];
        pP[0] = p.x - u.x;
        pP[1] = p.y - u.y;
        pP[2] = p.z - u.z;

        //  add term
        sum += pP[j] * pP[k] / numVert;
      }

      c[j][k] = sum;
      c[k][j] = sum;
    }
  }

  //  get eigen vectors and vals
  int numRot;
  double d[3];
  double v0[3], v1[3], v2[3], *v[3];
  v[0] = v0;
  v[1] = v1;
  v[2] = v2;
  jacobi(c, 3, d, v, &numRot);

  //  store the basis vectors
  V[0].x = v[0][0];
  V[0].y = v[1][0];
  V[0].z = v[2][0];
  V[0].norm();

  V[1].x = v[0][1];
  V[1].y = v[1][1];
  V[1].z = v[2][1];
  V[1].norm();

  V[2].x = v[0][2];
  V[2].y = v[1][2];
  V[2].z = v[2][2];
  V[2].norm();
}

//  using inertial tensor
void computeInterialTensor(double **c, const Array<Surface::Point> &pts) {
  //  compute the mean of the points
  Point3D u = Point3D::ZERO;
  int numVert = pts.getSize();
  for (int i = 0; i < numVert; i++) {
    //  get the points of the triangle
    Point3D p = pts.index(i).p;

    u.x += p.x / numVert;
    u.y += p.y / numVert;
    u.z += p.z / numVert;
  }

  //  off diagonals
  for (int j = 0; j < 3; j++) {
    for (int k = j + 1; k < 3; k++) {
      float sum = 0;
      for (int i = 0; i < numVert; i++) {
        //  get points
        Point3D p = pts.index(i).p;

        // compute p`
        float pP[3];
        pP[0] = p.x - u.x;
        pP[1] = p.y - u.y;
        pP[2] = p.z - u.z;

        //  add term
        sum += pP[j] * pP[k];
      }

      c[j][k] = -sum;
      c[k][j] = -sum;
    }
  }

  //  diagonals
  for (int j = 0; j < 3; j++) {
    float sum = 0;
    for (int i = 0; i < numVert; i++) {
      //  get points
      Point3D p = pts.index(i).p;

      // compute p`
      float pP[3];
      pP[0] = p.x - u.x;
      pP[1] = p.y - u.y;
      pP[2] = p.z - u.z;

      //  add term
      int i1 = (j + 1) % 3;
      int i2 = (j + 2) % 3;
      sum += pP[i1] * pP[i1] + pP[i2] * pP[i2];
    }

    c[j][j] = sum;
  }
}

void computeOBBinert(Vector3D V[3], const Array<Surface::Point> &pts,
                     float *mags) {
  //  compute the inertial tensors
  //  good unless the dimensions are exactly equal (could we use eigen vals to
  //  detech this ??)
  double c0[3], c1[3], c2[3], *c[3];
  c[0] = c0;
  c[1] = c1;
  c[2] = c2;
  computeInterialTensor(c, pts);

  //  get eigen vectors and vals
  int numRot;
  double d[3];
  double v0[3], v1[3], v2[3], *v[3];
  v[0] = v0;
  v[1] = v1;
  v[2] = v2;
  jacobi(c, 3, d, v, &numRot);

  //  store the basis vectors
  V[0].x = v[0][0];
  V[0].y = v[1][0];
  V[0].z = v[2][0];
  V[0].norm();

  V[1].x = v[0][1];
  V[1].y = v[1][1];
  V[1].z = v[2][1];
  V[1].norm();

  V[2].x = v[0][2];
  V[2].y = v[1][2];
  V[2].z = v[2][2];
  V[2].norm();

  if (mags) {
    mags[0] = d[0];
    mags[1] = d[1];
    mags[2] = d[2];
  }
}

void computeMVBB(Vector3D v[3], const Array<Surface::Point> &pts) {
  int numPts = pts.getSize();

  //  setup arrays
  int numUsed = 0;
  gdiam_real *p_arr = new gdiam_real[numPts * 3];
  gdiam_real *ptr = p_arr;
  for (int i = 0; i < numPts; i++) {
    const Point3D *p = &pts.index(i).p;
    int j;
    for (j = 0; j < numUsed; j++) {
      Point3D q = {p_arr[3 * j], p_arr[3 * j + 1], p_arr[3 * j + 2]};
      if (p->distance(q) < EPSILON_LARGE)
        break;
    }
    if (j == numUsed) {
      ptr[0] = p->x;
      ptr[1] = p->y;
      ptr[2] = p->z;
      ptr += 3;
      numUsed++;
    } else {
      OUTPUTINFO("Dup\n");
    }
  }
  gdiam_point *ptr_arr = gdiam_convert(p_arr, numPts);

  //  get bounding box
  gdiam_bbox bb = gdiam_approx_mvbb_grid(ptr_arr, numUsed, 5);
  for (int j = 0; j < 3; j++) {
    gdiam_point p = bb.get_dir(j);
    v[j].x = p[0];
    v[j].y = p[1];
    v[j].z = p[2];
  }

  //  tidy up
  delete p_arr;
  free(ptr_arr);

  /*  GBBox  ap_bbox;
    float start = clock();
    gdiam_bbox bb = gdiam_approx_const_mvbb(ptr_arr, numUsed, 0.0, &ap_bbox );
    float durn = (clock() - start)/(float)CLOCKS_PER_SEC;
    OUTPUTINFO("Time for const approx   : %g\n", durn);
    OUTPUTINFO("Volume on axis para bbox  : %g\n", ap_bbox.volume() );
    OUTPUTINFO("Volume of const approx    : %g\n", bb.volume() );
    for (int j = 0; j < 3; j++){
      gdiam_point p = bb.get_dir(j);
      v[j].x = p[0];
      v[j].y = p[1];
      v[j].z = p[2];

      OUTPUTINFO("v[%d] = %f %f %f\n", j, v[j].x, v[j].y, v[j].z);
      }
  */
  /*
    start = clock();
    bb = gdiam_approx_mvbb_grid_sample(ptr_arr, numUsed, 5, 400);
    durn = (clock() - start)/(float)CLOCKS_PER_SEC;
    OUTPUTINFO("Time for sample approx  : %g\n", durn);
    OUTPUTINFO("Volume of grid sample     : %g\n", bb.volume());
    for (j = 0; j < 3; j++){
      gdiam_point p = bb.get_dir(j);
      v[j].x = p[0];
      v[j].y = p[1];
      v[j].z = p[2];

      OUTPUTINFO("v[%d] = %f %f %f\n", j, v[j].x, v[j].y, v[j].z);
      }
  */
}
