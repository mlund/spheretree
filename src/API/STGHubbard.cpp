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

#include "STGHubbard.h"
#include "MergeHubbard.h"

void STGHubbard::setup(Voronoi3D *vor, const MedialTester *mt) {
  this->vor = vor;
  this->mt = mt;
}

void STGHubbard::constructTree(SphereTree *st) const {
  CHECK_DEBUG(vor != NULL && mt != NULL, "need to setup");

  //  setup merge
  MergeHubbard merger;
  merger.setup(vor, mt);

  //  make the bounding sphere
  merger.getBoundingSphere(&st->nodes.index(0));

  //  construct the tree
  if (st->levels > 1)
    buildTree(st, &merger, 0, 0, NULL);
}

void STGHubbard::buildTree(SphereTree *st, MergeHubbard *merger, int level,
                           int node, const Array<int> *parSphInd) const {
  //  temp
  printf("Doing Sphere Node %d (level %d) \n", node, level);

  //  construct the medial set of spheres for the parent
  Array<MergeHubbard::MedialSphere> medialSpheres;
  merger->constructSphereSet(&medialSpheres, parSphInd);

  //  do the merges
  merger->iterativeMerge(&medialSpheres, st->degree);

  //  fill in the tree and get the lists of spheres for children
  Array<Array<int> /**/> inds;
  inds.resize(st->degree);
  int firstChild = st->getFirstChild(node);
  int numMedial = medialSpheres.getSize();

  int oI = 0;
  for (int i = 0; i < numMedial; i++) {
    MergeHubbard::MedialSphere *ms = &medialSpheres.index(i);
    if (ms->valid) {
      st->nodes.index(firstChild + oI).c = ms->s.c;
      st->nodes.index(firstChild + oI).r = ms->s.r;

      if (level < st->levels - 2)
        inds.index(oI).clone(ms->vertices);

      oI++;
      if (oI > st->degree)
        break; //    no point continuing
    }
  }

  //  free up the medial spheres
  medialSpheres.free();

  //  recurse to do children
  if (level < st->levels - 2) {
    for (int i = 0; i < oI; i++)
      buildTree(st, merger, level + 1, firstChild + i, &inds.index(i));
  }
}