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

#include "SRMerge.h"
#include "SFRitter.h"
#include "SEPoint.h"
#include "RELargest.h"
#include "../Export/POV.h"

//#define ONE_SPHERE_PER_POINT

//  constructor
SRMerge::SRMerge(){
  sphereFitter = NULL;
  sphereEval = NULL;
  doAllBelow = 20;
  doRedundantCheckBelow = -1;
  redElim = NULL;
  useFormingPoints = false;
  useBeneficial = false;
}

//  reduce sphere set
void SRMerge::getSpheres(Array<Sphere> *spheres, int numDest, const SurfaceRep &surRep, const Sphere *filterSphere, float parSphereErr) const{
  CHECK_DEBUG(sphereFitter != NULL, "need sphere fitter");
  CHECK_DEBUG(sphereEval != NULL, "need sphere evaluator");

#ifdef ONE_SPHERE_PER_POINT
  bool onlyOneSpherePerPoint = true;
#else
  bool onlyOneSpherePerPoint = false;
#endif
 
  //  construct spheres from medial axis
  Array<MedialSphere> medialSpheres;
  constructSphereSet(&medialSpheres, surRep, filterSphere, parSphereErr, onlyOneSpherePerPoint);
  int numSpheres = medialSpheres.getSize();

  //  reconfig is using forming points
  const Array<Surface::Point> *coverRep = surRep.getSurPts();
  Array<Surface::Point> localCoverPts;
  if (useFormingPoints){
    //  setup local coverPts
    int numForming = vor->formingPoints.getSize();
    localCoverPts.resize(numForming);
    for (int i = 0; i < numForming; i++){
      localCoverPts.index(i).p = vor->formingPoints.index(i).p;
      localCoverPts.index(i).n = vor->formingPoints.index(i).v;
      }

    //  replace the coverRep
    coverRep = &localCoverPts;

    //  replace point indices for the medial spheres
    for (int i = 0; i < numSpheres; i++){
      //  get medial sphere
      MedialSphere *ms = &medialSpheres.index(i);

      //  get voronoi vertex
      Voronoi3D::Vertex *vert = &vor->vertices.index(ms->vertexNum);

      //  setup forming points
      ms->pts.resize(4);
      for (int i = 0; i < 4; i++)
        ms->pts.index(i) = vert->f[i];
      }
    }

  //  setup costs of the spheres
  initErrors(&medialSpheres, sphereEval);

  //  construct mergers
  Array<Merger> mergers;
  constructMergers(&mergers, medialSpheres, coverRep);

  //  dump neighbour info
  for (int i = 0; i < numSpheres; i++){
    MedialSphere *ms = &medialSpheres.index(i);
    if (ms->neighbours.getSize())
      ms->neighbours.free();
    }

  //  do reduction
  bool allDone = false;
  int initNumSpheres = numSpheres;
  float firstError = 0, errFact = 0, errMult = 0;
  while (numSpheres > numDest){
    int numMergers = mergers.getSize();

    //  when we get close to destination make all possible mergers
    if (numMergers == 0 || (!allDone && numSpheres <= doAllBelow)){  
      constructAllMergers(&mergers, medialSpheres, coverRep);
      numMergers = mergers.getSize();
      allDone = true;
      }

    int bestI = -1;
    float minErr = -1;
    if (useBeneficial){
      //  find the worst error
      double worstError = 0;
      for (int i = 0; i < numMergers; i++){
        const Merger *m = &mergers.index(i);
        const MedialSphere *ms1 = &medialSpheres.index(m->i1);
        const MedialSphere *ms2 = &medialSpheres.index(m->i2);
        double error = ms1->error > ms2->error ? ms1->error : ms2->error;
        if (error > worstError)
          worstError = error;
        }

      //  try to find improvement to worst error
      float minCost = 0;      
      for (int i = 0; i < numMergers; i++){
        const Merger *m = &mergers.index(i);
        const MedialSphere *ms1 = &medialSpheres.index(m->i1);
        const MedialSphere *ms2 = &medialSpheres.index(m->i2);
        double error = ms1->error > ms2->error ? ms1->error : ms2->error;
        if (fabs(error - worstError) < 1E-10 && m->cost < minCost){  //  -ve cost means higher saving
          minCost = m->cost;
          bestI = i;
          }
        }
      }

    if (bestI < 0){
      //  fall back to the merger with the lowest error
      minErr = REAL_MAX;
      for (int i = 0; i < numMergers; i++){
        const Merger *m = &mergers.index(i);
        if (m->newErr < minErr /*m->cost < minErr*/ ){
          minErr = m->newErr; /*m->cost*/;
          bestI = i;
          }
        }
      }

    //saveImage(medialSpheres, mergers.index(bestI), 50);

    //  do merger
    if (numSpheres % 25 == 0)
      OUTPUTINFO("NumSph = %d, MinCost = %f\n", numSpheres, minErr);
    applyMerger(&mergers, &medialSpheres, coverRep, bestI, (numSpheres-1) > numDest);
    numSpheres--;

    if (func)
      func(medialSpheres);

    if (numSpheres < doRedundantCheckBelow){  //  shouldn't really be used with useForming
      Array<Sphere> srcSph;
      int numSph = medialSpheres.getSize();
      for (int i = 0; i < numSph; i++)
        if (medialSpheres.index(i).valid)
          srcSph.addItem() = medialSpheres.index(i).s;

      RELargest elimLargest;
      REBase *elim = &elimLargest;
      if (this->redElim)
        elim = this->redElim;

      if(elim->reduceSpheres(spheres, srcSph, surRep, numDest)){
        OUTPUTINFO("Redundancy Test Got %d Spheres\n", spheres->getSize());
        return;
        }
      }
    }

  spheres->setSize(0);
  int numSph = medialSpheres.getSize();
  for (int i = 0; i < numSph; i++)
    if (medialSpheres.index(i).valid)
      spheres->addItem() = medialSpheres.index(i).s;
}

