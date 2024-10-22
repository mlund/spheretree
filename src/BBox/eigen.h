#ifndef _EIGEN_H_
#define _EIGEN_H_

void jacobi(double **a, int n, double d[], double **v, int *nrot);
// Computes all eigenvalues and eigenvectors of a real symmetric matrix
// a[0..n][0..n]. On output, elements of a above the diagonal are destroyed.
// d[0..n] returns the eigenvalues of a. v[0..n][0..n] is a matrix whose columns
// contain, on output, the normalized eigenvectors of a. nrot returns the number
// of Jacobi rotations that were required.

#endif