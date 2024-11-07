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

//  MFC includes
#include "stdafx.h"
#include "ModelView.h"
#include "resource.h"
#include "InputDialog.h"
#include "ProgressMeter.h"
#include "TraceOutput.h"
#include "OutputDialog.h"

//  ST includes
#include "../../src/API/MSGrid.h"
#include "../../src/API/VFAdaptive.h"
#include "../../src/API/SSIsohedron.h"
#include "../../src/API/SFWhite.h"
#include "../../src/API/MergeHubbard.h"
#include "../../src/API/SRMerge.h"
#include "../../src/API/SRBurst.h"
#include "../../src/API/SRExpand.h"
#include "../../src/API/SRComposite.h"
#include "../../src/API/SRSpawn.h"
#include "../../src/API/SRGrid.h"
#include "../../src/API/REMaxElim.h"
#include "../../src/API/SOPerSphere.h"
#include "../../src/API/SOSimplex.h"
#include "../../src/API/SOBalance.h"
#include "../../src/API/STGOctree.h"
#include "../../src/API/STGHubbard.h"
#include "../../src/API/STGGeneric.h"
#include "../../src/Export/POV.h"
#include "../../src/Export/VRML.h"
#include "../../src/Surface/objLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEPTH_SHIFT 0.001 //  SHIFT depth buffer for decal
#define SPHERE_DIVS 10    //  Slices and Stacks for spheres

IMPLEMENT_DYNCREATE(CModelView, CObjectView)

CModelView::CModelView() {
  showModel = true;
  showSamples = false;
  showSpheres = false;
  useConvexTester = true;
  sphereTreeLevel = -1;
  medialSpheresFromTree = false;
}

CModelView::~CModelView() {}

BEGIN_MESSAGE_MAP(CModelView, CObjectView)
//{{AFX_MSG_MAP(CModelView)
ON_COMMAND(ID_GENERATE_SAMPLES, OnGenerateSamples)
ON_UPDATE_COMMAND_UI(ID_GENERATE_SAMPLES, OnUpdateGenerateSamples)
ON_COMMAND(ID_SHOW_MODEL, OnShowModel)
ON_UPDATE_COMMAND_UI(ID_SHOW_MODEL, OnUpdateShowModel)
ON_COMMAND(ID_SHOW_SAMPLES, OnShowSamples)
ON_UPDATE_COMMAND_UI(ID_SHOW_SAMPLES, OnUpdateShowSamples)
ON_COMMAND(ID_GENERATE_STATIC_MA, OnGenerateStaticMA)
ON_COMMAND(ID_SHOW_SPHERES, OnShowSpheres)
ON_UPDATE_COMMAND_UI(ID_SHOW_SPHERES, OnUpdateShowSpheres)
ON_COMMAND(ID_GENERATE_ADAPTIVE_MA, OnGenerateAdaptiveMA)
ON_COMMAND(ID_GENERATE_IMPROVE_MA, OnGenerateImproveMA)
ON_UPDATE_COMMAND_UI(ID_GENERATE_IMPROVE_MA, OnUpdateGenerateImproveMA)
ON_COMMAND(ID_USE_CONVEX_TESTER, OnUseConvexTester)
ON_UPDATE_COMMAND_UI(ID_USE_CONVEX_TESTER, OnUpdateUseConvexTester)
ON_UPDATE_COMMAND_UI(ID_REDUCE_HUBBARD, OnUpdateReduces)
ON_COMMAND(ID_REDUCE_HUBBARD, OnReduceHubbard)
ON_COMMAND(ID_REDUCE_MERGE, OnReduceMerge)
ON_COMMAND(ID_REDUCE_BURST, OnReduceBurst)
ON_COMMAND(ID_REDUCE_EXPAND, OnReduceExpand)
ON_UPDATE_COMMAND_UI(ID_OPTIMISE_PERSPHERE, OnUpdateOptimises)
ON_COMMAND(ID_OPTIMISE_PERSPHERE, OnOptimisePerSphere)
ON_COMMAND(ID_OPTIMISE_SIMPLEX, OnOptimiseSimplex)
ON_COMMAND(ID_OPTIMISE_BALANCE, OnOptimiseBalance)
ON_COMMAND(ID_SPHERES_LOAD, OnSpheresLoad)
ON_COMMAND(ID_SPHERES_SAVE, OnSpheresSave)
ON_UPDATE_COMMAND_UI(ID_SPHERES_SAVE, OnUpdateSpheresSave)
ON_COMMAND(ID_SPHERES_EXPORT_POV, OnSpheresExportPOV)
ON_UPDATE_COMMAND_UI(ID_SPHERES_EXPORT_POV, OnUpdateSpheresExportPOV)
ON_COMMAND(ID_SPHERES_EXPORT_VRML, OnSpheresExportVRML)
ON_UPDATE_COMMAND_UI(ID_SPHERES_EXPORT_VRML, OnUpdateSpheresExportVRML)
ON_COMMAND(ID_MODEL_SAVE, OnModelSave)
ON_UPDATE_COMMAND_UI(ID_MODEL_SAVE, OnUpdateModelSave)
ON_COMMAND(ID_VALIDATE_MESH, OnValidateMesh)
ON_UPDATE_COMMAND_UI(ID_VALIDATE_MESH, OnUpdateValidateMesh)
ON_COMMAND(ID_EVALUATE_SPHERES, OnEvaluateSpheres)
ON_UPDATE_COMMAND_UI(ID_EVALUATE_SPHERES, OnUpdateEvaluateSpheres)
ON_COMMAND(ID_SPHERETREE_NONE, OnSphereTreeNone)
ON_UPDATE_COMMAND_UI(ID_SPHERETREE_NONE, OnUpdateSphereTreeNone)
ON_COMMAND(ID_SPHERETREE_NEXT, OnSphereTreeNext)
ON_UPDATE_COMMAND_UI(ID_SPHERETREE_NEXT, OnUpdateSphereTreeNext)
ON_COMMAND(ID_SPHERETREE_PREV, OnSphereTreePrev)
ON_UPDATE_COMMAND_UI(ID_SPHERETREE_PREV, OnUpdateSphereTreePrev)
ON_COMMAND(ID_SPHERETREE_TOP, OnSphereTreeTop)
ON_UPDATE_COMMAND_UI(ID_SPHERETREE_TOP, OnUpdateSphereTreeTop)
ON_COMMAND(ID_GENTREE_OCTREE, OnGentreeOctree)
ON_COMMAND(ID_GENTREE_HUBBARD, OnGentreeHubbard)
ON_COMMAND(ID_GENTREE_MERGE, OnGentreeMerge)
ON_COMMAND(ID_GENTREE_BURST, OnGentreeBurst)
ON_COMMAND(ID_GENTREE_EXPAND, OnGentreeExpand)
ON_COMMAND(ID_GENTREE_COMBINED, OnGentreeCombined)
ON_COMMAND(ID_GENTREE_SPAWN, OnGentreeSpawn)
ON_COMMAND(ID_GENTREE_GRID, OnGentreeGrid)
ON_COMMAND(ID_MODEL_SAVEOBJ, OnModelSaveObj)
ON_UPDATE_COMMAND_UI(ID_MODEL_SAVEOBJ, OnUpdateModelSaveObj)
ON_UPDATE_COMMAND_UI(ID_REDUCE_MERGE, OnUpdateReduces)
ON_UPDATE_COMMAND_UI(ID_REDUCE_BURST, OnUpdateReduces)
ON_UPDATE_COMMAND_UI(ID_REDUCE_EXPAND, OnUpdateReduces)
ON_UPDATE_COMMAND_UI(ID_OPTIMISE_SIMPLEX, OnUpdateOptimises)
ON_UPDATE_COMMAND_UI(ID_OPTIMISE_BALANCE, OnUpdateOptimises)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
    Get the model document
