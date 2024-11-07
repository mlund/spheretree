

/*******************************************************
 *                                                      *
 *  volInt.c                                            *
 *                                                      *
 *  This code computes volume integrals needed for      *
 *  determining mass properties of polyhedral bodies.   *
 *                                                      *
 *  For more information, see the accompanying README   *
 *  file, and the paper                                 *
 *                                                      *
 *  Brian Mirtich, "Fast and Accurate Computation of    *
 *  Polyhedral Mass Properties," journal of graphics    *
 *  tools, volume 1, number 2, 1996.                    *
 *                                                      *
 *  This source code is public domain, and may be used  *
 *  in any way, shape or form, free of charge.          *
 *                                                      *
 *  Copyright 1995 by Brian Mirtich                     *
 *                                                      *
 *  mirtich@cs.berkeley.edu                             *
 *  http://www.cs.berkeley.edu/~mirtich                 *
 *                                                      *
 *******************************************************/

/*
        Revision history

        26 Jan 1996	Program creation.

         3 Aug 1996	Corrected bug arising when polyhedron density
                        is not 1.0.  Changes confined to function main().
                        Thanks to Zoran Popovic for catching this one.

        27 May 1997     Corrected sign error in translation of inertia
                        product terms to center of mass frame.  Changes
                        confined to function main().  Thanks to
                        Chris Hecker.
*/

#include "VolInt.h"

#define X 0
#define Y 1
#define Z 2

#define SQR(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

//  globals
namespace VolInt_Globals {
static int A; /* alpha */
static int B; /* beta */
static int C; /* gamma */

/* projection integrals */
static double P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

/* face integrals */
static double Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

/* volume integrals */
static double T0, T1[3], T2[3], TP[3];
}; // namespace VolInt_Globals

/* compute various integrations over projection of face */
void compProjectionIntegrals(const Surface &sur, int face) {
  using namespace VolInt_Globals;

  double a0, a1, da;
  double b0, b1, db;
  double a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
  double a1_2, a1_3, b1_2, b1_3;
  double C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
  double Cab, Kab, Caab, Kaab, Cabb, Kabb;
  int i;

  P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0;

  // make face into arrays
  struct Face {
    double verts[3][3];
  } FACE;
  const Surface::Triangle *tri = &sur.triangles.index(face);
  for (i = 0; i < 3; i++) {
    const Point3D *p = &sur.vertices.index(tri->v[i]).p;
    FACE.verts[i][0] = p->x;
    FACE.verts[i][1] = p->y;
    FACE.verts[i][2] = p->z;
  }

  //  do his stuff
  for (i = 0; i < 3; i++) {
    a0 = FACE.verts[i][A];
    b0 = FACE.verts[i][B];
    a1 = FACE.verts[(i + 1) % 3][A];
    b1 = FACE.verts[(i + 1) % 3][B];
    da = a1 - a0;
    db = b1 - b0;
    a0_2 = a0 * a0;
    a0_3 = a0_2 * a0;
    a0_4 = a0_3 * a0;
    b0_2 = b0 * b0;
    b0_3 = b0_2 * b0;
    b0_4 = b0_3 * b0;
    a1_2 = a1 * a1;
    a1_3 = a1_2 * a1;
    b1_2 = b1 * b1;
    b1_3 = b1_2 * b1;

    C1 = a1 + a0;
    Ca = a1 * C1 + a0_2;
    Caa = a1 * Ca + a0_3;
    Caaa = a1 * Caa + a0_4;
    Cb = b1 * (b1 + b0) + b0_2;
    Cbb = b1 * Cb + b0_3;
    Cbbb = b1 * Cbb + b0_4;
    Cab = 3 * a1_2 + 2 * a1 * a0 + a0_2;
    Kab = a1_2 + 2 * a1 * a0 + 3 * a0_2;
    Caab = a0 * Cab + 4 * a1_3;
    Kaab = a1 * Kab + 4 * a0_3;
    Cabb = 4 * b1_3 + 3 * b1_2 * b0 + 2 * b1 * b0_2 + b0_3;
    Kabb = b1_3 + 2 * b1_2 * b0 + 3 * b1 * b0_2 + 4 * b0_3;

    P1 += db * C1;
    Pa += db * Ca;
    Paa += db * Caa;
    Paaa += db * Caaa;
    Pb += da * Cb;
    Pbb += da * Cbb;
    Pbbb += da * Cbbb;
    Pab += db * (b1 * Cab + b0 * Kab);
    Paab += db * (b1 * Caab + b0 * Kaab);
    Pabb += da * (a1 * Cabb + a0 * Kabb);
  }

  P1 /= 2.0;
  Pa /= 6.0;
  Paa /= 12.0;
  Paaa /= 20.0;
  Pb /= -6.0;
  Pbb /= -12.0;
  Pbbb /= -20.0;
  Pab /= 24.0;
  Paab /= 60.0;
  Pabb /= -60.0;
}

