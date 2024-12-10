#include "Matrix.h"
#include <math.h>

Matrix::Matrix(int m, int n) {
  if (m != 0 && n != 0) {
    allocate(m, n);
  }
}

Matrix::Matrix(const Matrix &other) {
  if (other.m != 0 && other.n != 0) {
    allocate(other.m, other.n);
    assign(other);
  }
}

//  init
void Matrix::identity() {
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      if (i == j)
        index(i, j) = 1.0;
      else
        index(i, j) = 0.0;
}

void Matrix::assign(const Matrix &other) {
  CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
               other.n);

  m = other.m;
  n = other.n;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) = other.index(i, j);
}

void Matrix::transpose() {
  CHECK_DEBUG2(n == m, "Array Not Square (%d, %d)", m, n);

  for (int i = 0; i < m; i++)
    for (int j = 0; j < i; j++) {
      float tmp = index(i, j);
      index(i, j) = index(j, i);
      index(j, i) = tmp;
    }
}

void Matrix::transpose(const Matrix &other) {
  CHECK_DEBUG2(other.n == other.m, "Array Not Square (%d, %d)", m, n);
  CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
               other.n);

  m = other.m;
  n = other.n;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) = other.index(j, i);
}

void Matrix::add(const Matrix &other, REAL s) {
  CHECK_DEBUG4(m == other.m && n == other.n, "Array Size (%d, %d) += (%d, %d)",
               m, n, other.m, other.n);

  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) += other.index(i, j) * s;
}

void Matrix::add(const Matrix &m1, const Matrix &m2) {
  CHECK_DEBUG4(m1.m == m2.m && m1.n == m2.n, "Array Size (%d, %d) + (%d, %d)",
               m1.m, m1.n, m2.m, m2.n);
  CHECK_DEBUG4(allocM >= m1.m && allocN >= m1.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, m1.m, m1.n);

  m = m1.m;
  n = m1.n;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) = m1.index(j, i) + m2.index(j, i);
}

void Matrix::sub(const Matrix &other, REAL s) {
  CHECK_DEBUG4(m == other.m && n == other.n, "Array Size (%d, %d) -= (%d, %d)",
               m, n, other.m, other.n);

  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) -= other.index(i, j) * s;
}

void Matrix::sub(const Matrix &m1, const Matrix &m2) {
  CHECK_DEBUG4(m1.m == m2.m && m1.n == m2.n, "Array Size (%d, %d) - (%d, %d)",
               m1.m, m1.n, m2.m, m2.n);
  CHECK_DEBUG4(allocM >= m1.m && allocN >= m1.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, m1.m, m1.n);

  m = m1.m;
  n = m1.n;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) = m1.index(j, i) - m2.index(j, i);
}

void Matrix::mult(REAL s) {
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) *= s;
}

void Matrix::mult(const Matrix &other, REAL s) {
  CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
               other.n);

  m = other.m;
  n = other.n;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      index(i, j) = other.index(i, j) * s;
}

void Matrix::mult(const Matrix &m1, const Matrix &m2) {
  CHECK_DEBUG4(m1.n == m2.m, "Matrix Size (%d, %d) * (%d, %d)", m1.m, m1.n,
               m2.m, m2.n);
  CHECK_DEBUG4(allocM >= m1.m && allocN == m2.n,
               "Matrix Size (%d, %d) <- (%d, %d)", allocM, allocN, m1.m, m1.n);

  m = m1.m;
  n = m2.n;
  for (int row = 0; row < m; row++)
    for (int col = 0; col < n; col++) {
      REAL val = 0.0;

      for (int k = 0; k < m1.n; k++)
        val += m1.index(row, k) * m2.index(k, col);

      index(row, col) = val;
    }
}

bool Matrix::invert(const Matrix &other) {
  CHECK_DEBUG2(other.m == other.n && other.m >= 2, "Invalid Matrix (%d, %d)",
               other.m, other.n);
  CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
               "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
               other.n);

  m = other.m;
  n = other.n;

  if (other.m == 2) { //  special efficient case
    REAL det = other.index(0, 0) * other.index(1, 1) -
               other.index(0, 1) * other.index(1, 0);

    if (fabs(det) <= EPSILON)
      return false;

    index(0, 0) = other.index(0, 0) / det;
    index(0, 1) = other.index(0, 1) / det;
    index(1, 0) = other.index(1, 0) / det;
    index(1, 1) = other.index(1, 1) / det;

    return true;
  } else if (other.m > 2) { //  gaussian reduction
    int size = other.m;
    identity();
    REAL **outData = data;

    Matrix tmp(other);
    REAL **tmpData = tmp.data;

    for (int pass = 0; pass < size; pass++) { //  for each column
      int maxRow = pass;

      for (int row = pass; row < size; row++) { //  find pivot row
        if (fabs(tmpData[row][pass]) > fabs(tmpData[maxRow][pass]))
          maxRow = row;
      }

      if (maxRow != pass) { //  swap pivot and current row
        for (int col = 0; col < size; col++) {
          REAL temp = outData[pass][col];
          outData[pass][col] = outData[maxRow][col];
          outData[maxRow][col] = temp;

          if (col >= pass) {
            REAL temp = tmpData[pass][col];
            tmpData[pass][col] = tmpData[maxRow][col];
            tmpData[maxRow][col] = temp;
          }
        }
      }

      REAL pivot = tmpData[pass][pass];
      if (fabs(pivot) <= EPSILON) // end
        return false;

      for (int col = 0; col < size; col++) { //  divide across by pivot
        outData[pass][col] /= pivot;
        if (col >= pass)
          tmpData[pass][col] /= pivot;
      }

      for (int row = 0; row < size; row++) //  get each other orw to be 0
        if (row != pass) {
          REAL factor = tmpData[row][pass];
          for (int col = 0; col < size; col++) {
            outData[row][col] -= factor * outData[pass][col];
            tmpData[row][col] -= factor * tmpData[pass][col];
          }
        }
    }

    return true;
  } else
    return false;
}

REAL Matrix::det() const {
  CHECK_DEBUG2(m == n, "Non-Square Matrix (%d, %d)", m, n);

  REAL det = 0;
  for (int col = 0; col < n; col++)
    for (int row = 0; row < m; row++)
      det += data[row][(col + row) % n];

  for (int col = 0; col < n; col++) {
    for (int row = 0; row < m; row++) {
      int colI = col - row;
      if (colI < 0)
        colI += n;
      det -= data[row][colI];
    }
  }

  return det;
}

void Matrix::print(FILE *f, const char *msg) const {
  CHECK_DEBUG(f != NULL, "NULL file");

  if (msg)
    fprintf(f, "%s\n", msg);

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      fprintf(f, "%.4f\t", index(i, j));
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
}