void SRMerge::constructMerger(Merger *merger, const Array<MedialSphere> &medialSpheres, const Array<Surface::Point> *surPts, int i1, int i2) const{
  //  setup merger
  merger->i1 = i1;
  merger->i2 = i2;

  //  get the spheres
  const MedialSphere *ms1 = &medialSpheres.index(i1);
  const MedialSphere *ms2 = &medialSpheres.index(i2);

  if (!ms1->valid || !ms2->valid)
    OUTPUTINFO("Constructing merger between invalid spheres\n");

  //  merge point lists
  Array<int> mergedPts;
  combineLists(&mergedPts, ms1->pts, ms2->pts);

  //  refit around sphere with min error
  //Point3D pC = ms1->s.c;
  //if (ms2->error < ms1->error)
  //  pC = ms2->s.c;

  //  make the new sphere
  merger->newErr = refitSphere(&merger->s, *surPts, mergedPts, sphereEval, sphereFitter, /*&pC*/&ms1->s.c, &ms2->s.c);

  //  compute the cost of the merger
  double maxOldError = ms1->error > ms2->error? ms1->error:ms2->error;
  if (!useBeneficial || merger->newErr > maxOldError)
    merger->cost = merger->newErr;
  else
    merger->cost = merger->newErr - maxOldError;
}

void SRMerge::constructMergers(Array<Merger> *mergers, const Array<MedialSphere> &medialSpheres, const Array<Surface::Point> *surPts) const{
  mergers->setSize(0);
  int numSpheres = medialSpheres.getSize();
  for (int i = 0; i < numSpheres; i++){
    const MedialSphere *ms = &medialSpheres.index(i);
    int numNeigh = ms->neighbours.getSize();
    for (int j = 0; j < numNeigh; j++){
      int neighNum = ms->neighbours.index(j);
      if (neighNum > i){
        Merger *merger = &mergers->addItem();
        constructMerger(merger, medialSpheres, surPts, i, neighNum);
        }
      }
    }
}

void SRMerge::constructAllMergers(Array<Merger> *mergers, const Array<MedialSphere> &medialSpheres, const Array<Surface::Point> *surPts) const{
  int numSpheres = medialSpheres.getSize();

  //  make merger for each pair of spheres
  mergers->setSize(0);
  for (int i = 0; i < numSpheres; i++){
    if (medialSpheres.index(i).valid){
      for (int j = i+1; j < numSpheres; j++){
        if (medialSpheres.index(j).valid){
          //  construct new merger
          Merger *m = &mergers->addItem();
          constructMerger(m, medialSpheres, surPts, i, j);
          }
        }
      }
    }
}