void compFaceIntegrals(const Surface &sur, int FACE_NUM, double *n) {
  using namespace VolInt_Globals;
  double k1, k2, k3, k4;

  compProjectionIntegrals(sur, FACE_NUM);

  Point3D vert = sur.vertices.index(sur.triangles.index(FACE_NUM).v[0]).p;
  double w = -n[X] * vert.x - n[Y] * vert.y - n[Z] * vert.z;

  k1 = 1 / n[C];
  k2 = k1 * k1;
  k3 = k2 * k1;
  k4 = k3 * k1;

  Fa = k1 * Pa;
  Fb = k1 * Pb;
  Fc = -k2 * (n[A] * Pa + n[B] * Pb + w * P1);

  Faa = k1 * Paa;
  Fbb = k1 * Pbb;
  Fcc = k3 * (SQR(n[A]) * Paa + 2 * n[A] * n[B] * Pab + SQR(n[B]) * Pbb +
              w * (2 * (n[A] * Pa + n[B] * Pb) + w * P1));

  Faaa = k1 * Paaa;
  Fbbb = k1 * Pbbb;
  Fccc = -k4 *
         (CUBE(n[A]) * Paaa + 3 * SQR(n[A]) * n[B] * Paab +
          3 * n[A] * SQR(n[B]) * Pabb + CUBE(n[B]) * Pbbb +
          3 * w * (SQR(n[A]) * Paa + 2 * n[A] * n[B] * Pab + SQR(n[B]) * Pbb) +
          w * w * (3 * (n[A] * Pa + n[B] * Pb) + w * P1));

  Faab = k1 * Paab;
  Fbbc = -k2 * (n[A] * Pabb + n[B] * Pbbb + w * Pbb);
  Fcca = k3 * (SQR(n[A]) * Paaa + 2 * n[A] * n[B] * Paab + SQR(n[B]) * Pabb +
               w * (2 * (n[A] * Paa + n[B] * Pab) + w * Pa));
}

void compVolumeIntegrals(const Surface &sur) {
  using namespace VolInt_Globals;
  double nx, ny, nz;
  int i;

  T0 = T1[X] = T1[Y] = T1[Z] = T2[X] = T2[Y] = T2[Z] = TP[X] = TP[Y] = TP[Z] =
      0;

  int numTri = sur.triangles.getSize();
  for (i = 0; i < numTri; i++) {
    Vector3D v;
    sur.getTriangleNormal(&v, i);
    double norm[3] = {v.x, v.y, v.z};

    nx = fabs(v.x);
    ny = fabs(v.y);
    nz = fabs(v.z);
    if (nx > ny && nx > nz)
      C = X;
    else
      C = (ny > nz) ? Y : Z;
    A = (C + 1) % 3;
    B = (A + 1) % 3;

    compFaceIntegrals(sur, i, norm);

    T0 += norm[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

    T1[A] += norm[A] * Faa;
    T1[B] += norm[B] * Fbb;
    T1[C] += norm[C] * Fcc;
    T2[A] += norm[A] * Faaa;
    T2[B] += norm[B] * Fbbb;
    T2[C] += norm[C] * Fccc;
    TP[A] += norm[A] * Faab;
    TP[B] += norm[B] * Fbbc;
    TP[C] += norm[C] * Fcca;
  }

  T1[X] /= 2;
  T1[Y] /= 2;
  T1[Z] /= 2;
  T2[X] /= 3;
  T2[Y] /= 3;
  T2[Z] /= 3;
  TP[X] /= 2;
  TP[Y] /= 2;
  TP[Z] /= 2;
}

void computeMassProperties(REAL *mass, REAL R[3], REAL J[3][3],
                           const Surface &sur) {
  using namespace VolInt_Globals;

  compVolumeIntegrals(sur);

  /* compute mass - assume unit density */
  REAL density = 1.0;
  *mass = density * T0;

  /* compute center of mass */
  R[X] = T1[X] / T0;
  R[Y] = T1[Y] / T0;
  R[Z] = T1[Z] / T0;

  /* compute inertia tensor */
  J[X][X] = density * (T2[Y] + T2[Z]);
  J[Y][Y] = density * (T2[Z] + T2[X]);
  J[Z][Z] = density * (T2[X] + T2[Y]);
  J[X][Y] = J[Y][X] = -density * TP[X];
  J[Y][Z] = J[Z][Y] = -density * TP[Y];
  J[Z][X] = J[X][Z] = -density * TP[Z];

  /* translate inertia tensor to center of mass */
  double m = *mass;
  J[X][X] -= m * (R[Y] * R[Y] + R[Z] * R[Z]);
  J[Y][Y] -= m * (R[Z] * R[Z] + R[X] * R[X]);
  J[Z][Z] -= m * (R[X] * R[X] + R[Y] * R[Y]);
  J[X][Y] = J[Y][X] += m * R[X] * R[Y];
  J[Y][Z] = J[Z][Y] += m * R[Y] * R[Z];
  J[Z][X] = J[X][Z] += m * R[Z] * R[X];
}