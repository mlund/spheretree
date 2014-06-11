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

#include "STGGeneric.h"
#include "SFWhite.h"
#include "MSGrid.h"
#include "RELargest.h"
#include "SOPerSphere.h"

#define PTS_PER_CELL 10

STGGeneric::STGGeneric(){
  optimiser = NULL;
  reducer = NULL;
  eval = NULL;
  useRefit = false;
  minLeaves = -1;
  surfacePoints = NULL;
  maxOptLevel = -1;
}

//  generate recursively
void STGGeneric::constructTreeRecursive(SphereTree *st) const{
  CHECK_DEBUG0(st != NULL && st->degree > 1 && st->levels >= 1);
  CHECK_DEBUG0(reducer != NULL);
  CHECK_DEBUG0(eval != NULL);
  CHECK_DEBUG0(surfacePoints != NULL);

  //  make cells to have 10 pts each, most will have alot more
  int numCells = surfacePoints->getSize() / PTS_PER_CELL;
  int gridDim = ceil(pow(numCells, 1.0 / 3.0));
  OUTPUTINFO("numCells = %d, gridDim = %d\n", numCells, gridDim);

  //  create surface rep
  SurfaceRep surRep;
  surRep.setup(*surfacePoints, gridDim);

  //  bounding sphere for root
  SFWhite::makeSphere(&st->nodes.index(0), *surfacePoints);

  //  setup for the base level (should really do for 
  //  all levels - but doesn't fit in recursive form)
  reducer->setupForLevel(0, st->degree, &surRep);

  //  make children nodes - using recursive algorithm
  if (st->levels > 1)
    makeChildren(st, 0, 1, surRep);
}

void STGGeneric::makeChildren(SphereTree *st, int node, int level, const SurfaceRep &surRep) const{
  //  get the error of the parent
  Sphere parS = st->nodes.index(node);
  double parErr = eval->evalSphere(parS);

  //  get minimum bounding sphere for points to give to reducer
  Sphere boundingSphere;
  SFWhite::makeSphere(&boundingSphere, *surRep.getSurPts());

  //  generate the set of child spheres
  Array<Sphere> initChildren, children;
  reducer->getSpheres(&initChildren, st->degree, surRep, &boundingSphere, parErr);

  //  do sphere refit - local optimisation
  if (useRefit){
    OUTPUTINFO("Refitting\n");
    SOPerSphere perSphere;
    perSphere.numIter = 3;
    perSphere.eval = eval;
    perSphere.optimise(&initChildren, surRep);
    }

  //  apply optimiser if required
  if (optimiser && (maxOptLevel < 0 || level <= maxOptLevel))
    optimiser->optimise(&initChildren, surRep, -1, &parS, level-1);

  //  remove redundent spheres
  RELargest reLargest;
  if (!reLargest.reduceSpheres(&children, initChildren, surRep))
    children.clone(initChildren);

  int numChildren = children.getSize();
  if (numChildren == 0)
    return;

  //  get the points that this node covers
  const Array<Surface::Point> *surPts = surRep.getSurPts();
  int numPts = surPts->getSize();

  //  info for areas to be covered by each sphere
  Array<Array<Surface::Point> > subPts(numChildren);
  Array<bool> covered(numPts);
  covered.clear();

  //  make the divisions between the children
  SurfaceDivision surDiv;
  surDiv.setup(children, surPts);

  //  do the children
  int firstChild = st->getFirstChild(node);
  for (int i = 0; i < numChildren; i++){
    //  get sphere
    Sphere s = children.index(i);

    //  list the points in the sphere
    Array<int> listPoints;
    surRep.listContainedPoints(&listPoints, NULL, s, NULL);
    int numList = listPoints.getSize();

    //  filter points
    Array<Surface::Point> *filterPts = &subPts.index(i);
    filterPts->resize(0);
    for (int j = 0; j < numList; j++){
      //  get point
      int pI = listPoints.index(j);
      Surface::Point p = surPts->index(pI);

      //  check if it's in the region
      if (surDiv.pointInRegion(p.p, i)){
        covered.index(j) = true;
        filterPts->addItem() = p;
        }
      }
    }

  //  count/cover uncovered points
  for (int i = 0; i < numPts; i++){
    if (!covered.index(i)){
      //  get the point
      Point3D p = surPts->index(i).p;

      //  find the closest sphere
      int closestJ = -1;
      float closestD = 0;
      for (int j = 0; j < numChildren;  j++){
        Sphere s = children.index(j);
        float d = p.distance(s.c) - s.r;
        if (d < closestD){
          closestJ = j;
          closestD = d;
          }
        }

      subPts.index(closestJ).addItem() = surPts->index(i);
      }
    }

  //  store spheres & recurse to children 
  int childNum = firstChild;
  for (int i = 0; i < numChildren; i++){
    if (subPts.index(i).getSize() > 1){
      //  recreate the sphere
      Sphere s = children.index(i);

      //  add sphere to tree
      st->nodes.index(childNum).c = s.c;
      st->nodes.index(childNum).r = s.r;

      if (level < st->levels-1 && numChildren > 1){
        const Array<Surface::Point> *pts = &subPts.index(i);

        //  make cells to have 10 pts each, most will have alot more
        int numCells = pts->getSize() / PTS_PER_CELL;
        int gridDim = ceil(pow(numCells, 1.0 / 3.0));
        OUTPUTINFO("numCells = %d, gridDim = %d\n", numCells, gridDim);

        //  make children by recursion
        SurfaceRep subRep;
        subRep.setup(*pts, gridDim);

        makeChildren(st, childNum, level+1, subRep);
        }

      childNum++;
      }
    }

  //  NULL out the rest of the spheres
  for (int i = childNum; i < st->degree; i++)
    st->initNode(firstChild+i, level+1);
}

