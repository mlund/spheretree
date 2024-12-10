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
    Transformation  in 3D Euclidean space,

        [ROW][COL] indexing

        | R R R T |
        | R R R T |
        | R R R T |
        | 0 0 0 1 |
*/
#ifndef _TRANSFORM_3D_
#define _TRANSFORM_3D_

#include "Point2D.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "../Exceptions/CheckDebug.h"
#include <memory.h>

struct Transform3D {
protected:
  REAL data[4][4];

public:
  //  indexing
  __inline const REAL &index(unsigned int i, unsigned int j) const {
    CHECK_DEBUG2(i < 4 && j < 4, "Transform Index (%d, %d)", i, j);
    return data[i][j];
  }

  __inline REAL &index(unsigned int i, unsigned int j) {
    CHECK_DEBUG2(i < 4 && j < 4, "Transform Index (%d, %d)", i, j);
    return data[i][j];
  }

  //  copy
  __inline void assign(const Transform3D &tr) {
    memcpy(data, tr.data, 16 * sizeof(REAL));
  }

  //  setup
  void identity();
  void setAngleX(REAL ang);
  void setAngleY(REAL ang);
  void setAngleZ(REAL ang);

  __inline void setTranslate(REAL x, REAL y, REAL z) {
    data[0][3] = x;
    data[1][3] = y;
    data[2][3] = z;
  }

  //  application
  void transform(Point3D *dest, const Point3D &src) const;
  void transform(Vector3D *dest, const Vector3D &src) const;

  //  this = t1 after t2
  void compose(const Transform3D &t1, const Transform3D &t2);

  //  this = t1 after this
  __inline void compose(const Transform3D &t1) {
    Transform3D trTmp(*this);
    compose(t1, trTmp);
  }

  //  reverse transform
  bool invert(const Transform3D &t);
  bool invertAffine(const Transform3D &t);

  //  alignment transform
  void alignToZ(/*const*/ Vector3D /*&*/ v);
  void alignToZ(const Vector3D &v, const Point3D &p);
  void transformTo2D(Point2D *p, const Point3D &q) const;
  void transformFrom2D(Point3D *p, const Point2D &q) const;
};

#endif
