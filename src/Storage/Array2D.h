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

/*
    two domensional array
    M*N, M rows, n cols, [row][col] indexing

    uses efficient allocation of all sub arrays in one block
    and copy etc. process in blocks

    Gareth Bradshaw : 24/July/1999
*/

#ifndef _ARRAY_2D_H_
#define _ARRAY_2D_H_

#include <stdlib.h>
#include <memory.h>
#include "../Base/Types.h"
#include "../Exceptions/CheckDebug.h"
#include "../Exceptions/CheckMemory.h"

template <class T> class Array2D {
protected:
  int m, n, allocM, allocN;
  T **data;

public:
  //  constructor
  Array2D() {
    data = NULL;
    m = n = 0;
    allocM = allocN = 0;
  }

  ~Array2D() {
    if (data)
      free();
  }

  //  allocation
  void allocate(int m, int n) {
    CHECK_DEBUG2(m > 0 && n > 0, "Array Size %d %d", m, n);

    T **outerData = new T *[m];
    CHECK_MEMORY1(outerData != NULL, "Array Allocate %d", m);

    T *innerData = new T[m * n];
    if (!innerData) {
      delete outerData;
      CHECK_MEMORY1(NULL, "Array Allocation", m * n);
    }

    this->m = m;
    this->n = n;
    allocM = m;
    allocN = n;
    data = outerData;
    for (int i = 0; i < m; i++)
      data[i] = &innerData[i * n];
  }

  void free() {
    if (data) {
      delete[] data[0];
      delete[] data;
      data = NULL;
      m = n = 0;
      allocM = allocN = 0;
    }
  }

  //  grow array or shrink if too large (DESTROYS contents)
  void makeRoom(int nM, int nN, int scale = 4) {
    if (allocM < nM || allocN < nN || (allocM * allocN) > (nM * nN * scale)) {
      if (allocM)
        free();
      allocate(nM, nN);
    } else {
      setSize(nM, nN);
    }
  }

  //  index
  __inline const T &index(int i, int j) const {
    CHECK_DEBUG4((unsigned)i < m && (unsigned)j < n,
                 "Array Index Size(%d,%d) Index(%d, %d)", m, n, i, j)
    return data[i][j];
  }

  __inline T &index(int i, int j) {
    CHECK_DEBUG4((unsigned)i < m && (unsigned)j < n,
                 "Array Index Size(%d,%d) Index(%d, %d)", m, n, i, j)
    return data[i][j];
  }

  //  clear
  void clear() {
    for (int i = 0; i < m; i++)
      memset(data[i], 0, n * sizeof(T));
  }

  //  clear contents safe for all types objects must support cast from void
  void clearSafe(Array2D<T> *a) {
    T nullOne;
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        data[i][j] = nullOne;
  }

  //  copy (use memcpy)
  void copy(const Array2D<T> &other) {
    CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
                 "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
                 other.n);

    for (int i = 0; i < other.m; i++)
      memcpy(this->data[i], other.data[i], other.n * sizeof(T));

    m = other.m;
    n = other.n;
  }

  //  copy safe - ok for objects
  void copySafe(const Array2D<T> &other) {
    CHECK_DEBUG4(allocM >= other.m && allocN >= other.n,
                 "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, other.m,
                 other.n);

    for (int i = 0; i < other.m; i++)
      for (int j = 0; j < other.n; j++)
        data[i][j] = other.data[i][j];

    m = other.m;
    n = other.n;
  }

  //  size
  __inline void setSize(int nM, int nN) {
    CHECK_DEBUG4(nM <= allocM && nN <= allocN,
                 "Array Size (%d, %d) <- (%d, %d)", allocM, allocN, nM, nN);
    m = nM;
    n = nN;
  }

  __inline int getM() const { return m; }

  __inline int getN() const { return n; }

  __inline int getAllocM() const { return allocM; }

  __inline int getAllocN() const { return allocN; }
};

#endif
