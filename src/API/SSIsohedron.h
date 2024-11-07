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
    Generate sample points on a sphere using an isohedron to approximate it.
    Level determines the level of sub-division in the isohedron algorithm

        0 = 12 points etc.
*/
#ifndef _ISOHEDRON_SPHERE_SAMPLER_H_
#define _ISOHEDRON_SPHERE_SAMPLER_H_

#include "SSBase.h"

class SSIsohedron : public SSBase {
public:
  //  constructor
  SSIsohedron(int level = 0);

  //  setup
  void setLevel(int level);

  //  sample
  void generateSamples(Array<Point3D> *pts) const;
  static void generateSamples(Array<Point3D> *pts, int levels);

private:
  int levels;

  //  data for initial isohedron
  static float vdata[12][3];
  static int tindices[20][3];
  static void subDivideIso(Array<Point3D> *pts, int v0, int v1, int v2,
                           int depth);
};

#endif