*/
CModelDoc *CModelView::GetDocument() { return (CModelDoc *)m_pDocument; }

/*
    initialise the CObjectView
*/
void CModelView::OnInitialUpdate() {
  CObjectView::OnInitialUpdate();
  CModelDoc *pDoc = GetDocument();

  //  calculate scale to fit into a 2*2*2 box
  float sX = 2.0 / (pDoc->surface.pMax.x - pDoc->surface.pMin.x);
  float sY = 2.0 / (pDoc->surface.pMax.y - pDoc->surface.pMin.y);
  float sZ = 2.0 / (pDoc->surface.pMax.z - pDoc->surface.pMin.z);

  fScale = sX;
  if (sY < fScale)
    fScale = sY;
  if (sZ < fScale)
    fScale = sZ;

  fScale *= 0.975f;

  fTr[0] = -(pDoc->surface.pMax.x + pDoc->surface.pMin.x) / 2;
  fTr[1] = -(pDoc->surface.pMax.y + pDoc->surface.pMin.y) / 2;
  fTr[2] = -(pDoc->surface.pMax.z + pDoc->surface.pMin.z) / 2;

  TRACE("set scale to %f\n", fScale);
  TRACE("set fTr to [%f, %f, %f]\n", fTr[0], fTr[1], fTr[2]);
}

