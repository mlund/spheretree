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

#if !defined(AFX_MODELVIEW_H__53B7BE81_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_)
#define AFX_MODELVIEW_H__53B7BE81_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//  MFC includes
#include "ModelDoc.h"
#include "ObjectView.h"

//  ST includes
#include "../../src/MedialAxis/Voronoi3D.h"
#include "../../src/MedialAxis/MedialTester.h"
#include "../../src/SphereTree/SphereTree.h"
#include "../../src/API/SEConvex.h"
#include "../../src/API/SESphPt.h"
#include "../../src/API/SRBase.h"
#include "../../src/API/SOBase.h"

class CModelView : public CObjectView {
protected:
  CModelView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CModelView)

  // Attributes
public:
  // Operations
public:
  // Overrides
  //{{AFX_VIRTUAL(CModelView)
public:
  virtual void OnInitialUpdate();
  //}}AFX_VIRTUAL

  // Implementation
protected:
  virtual ~CModelView();

  // Generated message map functions
protected:
  //{{AFX_MSG(CModelView)
  afx_msg void OnGenerateSamples();
  afx_msg void OnUpdateGenerateSamples(CCmdUI *pCmdUI);
  afx_msg void OnShowModel();
  afx_msg void OnUpdateShowModel(CCmdUI *pCmdUI);
  afx_msg void OnShowSamples();
  afx_msg void OnUpdateShowSamples(CCmdUI *pCmdUI);
  afx_msg void OnGenerateStaticMA();
  afx_msg void OnShowSpheres();
  afx_msg void OnUpdateShowSpheres(CCmdUI *pCmdUI);
  afx_msg void OnGenerateAdaptiveMA();
  afx_msg void OnGenerateImproveMA();
  afx_msg void OnUpdateGenerateImproveMA(CCmdUI *pCmdUI);
  afx_msg void OnUseConvexTester();
  afx_msg void OnUpdateUseConvexTester(CCmdUI *pCmdUI);
  afx_msg void OnUpdateReduces(CCmdUI *pCmdUI);
  afx_msg void OnReduceHubbard();
  afx_msg void OnReduceMerge();
  afx_msg void OnReduceBurst();
  afx_msg void OnReduceExpand();
  afx_msg void OnUpdateOptimises(CCmdUI *pCmdUI);
  afx_msg void OnOptimisePerSphere();
  afx_msg void OnOptimiseSimplex();
  afx_msg void OnOptimiseBalance();
  afx_msg void OnSpheresLoad();
  afx_msg void OnSpheresSave();
  afx_msg void OnUpdateSpheresSave(CCmdUI *pCmdUI);
  afx_msg void OnSpheresExportPOV();
  afx_msg void OnUpdateSpheresExportPOV(CCmdUI *pCmdUI);
  afx_msg void OnModelSave();
  afx_msg void OnUpdateModelSave(CCmdUI *pCmdUI);
  afx_msg void OnValidateMesh();
  afx_msg void OnUpdateValidateMesh(CCmdUI *pCmdUI);
  afx_msg void OnEvaluateSpheres();
  afx_msg void OnUpdateEvaluateSpheres(CCmdUI *pCmdUI);
  afx_msg void OnSphereTreeNone();
  afx_msg void OnUpdateSphereTreeNone(CCmdUI *pCmdUI);
  afx_msg void OnSphereTreeNext();
  afx_msg void OnUpdateSphereTreeNext(CCmdUI *pCmdUI);
  afx_msg void OnSphereTreePrev();
  afx_msg void OnUpdateSphereTreePrev(CCmdUI *pCmdUI);
  afx_msg void OnSphereTreeTop();
  afx_msg void OnUpdateSphereTreeTop(CCmdUI *pCmdUI);
  afx_msg void OnGentreeOctree();
  afx_msg void OnGentreeHubbard();
  afx_msg void OnGentreeMerge();
  afx_msg void OnGentreeBurst();
  afx_msg void OnGentreeExpand();
  afx_msg void OnGentreeCombined();
  afx_msg void OnGentreeSpawn();
  afx_msg void OnGentreeGrid();
  afx_msg void OnModelSaveObj();
  afx_msg void OnUpdateModelSaveObj(CCmdUI *pCmdUI);
  afx_msg void OnSpheresExportVRML();
  afx_msg void OnUpdateSpheresExportVRML(CCmdUI *pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  //  get the model
  CModelDoc *GetDocument();

  //  CObjectView calls drawObject to do OpenGL
  void drawObject(bool sel);

private:
  //  flags to say what we are displaying
  bool showModel, showSamples, showSpheres;

  //  set of surface samples
  Array<Surface::Point> samples;
  void sampleObject(int num, bool useVert);
  bool haveSamples() const;

  //  Voronoi to represent the medial axis approximation
  Voronoi3D vor;
  bool haveVoronoi() const;

  //  medial tester (needed to check if vertices are inside the object)
  MedialTester mt;
  void makeMT();

  //  medial axis spheres
  Array<Sphere> medialSpheres;
  void makeSpheresFromVoronoi();
  bool haveSpheres() const;

  // if the spheres are directly from the tree we can show children
  bool medialSpheresFromTree;

  //  sphere-tree for the model
  SphereTree sphereTree;
  bool haveSphereTree() const;
  int sphereTreeLevel;
  void fillMedialSpheresFromSphereTree();
  void setupSpheresFromSphereTree();

  //  choice of whether to use convex or non-convex evaluator
  bool useConvexTester;
  SEConvex convexEval;
  Array<Point3D> testerPts;
  SESphPt nonConvexEval;
  SEBase *getErrorTester();

  //  apply a reducer to the current medial axis approximation
  void applyReducer(SRBase *reducer);

  //  apply an optimiser to the set of spheres
  void applyOptimiser(SOBase *optimiser);

  //  make a sphere-tree using the generic sphere- algorithm
  bool makeSphereTree(SRBase *reducer, bool initVor);
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_MODELVIEW_H__53B7BE81_376E_11D7_BEA8_00104B9D2B6D__INCLUDED_)
