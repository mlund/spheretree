/*

  THIS FILE CONTAINED CODE FROM NUMERICAL RECIPIES IN C WHICH CANNOT BE
  REDISTRIBUTED

        IN ORDER TO COMPILE THIS CODE YOU WILL HAVE TO OBTAIN THE NR SOURCE

*/

#ifndef NRSVD
#define NRSVD

void svbksb(float **u, float w[], float **v, int m, int n, float b[],
            float x[]);
/*Solves A . X = B for a vector X, whereAis specied by the arrays
u[1..m][1..n], w[1..n], v[1..n][1..n] as returned by svdcmp. m and n are the
dimensions of a, and will be equal for square matrices. b[1..m] is the input
right-hand side. x[1..n] is the output solution vector. No input quantities are
destroyed, so the routine may be called sequentially with dierent b's.*/

void svdcmp(float **a, int m, int n, float w[], float **v);
/*Given a matrix a[1..m][1..n], this routine computes its singular value
decomposition, A = U . W . V T . ThematrixUreplaces a on output. The diagonal
matrix of singular values W is out- put as a vector w[1..n]. ThematrixV(not the
transpose V T ) is output as v[1..n][1..n].*/

#endif