/*
    Draw the object when CObjectView requests it

    selection mechanism used to select a sphere which is
    made transparant
*/
void CModelView::drawObject(bool sel) {
  //  setup draw model
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glLineWidth(1);

  //  get model info
  CModelDoc *pDoc = GetDocument();
  Array<Surface::Point> *pts = &pDoc->surface.vertices;
  Array<Surface::Triangle> *tris = &pDoc->surface.triangles;

  //  draw model
  if (showModel && !sel) {
    //  setup for decal
    glDepthRange(DEPTH_SHIFT, 1.0);

    //  draw triangles
    glColor4f(1, 1, 1, 1);
    glBegin(GL_TRIANGLES);
    int numTris = tris->getSize();
    for (int i = 0; i < numTris; i++) {
      //  get triangle
      Surface::Triangle *tri = &tris->index(i);

      //  get face normal
      Vector3D v;
      pDoc->surface.getTriangleNormal(&v, i);
      glNormal3f(v.x, v.y, v.z);

      //  draw vertices
      for (int j = 0; j < 3; j++) {
        const Surface::Point *p = &pts->index(tri->v[j]);
        glVertex3f(p->p.x, p->p.y, p->p.z);
      }
    }
    glEnd();

    //  draw edges
    if (!sel) {
      //  setup for decal
      glDepthRange(0.0, 1.0 - DEPTH_SHIFT);

      //  set line mode
      glPolygonMode(GL_FRONT, GL_LINE);

      //  no lighting
      glDisable(GL_LIGHTING);

      //  draw edges
      glColor3f(0, 0, 1);
      glBegin(GL_TRIANGLES);
      for (i = 0; i < numTris; i++) {
        Surface::Triangle *tri = &tris->index(i);

        for (int j = 0; j < 3; j++) {
          Surface::Point *p = &pts->index(tri->v[j]);
          glNormal3f(p->n.x, p->n.y, p->n.z);
          glVertex3f(p->p.x, p->p.y, p->p.z);
        }
      }
      glEnd();
    }
  }

  //  draw samples
  if (showSamples && !sel) {
    glDepthRange(0.0, 1.0 - DEPTH_SHIFT);
    glDisable(GL_LIGHTING);
    glPointSize(4);

    int n = samples.getSize();

    glColor3f(0, 0, 1);
    glBegin(GL_POINTS);
    for (int i = 0; i < n; i++) {
      Point3D p = samples.index(i).p;
      glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
  }

  //  draw the spheres (from medial set)
  if (showSpheres && !medialSpheresFromTree) {
    glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_CULL_FACE);
    GLUquadricObj *q = gluNewQuadric();
    glDepthRange(0, 1.0);
    glColor3f(1, 0, 0);

    int numSph = medialSpheres.getSize();
    for (int i = 0; i < numSph; i++) {
      if (i == selID)
        continue;

      //  get sphere
      Sphere *s = &medialSpheres.index(i);

      //  setup label for selection
      if (sel)
        glLoadName(i);

      //  draw sphere
      glPushMatrix();
      glTranslatef(s->c.x, s->c.y, s->c.z);
      gluSphere(q, s->r, SPHERE_DIVS, SPHERE_DIVS);
      glPopMatrix();
    }

    //  draw selected sphere
    if (selID >= 0 && selID < medialSpheres.getSize()) {
      Sphere *s = &medialSpheres.index(selID);

      //  setup alpha blend
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(1, 0, 0, 0.5);

      //  draw sphere
      glPushMatrix();
      glTranslatef(s->c.x, s->c.y, s->c.z);
      gluSphere(q, s->r, SPHERE_DIVS, SPHERE_DIVS);
      glPopMatrix();

      //  turn off blend
      glDisable(GL_BLEND);
    }

    gluDeleteQuadric(q);
  }

  //  draw the spheres from the sphere-tree so we can show their children
  if (showSpheres && sphereTreeLevel >= 0 && medialSpheresFromTree) {
    //  setup GL
    glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_CULL_FACE);
    GLUquadricObj *q = gluNewQuadric();
    glDepthRange(0, 1.0);
    glColor3f(1, 0, 0);

    //  get sphere-tree info
    unsigned long start, size;
    sphereTree.getRow(&start, &size, sphereTreeLevel);

    //  draw selected sphere into stencil buffer
    if (!sel && selID >= 0) {
      //  draw selected sphere into stencil buffer
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glColorMask(0, 0, 0, 0);
      glPolygonMode(GL_FRONT, GL_FILL);
      glDisable(GL_DEPTH_TEST);

      Sphere s = sphereTree.nodes.index(selID);
      glPushMatrix();
      glTranslatef(s.c.x, s.c.y, s.c.z);
      gluSphere(q, s.r, SPHERE_DIVS, SPHERE_DIVS);
      glPopMatrix();

      glColorMask(1, 1, 1, 1);
      glStencilFunc(GL_NOTEQUAL, 1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      glEnable(GL_DEPTH_TEST);
    }

    //  draw the full set of spheres
    for (int i = 0; i < size; i++) {
      int sphNum = i + start;

      //  get sphere
      Sphere *s = &sphereTree.nodes.index(sphNum);
      if (s->r <= 0)
        continue;

      //  setup label for selection
      if (sel)
        glLoadName(sphNum);

      //  draw sphere
      glPushMatrix();
      glTranslatef(s->c.x, s->c.y, s->c.z);
      gluSphere(q, s->r, SPHERE_DIVS, SPHERE_DIVS);
      glPopMatrix();
    }

    //  draw selected sphere and its children
    if (!sel && selID >= 0) {
      //  turn off stencil
      glDisable(GL_STENCIL_TEST);

      //  enable blend
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //  draw children
      int firstChild = sphereTree.getFirstChild(selID);
      if (firstChild < sphereTree.nodes.getSize()) {
        glColor4f(1, 0, 1, 1);

        int num = sphereTree.degree;
        for (int i = 0; i < num; i++) {
          Sphere s = sphereTree.nodes.index(firstChild + i);
          if (s.r > 0) {
            glPushMatrix();
            glTranslatef(s.c.x, s.c.y, s.c.z);
            gluSphere(q, s.r, 10, 10);
            glPopMatrix();
          }
        }
      }

      //  draw the parent sphere
      glColor4f(1, 0, 0, 0.2f);
      Sphere s = sphereTree.nodes.index(selID);
      glPushMatrix();
      glTranslatef(s.c.x, s.c.y, s.c.z);
      gluSphere(q, s.r, 10, 10);
      glPopMatrix();
    }
  }
}

/*
  //  draw lines to closest features ---  goes in where we draw children above
  if (showSpheres && selID >= 0 && !sel){
    //  get selected sphere
    Sphere s;

    if (sphereTreeLevel >= 0 && medialSpheresFromTree)
      s = sphereTree.nodes.index(selID);
    else
      s = medialSpheres.index(selID);

    //  generate points
    Array<Point3D> sphPts;
    SSIsohedron::generateSamples(&sphPts, 2);

    //  make sure we have medial tester
    makeMT();

    //  enable blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //  draw points
    glPointSize(4);
    glDisable(GL_LIGHTING);
    for (int i = 0; i < sphPts.getSize(); i++){
      //  get point on sphere
      Point3D p = sphPts.index(i);
      p.x = p.x*s.r + s.c.x;
      p.y = p.y*s.r + s.c.y;
      p.z = p.z*s.r + s.c.z;

      //  find closest point on surface
      ClosestPointInfo inf;
      getClosestPoint(&inf, p, pDoc->surface, mt.getFaceHash());

      //  draw points
      glBegin(GL_POINTS);
      glColor4f(1, 0, 0, 1);
      glVertex3f(p.x, p.y, p.z);
      glColor4f(0, 1, 0, 1);
      glVertex3f(inf.pClose.x, inf.pClose.y, inf.pClose.z);
      glEnd();

      //  draw line
      glBegin(GL_LINES);
      glColor4f(1, 0, 0, 1);
      glVertex3f(p.x, p.y, p.z);
      glColor4f(0, 1, 0, 1);
      glVertex3f(inf.pClose.x, inf.pClose.y, inf.pClose.z);
      glEnd();
      }
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    }
*/