void SRMerge::applyMerger(Array<Merger> *mergers, Array<MedialSphere> *medialSpheres, const Array<Surface::Point> *surPts, int mergI, bool updateHouseKeeping) const{
  int numMergs = mergers->getSize();
  Merger *merger = &mergers->index(mergI);
  int i1 = merger->i1, i2 = merger->i2;
  if (i1 > i2){
    int tmp = i1;
    i1 = i2;
    i2 = tmp;
    }
  MedialSphere *s1 = &medialSpheres->index(i1);
  MedialSphere *s2 = &medialSpheres->index(i2);

  if (!s1->valid || !s2->valid)
    OUTPUTINFO("One of these sphere is already invalid\n");

  //  update S1 with merged sphere
  s1->s = merger->s;
  s1->error = merger->newErr;

  //  merge lists of forming points and vertices
#ifdef ONE_SPHERE_PER_POINT
  s1->pts.append(s2->pts);
#else
  mergeLists(&s1->pts, s2->pts);
#endif

  //  remove merger as sJ will map to sI, can't merge sphere to itself
  if (mergI != numMergs-1)
    mergers->index(mergI) = mergers->index(numMergs-1);
  mergers->setSize(--numMergs);

  //  mark s2 as dead
  s2->valid = false;
  s2->pts.setSize(0);

  if (updateHouseKeeping){
    //  list of mergers already re-done
    Array<int> mergesWithI1;

    //  update merges
    int numNewMergers = 0;
    for (int i = 0; i < numMergs; i++){
      Merger *m = &mergers->index(i);
      //  if merger involved i1 or i2, update
      if (m->i1 == i1 || m->i2 == i1 || m->i1 == i2 || m->i2 == i2){
        //  remap i2 to i1
        if (m->i1 == i2)
          m->i1 = i1;
        else if (m->i2 == i2)
          m->i2 = i1;

        //  find index which isn't i1
        int otherI;
        if (m->i1 == i1)
          otherI = m->i2;
        else
          otherI = m->i1;

        if (mergesWithI1.inList(otherI)){
          //  merge already done, delete merge
          if (i != numMergs-1)
            mergers->index(i) = mergers->index(numMergs-1);
          mergers->setSize(numMergs-1);
          numMergs--;
          i--;
          }
        else{
          //  update merge
          numNewMergers++;
          constructMerger(m, *medialSpheres, surPts, i1, otherI);
          mergesWithI1.addItem() = otherI;
          }
        }
      }

    //  check for unmergable spheres
    if (numNewMergers == 0){
      int numSph = medialSpheres->getSize(), numNew = 0;
      for (int i = 0; i < numSph; i++){
        const MedialSphere *sI = &medialSpheres->index(i);
        if (i != i1 && sI->valid){
          if (s1->s.overlap(sI->s)){
            int iA = i1, iB = i;
            if (i < i1){
              iA = i;
              iB = i1;
              }

            numNew++;
            Merger *newMerge = &mergers->addItem();
            constructMerger(newMerge, *medialSpheres, surPts, iA, iB);        
            }
          }
        }

      //  if no mergers drop overlap criteria
      if (numNew == 0){
        for (int i = 0; i < numSph; i++){
          const MedialSphere *sI = &medialSpheres->index(i);
          if (i != i1 && sI->valid){
            int iA = i1, iB = i;
            if (i < i1){
              iA = i;
              iB = i1;
              }

            numNew++;
            Merger *newMerge = &mergers->addItem();
            constructMerger(newMerge, *medialSpheres, surPts, iA, iB);        
            }
          }
        }
      }
    }
}

//  statics
void SRMerge::mergeLists(Array<int> *dest, const Array<int> &l){
  int numL = l.getSize();
  int numDest = dest->getSize();
  for (int i = 0; i < numL; i++){
    int n = l.index(i);

    //  check for duplicate assumes dest list contained no dups
    int j;
    for (j = 0; j < numDest; j++)
      if (dest->index(j) == n)
        break;

    //  unique entry
    if (j == numDest)
      dest->addItem() = n;
    }
}

void SRMerge::combineLists(Array<int> *dest, const Array<int> &l1, const Array<int> &l2){
  //  don't bother removing duplicates
  dest->clone(l1);
  dest->append(l2);
}


/*

  Illustrate the mergers 

  NOTE 1.0f/1000.0f is the scale of the output as 1000 is the default size we load model
                    not very generic - but it is the day before my Viva
*/
void SRMerge::saveImage(const Array<MedialSphere> &medialSpheres, const Merger &merger, int maxNum) const{
  Array<int> sel;
  Array<Sphere> spheres;

  float colorPre[] = {0, 0.75, 0};
  float colorPost[] = {0.75, 0, 0};

  int numMed = medialSpheres.getSize();
  for (int i = 0; i < numMed; i++){
    const MedialSphere *ms = &medialSpheres.index(i);
    if (ms->valid && i != merger.i1 && i != merger.i2)
      spheres.addItem() = ms->s;
    }

  int numValid = spheres.getSize() + 2;
  if (numValid < maxNum-1){
    char buffer[1024];

    //  do pre merge
    int newSph = spheres.addIndex();
    spheres.index(newSph) = medialSpheres.index(merger.i1).s;
    sel.addItem() = newSph;
    newSph = spheres.addIndex();
    spheres.index(newSph) = medialSpheres.index(merger.i2).s;
    sel.addItem() = newSph;
    sprintf(buffer, "c:/devel/mergePics/mergePic-%.4d-A.pov", maxNum-numValid);
    exportSpheresPOV(buffer, spheres, 1.0f/1000, true, NULL, &sel, colorPre);

    //  reset lists
    spheres.resize(numValid - 2);
    sel.resize(0);
    
    //  do post merge
    newSph = spheres.addIndex();
    spheres.index(newSph) = merger.s;
    sel.addItem() = newSph;
    sprintf(buffer, "c:/devel/mergePics/mergePic-%.4d-B.pov", maxNum-numValid);
    exportSpheresPOV(buffer, spheres, 1.0f/1000, true, NULL, &sel, colorPost);
    }
}

