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
    Incremental Algorithm for constructing 3D Voronoi Diagrams
*/
#ifndef _VORONOI_3D_H_
#define _VORONOI_3D_H_

#include "../Geometry/Point3D.h"
#include "../Geometry/Vector3D.h"
#include "../Storage/Array.h"
#include "../Base/ProcMon.h"
#include "../Surface/Surface.h"
#include "../Surface/SpacialHash.h"
#include "../Base/Defs.h"

/*
    Flags used by this proceedure

      0 - uncategorised
      1 - internal
      2 - external
      3 - internal and processed (for splitting etc)
      4 - coverage sphere
      5 - coverage sphere and processed (should have been removed)
*/
#define VOR_FLAG_UNKNOWN 0
#define VOR_FLAG_INTERNAL 1
#define VOR_FLAG_EXTERNAL 2
#define VOR_FLAG_OLDINTERNAL 3
#define VOR_FLAG_COVER 4
#define VOR_FLAG_OLDCOVER 5

class Voronoi3D {
public:
  //  forming points of diagram
  struct FormingPoint {
    Point3D p;           //  position of point
    Vector3D v;          //  normal of surface at point
    Array<int> vertices; //  list of vertices using this forming point
  };
  Array<FormingPoint> formingPoints;

  //  vertices for the diagram
  struct Vertex {
    // Point3D p;                  //  location of vertex
    // float r;                    //  radius to forming points
    Sphere s;
    int f[4]; //  each verrtex is made from 4 forming points
    int n[4]; //  each vertex has 4 neighbouring vertices

    Point3D closestPt; //  closest point on surface - for adaptive sample
    int closestTri;    //  closest triangle

    int flag;  //  general purpose flag (0 for new)
    float err; //  error associated with vertex (-1 for new)
  };
  Array<Vertex> vertices;

  //  hashings
  SpacialHash formingHash;

public:
  //  setup the diagram with a triangle (base in cw order and top)
  void initialise(const Point3D &pC, double r);
  void initialise(const Point3D &p0, const Point3D &p1, const Point3D &p2,
                  const Point3D &pT);

  //  insert a new forming point
  class Tester {
  public:
    virtual bool test(const Sphere &s) const = 0;
  };

  bool insertPoint(const Point3D &p, const Vector3D &n, int startVertex = -1,
                   const Tester *tester = NULL);
  bool isNeighbour(int v, int n) const;
  void randomInserts(Array<Surface::Point> &pts, int maxNum = -1,
                     ProcessorMonitor *mon = NULL);

  int findCell(const Point3D &pTest, float *dist) const;
  int findCellBrute(const Point3D &pTest, float *dist = NULL) const;

  //  debug stuff
  void draw();
  void dump(const char *fileName) const;

  //  flag management for the adaptive sampling
  void resetFlag(Vertex *vert);

private:
  //  vertex stuff
  int findClosestVertex(const Point3D &p) const;
  bool constructVertex(int s, int f0, int f1, int f2, int f3,
                       int backupVertex = -1, bool storeForming = TRUE);
  bool constructVertexSphere(Sphere *s, int f0, int f1, int f2, int f3,
                             int backupVertex = -1) const;
  void joinVertices(int i, int j);
  void addNeighbour(Vertex *vert, int n);
  void addNeighbour(int dest, int n);
  void replaceNeighbour(Vertex *vert, int oldN, int newN);
  bool isNeighbour(const Vertex *vert, int n) const;

  //  forming point stuff
  int sharedFormingPoints(int s[3], int vI, int vJ) const;
  int sharedFormingPoints(int s[3], const Vertex &vert, int vJ) const;

  //  vertex deletion
  void deleteVertex(int dI, Array<int> &deletable);
  void remap(Array<int> &list, int oldV, int newV, int sI = 0) const;

  //  misc
  void projectThroughFace(Point3D *newP, const Point3D &p, const Point3D &p0,
                          const Point3D &p1, const Point3D &p2) const;

  //  search algorithm
  struct PriorityQueue {
    int i;
    float v;
  };

  void deletableSearch(Array<int> &deletable, Array<bool> &flags,
                       const Point3D &p, int startVertex, bool testALL = true);
  bool validDeletionSet(const Array<int> &deletable, const Array<bool> &flags,
                        int vIN) const;
  double getValue(int vI, const Point3D &p);
  bool isConnected(const Array<int> &nodes) const;
  bool canConnect(const Array<int> &list, int newV) const;

  //  my new search
  bool canAddNeighbour(int dest, int n);
  bool canReplaceNeighbour(Vertex *vert, int oldN, int newN);
  bool validDeletionSetNew(Array<int> &deletable, Array<bool> &flags, int vI,
                           bool complete = false);

  //  check to see that the new vertices will be favourable
  bool validNewVertices(const Array<int> &deletable,
                        const Tester *tester = NULL) const;

  bool hasClosePoint(const Point3D &p, float dist) const;
  bool getCloserForming(int *newForming, float *newDistance,
                        const Point3D &pTest, int currentForming,
                        float currentDistance) const;
};

#endif
