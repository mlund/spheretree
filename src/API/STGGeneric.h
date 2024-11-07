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
    Generic sphere construction algorithm for a model sampled by a set of points
*/
#ifndef _GENERIC_SPHERE_TREE_GENERATOR_H_
#define _GENERIC_SPHERE_TREE_GENERATOR_H_

#include "STGBase.h"
#include "SRBase.h"
#include "SOBase.h"
#include "SFBase.h"
#include "MSBase.h"
#include "SurfaceDiv.h"

class STGGeneric /*: public STGBase*/ {
public:
  //  parameters
  const SRBase *reducer;
  const SOBase *optimiser;
  const SEBase *eval;
  bool useRefit;
  int minLeaves;
  int maxOptLevel;

  //  constructor
  STGGeneric();
  void setSamples(const Array<Surface::Point> &surPts) {
    surfacePoints = &surPts;
  }

  //  generate
  void constructTree(SphereTree *st) const;
  void constructTreeRecursive(SphereTree *st) const;

private:
  //  set of points for coverage
  const Array<Surface::Point> *surfacePoints;

  // recursive function
  void makeChildren(SphereTree *st, int node, int level,
                    const SurfaceRep &surRep) const;
};

#endif