//  generate breadth first
void STGGeneric::constructTree(SphereTree *st) const{
  CHECK_DEBUG0(st != NULL && st->degree > 1 && st->levels >= 1);
  CHECK_DEBUG0(reducer != NULL);
  CHECK_DEBUG0(eval != NULL);
  CHECK_DEBUG0(surfacePoints != NULL);

  //  NULL out the entire tree
  st->initNode(0);

  //  make cells to have 10 pts each, most will have alot more
  int numCells = surfacePoints->getSize() / PTS_PER_CELL;
  int gridDim = ceil(pow(numCells, 1.0 / 3.0));
  OUTPUTINFO("numCells = %d, gridDim = %d\n", numCells, gridDim);

  SurfaceRep surRep;
  surRep.setup(*surfacePoints, gridDim);

  //  bounding sphere for root - should use vertices
  SFWhite::makeSphere(&st->nodes.index(0), *surfacePoints);

  //  list of points to be covered by each node
  unsigned long start, num;
  st->getRow(&start, &num, st->levels-1);
  Array<Array<int>/**/> pointsInSpheres;
  pointsInSpheres.resize(st->nodes.getSize() - num);

  //  initialise the list for the first node
  int numPts = surfacePoints->getSize();
  Array<int> *list0 = &pointsInSpheres.index(0);
  list0->resize(numPts);
  for (int i = 0; i < numPts; i++)
    list0->index(i) = i;

  //  process the remaining levels
  int numLeaves = 0;
  for (int level = 0; level < st->levels-1; level++){
    //  get the positions of the nodes
    unsigned long start, num;
    st->getRow(&start, &num, level);

    //  update samples etc. for this level
    reducer->setupForLevel(level, st->degree, &surRep);

    //  get the errors for all the spheres in that level
    int numActualSpheres = 0;
    double averageError = 0;
    Array<double> sphereErrors(num);
    for (int i = 0; i < num; i++){
      Sphere s = st->nodes.index(start+i);
      if (s.r >= 0){
        double err = eval->evalSphere(s);
        sphereErrors.index(i) = err;
        averageError += err;
        numActualSpheres++;
        }
      else 
        sphereErrors.index(i) = -1;
      }
    averageError /= numActualSpheres;
    if (level != 0 && numActualSpheres <= 1){
      numLeaves++;
      continue;     //  there is only one sphere here - will never improve
      }

    //  process each node's to make children
    int maxNode = -1;
    double maxR = -1;
    int levelChildren = 0;
    for (int nodeI = 0; nodeI < num; nodeI++){
      //OUTPUTINFO("Level = %d, node = %d\n", level, nodeI);
      printf("Level = %d, node = %d\n", level, nodeI);

      int node = nodeI + start;
      if (st->nodes.index(node).r <= 0){
          OUTPUTINFO("R = %f\n", st->nodes.index(node).r);
          st->initNode(node);
          continue;
          }

/*
      //  hack to do largest sphere at each run - gives guide to good params
      double r = st->nodes.index(node).r;
      if (r > maxR){
        maxR = r;
        maxNode = node;
        }
      }

      nodeI = maxNode - start;
      int node = maxNode;{
      //  end hack
*/

      //  make the set of surface poitns to be covered by this sphere
      Array<int> *selPtsI = &pointsInSpheres.index(node);
      int numSelPts = selPtsI->getSize();
      Array<Surface::Point> selPts(numSelPts);
      for (int i = 0; i < numSelPts; i++)
        selPts.index(i) = surfacePoints->index(selPtsI->index(i));

      //  get filter sphere
      Sphere s;
      SFWhite::makeSphere(&s, selPts);

      //  make cells to have 10 pts each, most will have alot more
      int numCells = numSelPts / PTS_PER_CELL;
      int gridDim = ceil(pow(numCells, 1.0 / 3.0));
      OUTPUTINFO("%d Points\n", numSelPts);
      OUTPUTINFO("numCells = %d, gridDim = %d\n", numCells, gridDim);

      //  make new SurfaceRepresentation
      SurfaceRep subRep;
      subRep.setup(selPts, gridDim);

      //  compute error for this sphere
      double err = sphereErrors.index(nodeI);
      if (err > averageError)
        err = averageError;  //  improve the bad ones a bit more

      //  generate the children nodes
      Array<Sphere> initChildren, children;
      reducer->getSpheres(&initChildren, st->degree, subRep, &s, err);

      //  apply optimiser if required
      if (optimiser && (maxOptLevel < 0 || level <= maxOptLevel)){
printf("RUNNING OPTIMISER...\n");
        optimiser->optimise(&initChildren, subRep, -1, &s, level);
printf("DONE OPTIMISING...\n");
        }

      //  do sphere refit - local optimisation
      if (useRefit){
        OUTPUTINFO("Refitting\n");
        SOPerSphere perSphere;
        perSphere.numIter = 3;
        perSphere.eval = eval;
        perSphere.optimise(&initChildren, subRep);
        }

      //  remove redundent spheres
      RELargest reLargest;
      if (!reLargest.reduceSpheres(&children, initChildren, subRep))
        children.clone(initChildren);

      //  setup the node's sub-division (make the regions to be covered by children)
      //subDivs.index(node).setup(children, &selPts);
      SurfaceDivision surDiv;
      surDiv.setup(children, &selPts);

      //  list of points that are covered
      Array<bool> covered(numSelPts);
      covered.clear();

      //  create the new nodes and their the points to cover
      int numChildren = children.getSize();
      int firstChild = st->getFirstChild(node);
      levelChildren += numChildren;
      for (int i = 0; i < numChildren; i++){
        int childNum = firstChild + i;

        //  get sphere
        Sphere s = children.index(i);

        //  add sphere to tree
        st->nodes.index(childNum).c = s.c;
        st->nodes.index(childNum).r = s.r;

        if (level < st->levels-2){
          //  get the points in this sphere
          Array<int> pointsInS;
          subRep.listContainedPoints(&pointsInS, NULL, s);
          int numInS = pointsInS.getSize();

          //  populate list of points in sphere
          Array<int> *pointsToCover = &pointsInSpheres.index(childNum);
          pointsToCover->resize(0);   //  just in case
          for (int j = 0; j < numInS; j++){
            int pI = pointsInS.index(j);
            if (surDiv.pointInRegion(selPts.index(pI).p, i)){
              pointsToCover->addItem() = selPtsI->index(pI);
              covered.index(pI) = true;
              }
            }
          }
        }

      //  assign uncovered points
      if (numChildren > 0 && level < st->levels-2){
        for (int i = 0; i < numSelPts; i++){
          if (!covered.index(i)){
            //  get point
            Point3D pt = selPts.index(i).p;

            //  find the sphere
            int minJ = -1;
            float minD = FLT_MAX;
            for (int j = 0; j < numChildren; j++){
              Sphere s = children.index(j);
              float d = pt.distance(s.c);// - s.r;
              if (d < minD){
                minD = d;
                minJ = j;
                }
              }

            //  add the point to the sphere's list
            pointsInSpheres.index(firstChild+minJ).addItem() = selPtsI->index(i);
            }
          }
        }

      //  save after each child set
      //st->saveSphereTree("saved.sph");
      }

    //  save after each level
    //st->saveSphereTree("saved.sph");

    //  see if we need to add another level
    if (level == st->levels - 2 && minLeaves > 0 && numLeaves + levelChildren < minLeaves){
      //  grow the tree
      OUTPUTINFO("Growing the tree : %d-->%d\n", st->levels, st->levels+1);
      OUTPUTINFO("New Nodes : %d-->", st->nodes.getSize());
      st->growTree(st->levels+1);
      OUTPUTINFO("%d\n", st->nodes.getSize());
      }
    }
}
