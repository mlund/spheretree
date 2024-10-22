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
    Transformation  in 2D Euclidean space,

        [ROW][COL] indexing

        | R R T |
        | R R T |
        | 0 0 1 |
*/
#ifndef _TRANSFORM_2D_
#define _TRANSFORM_2D_

#include "Point2D.h"
#include "Vector2D.h"
#include "../Exceptions/CheckDebug.h"

struct Transform2D {
protected:
  REAL data[3][3];

public:
  //  indexing
  __inline const REAL &index(unsigned int i, unsigned int j) const {
    CHECK_DEBUG2(i < 3 && j < 3, "Transform Index (%d, %d)", i, j);
    return data[i][j];
  }

  __inline REAL &index(unsigned int i, unsigned int j) {
    CHECK_DEBUG2(i < 3 && j < 3, "Transform Index (%d, %d)", i, j);
    return data[i][j];
  }

  //  copy
  void assign(const Transform2D &tr);

  //  setup
  void identity();
  void setup(REAL ang, REAL tX, REAL tY);
  void setAngle(REAL ang);

  __inline void setTranslate(REAL x, REAL y) {
    data[0][2] = x;
    data[1][2] = y;
  }

  //  application
  void transform(Point2D *dest, const Point2D &src) const;
  void transform(Vector2D *dest, const Vector2D &src) const;

  //  this = t1 after t2
  void compose(const Transform2D &t1, const Transform2D &t2);

  //  this = t1 after this
  void compose(const Transform2D &t1);

  //  reverse transform
  bool invert(const Transform2D &t);
  bool invertAffine(const Transform2D &t);
};

#endif