/*
    Do we display the model
*/
void CModelView::OnShowModel() {
  showModel = !showModel;
  clearDisplayList();
  InvalidateRect(NULL);
}

void CModelView::OnUpdateShowModel(CCmdUI *pCmdUI) {
  CModelDoc *pDoc = GetDocument();
  Array<Surface::Triangle> *tris = &pDoc->surface.triangles;
  pCmdUI->Enable(tris->getSize() > 0);
  pCmdUI->SetCheck(showModel);
}

/*
    Sample the surface of the object
*/
void CModelView::sampleObject(int num, bool useVert) {
  try {
    CModelDoc *pDoc = GetDocument();
    MSGrid::generateSamples(&samples, num, pDoc->surface, useVert);
    TRACE("%d Samples Generated\n", samples.getSize());
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

bool CModelView::haveSamples() const { return samples.getSize() > 0; }

void CModelView::OnGenerateSamples() {
  try {
    CInputDialog id;
    id.m_message = "Number of Samples : ";
    id.m_input = 500;
    if (id.DoModal() == IDOK) {
      int numSamples = id.m_input;
      samples.resize(0);

      //  generate the samples
      sampleObject(numSamples, true);

      // redraw
      showSamples = true;
      clearDisplayList();
      InvalidateRect(NULL);
    }
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

void CModelView::OnUpdateGenerateSamples(CCmdUI *pCmdUI) {
  CModelDoc *pDoc = GetDocument();
  Array<Surface::Triangle> *tris = &pDoc->surface.triangles;

  pCmdUI->Enable(tris->getSize() > 0);
}

void CModelView::OnShowSamples() {
  showSamples = !showSamples;
  clearDisplayList();
  InvalidateRect(NULL);
}

void CModelView::OnUpdateShowSamples(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSamples());
  pCmdUI->SetCheck(showSamples);
}

/*
    Medial Axis Approximation
*/
void CModelView::OnShowSpheres() {
  showSpheres = !showSpheres;
  clearDisplayList();
  InvalidateRect(NULL);
}

bool CModelView::haveSpheres() const { return medialSpheres.getSize() > 0; }

void CModelView::OnUpdateShowSpheres(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
  pCmdUI->SetCheck(showSpheres);
}

void CModelView::OnGenerateStaticMA() {
  try {
    //  make sure we have some samples
    if (!haveSamples()) {
      OnGenerateSamples();
      showSamples = false;
    }

    //  initialise the Voronoi diagram
    CModelDoc *pDoc = GetDocument();
    Point3D pMin = pDoc->surface.pMin;
    Point3D pMax = pDoc->surface.pMax;

    Point3D pC;
    pC.x = (pMax.x + pMin.x) / 2.0f;
    pC.y = (pMax.y + pMin.y) / 2.0f;
    pC.z = (pMax.z + pMin.z) / 2.0f;
    vor.initialise(pC, 1.0f * pDoc->surface.pMin.distance(pC));

    //  add the samples
    CProgressMeter pm;
    pm.init(0);
    pm.setMessage("Generating Voronoi");
    vor.randomInserts(samples, -1, &pm);

    //  make the spheres
    makeSpheresFromVoronoi();

    //  turn on spheres
    showSpheres = false;
    OnShowSpheres();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

void CModelView::makeSpheresFromVoronoi() {
  CProgressMeter pm;
  pm.init(0);
  pm.setMessage("Making Medial Spheres");

  //  make sure we have the medial tester
  makeMT();

  //  produce the medial spheres
  medialSpheres.setSize(0);
  int numVerts = vor.vertices.getSize();
  for (int i = 0; i < numVerts; i++) {
    //  update the progress bar
    if (!pm.setProgress(100.0f * i / (float)numVerts))
      break;

    //  get the vertex
    Voronoi3D::Vertex *v = &vor.vertices.index(i);

    if (mt.isMedial(v)) {
      //  make sphere
      Sphere *s = &medialSpheres.addItem();
      *s = v->s;
    }
  }

  //  clear flag
  medialSpheresFromTree = false;
  selID = -1;
}

void CModelView::makeMT() {
  if (!mt.hasSurface()) {
    mt.setSurface(GetDocument()->surface, 20, 3);
    mt.useLargeCover = false;
  }
}

/*
    Adaptive Medial Axis
*/
void CModelView::OnGenerateAdaptiveMA() {
  try {
    //  reset the medial axis
    CModelDoc *pDoc = GetDocument();
    Point3D pMin = pDoc->surface.pMin;
    Point3D pMax = pDoc->surface.pMax;

    Point3D pC;
    pC.x = (pMax.x + pMin.x) / 2.0f;
    pC.y = (pMax.y + pMin.y) / 2.0f;
    pC.z = (pMax.z + pMin.z) / 2.0f;
    vor.initialise(pC, 1.0f * pDoc->surface.pMin.distance(pC));

    //  dump medial spheres
    medialSpheres.setSize(0);

    //  call the adaptive algorithm
    OnGenerateImproveMA();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

void CModelView::OnGenerateImproveMA() {
  try {
    //  generate parameters
    CInputDialog id;
    id.m_message = "Voronoi Tolerance : ";
    id.m_input = -1;
    if (id.DoModal() != IDOK)
      return;
    float tol = id.m_input;

    int maxSpheres = -1;
    if (tol < 0) {
      id.m_message = "Max Spheres : ";
      id.m_input = 500;
      if (id.DoModal() != IDOK)
        return;
      maxSpheres = id.m_input;
      tol = 0;
    }

    //  perform adaptive sampling
    if (tol > 0 || maxSpheres > 0) {
      //  generate set of points to represent the surface
      id.m_message = "Coverage Samples : ";
      id.m_input = 1000;
      if (id.DoModal() != IDOK)
        return;
      int numSam = id.m_input;
      sampleObject(numSam, true);

      SurfaceRep surRep;
      surRep.setup(samples);

      //  do adaptive step
      VFAdaptive adaptive;
      adaptive.eval = getErrorTester();
      adaptive.mt = &mt;
      adaptive.adaptiveSample(&vor, tol, -1, maxSpheres, 0, &surRep);
    }

    //  make spheres
    makeSpheresFromVoronoi();
    showSpheres = false;
    OnShowSpheres();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

bool CModelView::haveVoronoi() const { return vor.vertices.getSize() > 5; }

void CModelView::OnUpdateGenerateImproveMA(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveVoronoi());
}

/*
    choice of non-convex or convex evaluators
*/
void CModelView::OnUseConvexTester() { useConvexTester = !useConvexTester; }

void CModelView::OnUpdateUseConvexTester(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(useConvexTester);
}

SEBase *CModelView::getErrorTester() {
  //  make sure medial tester is setup
  makeMT();

  //  choose tester
  if (useConvexTester) {
    convexEval.setTester(mt);
    return &convexEval;
  } else {
    //  generate 42 sphere points
    if (testerPts.getSize() == 0) {
      SSIsohedron::generateSamples(&testerPts, 1);
      TRACE("using %d points for tester\n", testerPts.getSize());
    }

    nonConvexEval.setup(mt, testerPts);
    return &nonConvexEval;
  }
}

/*
    enable ALL reducers
*/
void CModelView::OnUpdateReduces(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveVoronoi());
}

/*
    reduce using Hubbard's algorithm
*/
void CModelView::OnReduceHubbard() {
  try {
    CInputDialog id;
    id.m_message = "Number of Spheres : ";
    id.m_input = 8;
    if (id.DoModal() != IDOK)
      return;
    int numSpheres = id.m_input;

    //  reduction
    MergeHubbard merger;
    merger.setup(&vor, &mt);
    merger.getSpheres(&medialSpheres, numSpheres);

    //  clear flag
    medialSpheresFromTree = false;
    selID = -1;

    //  make sure we are showing spheres
    showSpheres = false;
    OnShowSpheres();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    apply a reducer to the current medial axis approximation
*/
void CModelView::applyReducer(SRBase *reducer) {
  //  get parameters
  CInputDialog id;
  id.m_message = "Surface Samples : ";
  id.m_input = 1000;
  if (id.DoModal() != IDOK || id.m_input <= 0)
    return;
  int numSam = id.m_input;

  id.m_message = "Number of Spheres : ";
  id.m_input = 8;
  if (id.DoModal() != IDOK)
    return;
  int numSpheres = id.m_input;

  //  generate the surface representation
  sampleObject(numSam, true);
  SurfaceRep surRep;
  surRep.setup(samples);

  //  make sure the medial tester is initialised
  makeMT();

  //  do the reduction
  reducer->getSpheres(&medialSpheres, numSpheres, surRep);

  //  clear flag
  medialSpheresFromTree = false;
  selID = -1;

  //  make sure we are showing spheres
  showSpheres = false;
  OnShowSpheres();
}

/*
    reduce the set of spheres using Merge algorithm
*/
void CModelView::OnReduceMerge() {
  try {
    SFWhite fitter;
    SRMerge merger;
    merger.sphereFitter = &fitter;
    merger.sphereEval = getErrorTester();
    merger.setup(&vor, &mt);

    applyReducer(&merger);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    reduce the set of spheres using Burst algorithm
*/
void CModelView::OnReduceBurst() {
  try {
    SFWhite fitter;
    SRBurst burster;
    burster.sphereFitter = &fitter;
    burster.sphereEval = getErrorTester();
    burster.setup(&vor, &mt);

    applyReducer(&burster);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    reduce the set of spheres using Expand algorithm
*/
void CModelView::OnReduceExpand() {
  try {
    REMaxElim elim;
    SRExpand expander;
    expander.setup(&vor, &mt);
    expander.errStep = 100;
    expander.redElim = &elim;
    expander.useIterativeSelect = false;

    applyReducer(&expander);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    enable ALL optimisers
*/
void CModelView::OnUpdateOptimises(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
}

/*
    apply any of the optimisers
*/
void CModelView::applyOptimiser(SOBase *optimiser) {
  //  generate the surface representation
  CInputDialog id;
  id.m_message = "Number of Samples : ";
  id.m_input = 1000;
  if (id.DoModal() != IDOK)
    return;
  int numSam = id.m_input;
  sampleObject(numSam, true);
  SurfaceRep surRep;
  surRep.setup(samples);

  //  make sure the medial tester is initialised
  makeMT();

  //  apply optimiser
  optimiser->optimise(&medialSpheres, surRep);

  //  clear flag
  medialSpheresFromTree = false;
  selID = -1;

  //  make sure we are showing the spheres
  showSpheres = false;
  OnShowSpheres();
}

/*
    apply per sphere optimisation
*/
void CModelView::OnOptimisePerSphere() {
  try {
    //  make surface rep
    SOPerSphere perSphere;
    perSphere.eval = getErrorTester();
    perSphere.numIter = 5;

    applyOptimiser(&perSphere);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    apply simplex based optimisation
*/
void CModelView::OnOptimiseSimplex() {
  try {
    SFWhite fitter;
    SOSimplex optimiser;
    optimiser.sphereFitter = &fitter;
    optimiser.sphereEval = getErrorTester();
    applyOptimiser(&optimiser);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    apply balance optimiser to decide if all spheres are needed
*/
void CModelView::OnOptimiseBalance() {
  try {
    //  get parameters
    CInputDialog inputDialog;
    inputDialog.m_message = "% increase allowed :";
    inputDialog.m_input = 5;
    if (inputDialog.DoModal() != IDOK)
      return;
    float incAllowed = inputDialog.m_input / 100.0f;

    //  getup simplex optimiser
    SFWhite fitter;
    SOSimplex optimiser;
    optimiser.sphereFitter = &fitter;
    optimiser.sphereEval = getErrorTester();

    //  setup balance
    SOBalance balancer;
    balancer.V = 0.0f;
    balancer.A = 1;
    balancer.B = incAllowed;
    balancer.optimiser = &optimiser;
    balancer.sphereEval = getErrorTester();

    //  apply algorithm
    applyOptimiser(&balancer);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    displaying the levels of the sphereTree
*/
bool CModelView::haveSphereTree() const {
  return sphereTree.nodes.getSize() > 0;
}

void CModelView::OnSphereTreeNone() {
  sphereTreeLevel = -1;
  fillMedialSpheresFromSphereTree();
}

void CModelView::OnUpdateSphereTreeNone(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSphereTree());
  pCmdUI->SetCheck(sphereTreeLevel == -1);
}

void CModelView::OnSphereTreeTop() {
  sphereTreeLevel = 0;
  fillMedialSpheresFromSphereTree();
}

void CModelView::OnUpdateSphereTreeTop(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSphereTree() && sphereTreeLevel != 0);
}

void CModelView::OnSphereTreeNext() {
  sphereTreeLevel++;
  fillMedialSpheresFromSphereTree();
}

void CModelView::OnUpdateSphereTreeNext(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSphereTree() && sphereTreeLevel < sphereTree.levels - 1);
}

void CModelView::OnSphereTreePrev() {
  sphereTreeLevel--;
  fillMedialSpheresFromSphereTree();
}

void CModelView::OnUpdateSphereTreePrev(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSphereTree() && sphereTreeLevel > 0);
}

void CModelView::setupSpheresFromSphereTree() {
  //  update the set of spheres
  if (sphereTree.levels == 0)
    OnSphereTreeNone();
  else {
    sphereTreeLevel = 1;
    fillMedialSpheresFromSphereTree();
  }
}

void CModelView::fillMedialSpheresFromSphereTree() {
  medialSpheres.setSize(0);

  if (sphereTreeLevel >= 0) {
    // read spheres from sphere-tree
    sphereTree.getLevel(&medialSpheres, sphereTreeLevel);

    //  make sure we show the spheres
    showSpheres = false;
    OnShowSpheres();
  }

  //  set flag
  medialSpheresFromTree = true;
  selID = -1;
}

/*
    Loading and Saving of the spheres
*/
void CModelView::OnSpheresLoad() {
  float scale = GetDocument()->getScaleFactor();
  CFileDialog fd(TRUE, "sph", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "SphereTree (*.sph)|*.sph|", this);
  if (fd.DoModal() == IDOK &&
      sphereTree.loadSphereTree(fd.GetPathName(), scale)) {
    setupSpheresFromSphereTree();
  }
}

void CModelView::OnSpheresSave() {
  CFileDialog fd(FALSE, "sph", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "SphereTree (*.sph)|*.sph|", this);
  if (fd.DoModal() == IDOK) {
    float scale = GetDocument()->getScaleFactor();
    if (this->medialSpheresFromTree)
      sphereTree.saveSphereTree(fd.GetPathName(), 1.0f / scale);
    else
      SphereTree::saveSpheres(medialSpheres, fd.GetPathName(), 1.0f / scale);
  }
}

void CModelView::OnUpdateSpheresSave(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
}

void CModelView::OnSpheresExportPOV() {
  CFileDialog fd(FALSE, "pov", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "POV-RAY (*.pov)|*.pov|", this);
  if (fd.DoModal() == IDOK) {
    //  get the OpenGL matrix (undo the zoom)
    SELECT_CONTEXT();
    glPushMatrix();
    glScalef(1.0 / fScale, 1.0 / fScale, 1.0 / fScale);
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
    UNSELECT_CONTEXT();

    //  convert GL to POV by rearranging the matrix
    float mPOV[12];
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 3; j++)
        mPOV[i * 3 + j] = m[i * 4 + j];

    //  work out scale to get the top sphere to have unit radius
    float bScale = 1.0f / sphereTree.nodes.index(0).r;

    //  work out scale (multiplied by 10 to make bigger in POV)
    exportSpheresPOV(fd.GetPathName(), medialSpheres, bScale, false, mPOV);
  }
}

void CModelView::OnUpdateSpheresExportPOV(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
}

void CModelView::OnSpheresExportVRML() {
  CFileDialog fd(FALSE, "wrl", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "VRML (*.wrl)|*.wrl|", this);
  if (fd.DoModal() == IDOK) {
    //  work out scale to get the top sphere to have unit radius
    float bScale = 1.0f / sphereTree.nodes.index(0).r;

    //  work out scale (multiplied by 10 to make bigger in POV)
    exportSpheresVRML(fd.GetPathName(), medialSpheres, bScale);
  }
}

void CModelView::OnUpdateSpheresExportVRML(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
}

/*
    Save model as SUR format
*/
void CModelView::OnModelSave() {
  CFileDialog fd(FALSE, "sur", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "Model (*.sur)|*.sur|", this);
  if (fd.DoModal() == IDOK) {
    CModelDoc *pDoc = GetDocument();
    pDoc->surface.saveSurface(fd.GetPathName(), 1.0f / pDoc->getScaleFactor());
  }
}

void CModelView::OnUpdateModelSave(CCmdUI *pCmdUI) {
  CModelDoc *pDoc = GetDocument();
  Array<Surface::Triangle> *tris = &pDoc->surface.triangles;
  pCmdUI->Enable(tris->getSize() > 0);
}

void CModelView::OnModelSaveObj() {
  CFileDialog fd(FALSE, "obj", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 "Wavefront OBJ (*.obj)|*.obj|", this);
  if (fd.DoModal() == IDOK) {
    CModelDoc *pDoc = GetDocument();
    saveOBJ(pDoc->surface, fd.GetPathName(), 1.0f / pDoc->getScaleFactor());
  }
}

void CModelView::OnUpdateModelSaveObj(CCmdUI *pCmdUI) {
  CModelDoc *pDoc = GetDocument();
  Array<Surface::Triangle> *tris = &pDoc->surface.triangles;
  pCmdUI->Enable(tris->getSize() > 0);
}

/*
    Validate that the mesh is closed and nicely formed
*/
void CModelView::OnValidateMesh() {
  try {
    //  get model doc
    CModelDoc *doc = GetDocument();

    //  run test
    int badVerts, badNeighs, openEdges, multEdges, badFaces;
    doc->surface.testSurface(&badVerts, &badNeighs, &openEdges, &multEdges,
                             &badFaces);

    //  write out output
    char buffer[2048];
    sprintf(buffer,
            "Bad Vertices\t:\t%d\nBad Neigh\t:\t%d\nOpen Edges\t:\t%d\nMulti "
            "Edges\t:\t%d\nBad Faces\t:\t%d",
            badVerts, badNeighs, openEdges, multEdges, badFaces);
    AfxMessageBox(buffer);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

void CModelView::OnUpdateValidateMesh(CCmdUI *pCmdUI) {
  pCmdUI->Enable(GetDocument()->surface.triangles.getSize() > 0);
}

/*
    Evaluate the worst error of a set of spheres
*/
void CModelView::OnEvaluateSpheres() {
  try {
    //  get tester
    SEBase *eval = getErrorTester();

    //  run evaluation
    int worstI = -1;
    float worstErr = 0;
    int numSph = medialSpheres.getSize();
    for (int i = 0; i < numSph; i++) {
      float err = eval->evalSphere(medialSpheres.index(i));
      if (err > worstErr) {
        worstErr = err;
        worstI = i;
      }
    }

    //  write out info
    char buffer[1024];
    sprintf(buffer, "Worst Sphere\t: %d\nWorst Error\t: %.3f", worstI,
            worstErr);
    AfxMessageBox(buffer);

    //  select worst sphere
    selID = worstI;
    clearDisplayList();
    InvalidateRect(NULL);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

void CModelView::OnUpdateEvaluateSpheres(CCmdUI *pCmdUI) {
  pCmdUI->Enable(haveSpheres());
}

/*
    Generate a sphere-tree with the OCTREE algorithm
*/
void CModelView::OnGentreeOctree() {
  try {
    //  get parameters
    CInputDialog id;
    id.m_message = "Number of Levels : ";
    id.m_input = 3;
    id.DoModal();
    int numLevels = id.m_input;

    //  setup the sphere-tree
    sphereTree.setupTree(8, numLevels + 1);

    //  generate tree
    STGOctree generator;
    generator.setSurface(GetDocument()->surface);
    generator.constructTree(&sphereTree);

    //  setup the sphere set
    setupSpheresFromSphereTree();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    Generate a sphere-tree with the HUBBARD's algorithm
*/
void CModelView::OnGentreeHubbard() {
  try {
    //  get info
    CInputDialog id;
    id.m_message = "Number of Levels : ";
    id.m_input = 3;
    if (id.DoModal() != IDOK)
      return;
    int numLevels = id.m_input;

    id.m_message = "Branching Factor : ";
    id.m_input = 8;
    if (id.DoModal() != IDOK)
      return;
    int numChildren = id.m_input;

    //  make the medial axis approximation
    if (!haveVoronoi())
      OnGenerateStaticMA();

    //  setup STG
    makeMT();
    STGHubbard hubbard;
    hubbard.setup(&vor, &mt);

    //  setup the sphere-tree
    sphereTree.setupTree(numChildren, numLevels + 1);

    //  build
    hubbard.constructTree(&sphereTree);

    //  setup the sphere set
    setupSpheresFromSphereTree();
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    Use the generic sphere-tree construction algorithm to do construction
*/
bool CModelView::makeSphereTree(SRBase *reducer, bool initVor) {
  if (initVor) {
    //  initialise the voronoi diagram
    CModelDoc *pDoc = GetDocument();
    Point3D pMin = pDoc->surface.pMin;
    Point3D pMax = pDoc->surface.pMax;

    Point3D pC;
    pC.x = (pMax.x + pMin.x) / 2.0f;
    pC.y = (pMax.y + pMin.y) / 2.0f;
    pC.z = (pMax.z + pMin.z) / 2.0f;
    vor.initialise(pC, 1.0f * pDoc->surface.pMin.distance(pC));
  }

  //  get parameters
  CInputDialog id;
  id.m_message = "Coverage Samples : ";
  id.m_input = 1000;
  if (id.DoModal() != IDOK)
    return false;
  int numSam = id.m_input;

  id.m_message = "Number of Levels : ";
  id.m_input = 3;
  if (id.DoModal() != IDOK)
    return false;
  int numLevels = id.m_input;

  id.m_message = "Branching Factor : ";
  id.m_input = 8;
  if (id.DoModal() != IDOK)
    return false;
  int numChildren = id.m_input;

  //  make surface representation
  sampleObject(numSam, true);

  //  make the generator
  STGGeneric generator;
  generator.reducer = reducer;
  generator.eval = getErrorTester();
  generator.useRefit = true;
  generator.setSamples(samples);

  // make sure the MedialTester is all setup
  makeMT();

  //  setup the sphere-tree
  sphereTree.setupTree(numChildren, numLevels + 1);

  //  run algorithm
  generator.constructTree(&sphereTree);

  //  setup the sphere set
  setupSpheresFromSphereTree();

  return true;
}

/*
    use Merge Algorithm
*/
void CModelView::OnGentreeMerge() {
  try {
    //  setup merger
    SFWhite fitter;
    SRMerge merger;
    merger.sphereFitter = &fitter;
    merger.sphereEval = getErrorTester();
    merger.setup(&vor, &mt);
    merger.doAllBelow = 20;

    //  get the parameters
    CInputDialog id;
    id.m_message = "Error Decrease Factor : ";
    id.m_input = 2;
    if (id.DoModal() != IDOK)
      return;
    merger.errorDecreaseFactor = id.m_input;

    id.m_message = "Base Set of Spheres : ";
    id.m_input = 250;
    if (id.DoModal() != IDOK)
      return;
    merger.initSpheres = id.m_input;

    //  do construction
    makeSphereTree(&merger, true);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    use Burst Algorithm
*/
void CModelView::OnGentreeBurst() {
  try {
    //  setup the burster
    SFWhite fitter;
    SRBurst burster;
    burster.sphereFitter = &fitter;
    burster.sphereEval = getErrorTester();
    burster.setup(&vor, &mt);

    //  get the parameters
    CInputDialog id;
    id.m_message = "Error Decrease Factor : ";
    id.m_input = 2;
    if (id.DoModal() != IDOK)
      return;
    burster.errorDecreaseFactor = id.m_input;

    id.m_message = "Base Set of Spheres : ";
    id.m_input = 250;
    if (id.DoModal() != IDOK)
      return;
    burster.initSpheres = id.m_input;

    //  do construction
    makeSphereTree(&burster, true);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    use Expand Algorithm
*/
void CModelView::OnGentreeExpand() {
  try {
    //  setup expander
    REMaxElim elim;
    SRExpand expander;
    expander.setup(&vor, &mt);
    expander.redElim = &elim;
    expander.useIterativeSelect = false;
    expander.relTol = 1E-5;

    //  get the parameters
    CInputDialog id;
    id.m_message = "Error Decrease Factor : ";
    id.m_input = 2;
    if (id.DoModal() != IDOK)
      return;
    expander.errorDecreaseFactor = id.m_input;

    id.m_message = "Base Set of Spheres : ";
    id.m_input = 250;
    if (id.DoModal() != IDOK)
      return;
    expander.initSpheres = id.m_input;

    //  do construction
    makeSphereTree(&expander, true);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    use Combination of Merge and Expand
*/
void CModelView::OnGentreeCombined() {
  try {
    SEBase *eval = getErrorTester();

    //  setup adaptive Voronoi algorithm
    Voronoi3D vor;
    VFAdaptive adaptive;
    adaptive.mt = &mt;
    adaptive.eval = eval;

    //  setup merge
    SFWhite fitter;
    SRMerge merger;
    merger.sphereEval = eval;
    merger.sphereFitter = &fitter;
    merger.vorAdapt = &adaptive;
    merger.minSpheresPerNode = 100;
    merger.errorDecreaseFactor = 2;
    merger.maxItersForVoronoi = 150;
    merger.doAllBelow = 20;
    merger.useBeneficial = true;
    merger.setup(&vor, &mt);

    //  setup expand
    REMaxElim elimME;
    SRExpand expander;
    expander.relTol = 1E-3;
    expander.redElim = &elimME;
    expander.vorAdapt = NULL; //  merge will maintain Voronoi for us
    expander.errStep = 100;
    expander.useIterativeSelect = false;
    expander.setup(&vor, &mt);

    //  setup the COMPOSITE algorithm
    SRComposite composite;
    composite.eval = eval;
    composite.addReducer(&merger);
    composite.addReducer(&expander);

    //  get parameters
    CInputDialog id;
    id.m_message = "Base Set of Spheres : ";
    id.m_input = 250;
    if (id.DoModal() != IDOK)
      return;
    merger.initSpheres = id.m_input;

    id.m_message = "Min Spheres Per Node : ";
    id.m_input = 100;
    if (id.DoModal() != IDOK)
      return;
    merger.minSpheresPerNode = id.m_input;

    id.m_message = "Error Decrease Factor : ";
    id.m_input = 2;
    if (id.DoModal() != IDOK)
      return;
    merger.errorDecreaseFactor = id.m_input;

    //  do construction
    makeSphereTree(&expander, true);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    use SPAWN Algorithm
*/
void CModelView::OnGentreeSpawn() {
  try {
    //  setup spawn
    SRSpawn spawn;
    spawn.setup(mt);
    spawn.eval = getErrorTester();
    spawn.useIterativeSelect = false;

    //  do construction
    makeSphereTree(&spawn, false);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}

/*
    use GRID algorithm
*/
void CModelView::OnGentreeGrid() {
  try {
    //  setup grid algorithm
    SRGrid grid;
    grid.optimise = FALSE;
    grid.sphereEval = getErrorTester();
    grid.useQuickTest = true;

    //  do construction
    makeSphereTree(&grid, false);
  } catch (CancelException &e) {
    AfxMessageBox("Operation Aborted");
  }
}
