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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdio.h>
#include "../Storage/Array2D.h"

class Matrix : public Array2D<REAL> {
public:
  Matrix() {}
  Matrix(int m, int n);
  Matrix(const Matrix &other);

  //  init
  void identity();

  //  assign
  void assign(const Matrix &other);

  //  transpose
  void transpose();
  void transpose(const Matrix &other);

  // addition
  void add(const Matrix &other, REAL s = 1.0);
  void add(const Matrix &m1, const Matrix &m2);

  //  subtraction
  void sub(const Matrix &other, REAL s = 1.0);
  void sub(const Matrix &m1, const Matrix &m2);

  //  multipliication
  void mult(REAL s);
  void mult(const Matrix &other, REAL s);
  void mult(const Matrix &m1, const Matrix &m2);

  //  inverse
  bool invert(const Matrix &other);

  //  determinat
  REAL det() const;

  //  output
  void print(FILE *f, const char *msg = NULL) const;
};

#endif
