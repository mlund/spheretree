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

#include "SphereTree.h"
#include "../API/SFRitter.h"
#include "../Base/Defs.h"
#include <stdio.h>
#include <string.h>

bool SphereTree::saveSphereTree(const char *fileName, float scale){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  fprintf(f, "%d %d\n", levels, degree);
  int numnodes = nodes.getSize();
  for (int i = 0; i < numnodes; i++){
    STSphere *s = &nodes.index(i);        //  NOW SAVING OCCUPANCY - need to do loading too sometime
    fprintf(f, "%f %f %f %f %f", s->c.x*scale, s->c.y*scale, s->c.z*scale, s->r*scale, s->occupancy);
    if (s->hasAux)
      fprintf(f, " %f %f %f %f %f", s->sAux.c.x*scale, s->sAux.c.y*scale, s->sAux.c.z*scale, s->sAux.r*scale, s->errDec);
    fprintf(f, "\n");
    };

  fclose(f);
  return true;
}

bool SphereTree::loadSphereTree(const char *fileName, float scale){
  FILE *f = fopen(fileName, "r");
  if (!f)
    return false;

  if (fscanf(f, "%d %d", &levels, &degree) != 2 ||
      levels < 1){
    fclose(f);
    return false;
    }

  char buffer[1024];

  int numnodes = 1;
  nodes.setSize(0);
  for (int level = 0; level < levels; level++){
    int baseI = nodes.getSize();
    nodes.resize(baseI + numnodes);

    for (int i = 0; i < numnodes; i++){
      //  read line, chew up end of lines
      buffer[0] = '\0';
      while (fgets(buffer, 1023, f) && strlen(buffer) <= 1);

      //  read the sphere
      float x, y, z, r, x1, y1, z1, r1, errDec;
      if (sscanf(buffer, "%f %f %f %f %f %f %f %f %f", &x, &y, &z, &r, &x1, &y1, &z1, &r1, &errDec) == 9){
        STSphere *s = &nodes.index(baseI + i);
        s->c.x = x*scale;
        s->c.y = y*scale;
        s->c.z = z*scale;
        s->r = r*scale;
        s->occupancy = -1;

        s->hasAux = true;
        s->sAux.c.x = x1*scale;
        s->sAux.c.y = y1*scale;
        s->sAux.c.z = z1*scale;
        s->sAux.r = r1*scale;
        s->errDec = errDec;
        }
      else if (sscanf(buffer, "%f %f %f %f", &x, &y, &z, &r) == 4){
        STSphere *s = &nodes.index(baseI + i);
        s->c.x = x*scale;
        s->c.y = y*scale;
        s->c.z = z*scale;
        s->r = r*scale;
        s->hasAux = false;
        }
      else{
        nodes.setSize(baseI);
        levels = level;
        fclose(f);
        return false;
        }
      }

    numnodes *= degree;
    }

  fclose(f);
  return true;
}

void SphereTree::initNode(int node, int level){
  if (level < 0){
    int lev;
    int start = 0, num = 1;
    for (lev = 0; lev <= levels; lev++){
      if (node >= start && node < start+num)
        break;

      start += num;
      num *= degree;
      }
    level = lev+1;
    }

  //  init node
  STSphere *s = &nodes.index(node);
  s->c.x = s->c.y = s->c.z = 0.0f;
  s->r = -1.0;
  s->errDec = -1;
  s->hasAux = false;
  s->occupancy = 1.0;

  // do we have children
  if (level < levels){
    //  make sure we have room for children
    int firstChild = node*degree + 1;
    for (int i = 0; i < degree; i++)
      initNode(firstChild+i, level+1);
    }
}

bool SphereTree::saveSpheres(const Array<Sphere> &nodes, const char *fileName, float scale){
  int numSph = nodes.getSize();

  //  compute simple bounding sphere
  Sphere boundSphere;
  boundSphere.c = Point3D::ZERO;
  int numValid = 0;
  for (int i = 0; i < numSph; i++){
    Sphere s = nodes.index(i);
    if (s.r > 0){
      boundSphere.c.x += s.c.x;
      boundSphere.c.y += s.c.y;
      boundSphere.c.z += s.c.z;
      numValid++;
      }
    }
  boundSphere.c.x /= numValid;
  boundSphere.c.y /= numValid;
  boundSphere.c.z /= numValid;

  //  radius
  boundSphere.r = 0;
  for (int i = 0; i < numSph; i++){
    Sphere s = nodes.index(i);
    if (s.r > 0){
      float r = boundSphere.c.distance(s.c) + s.r;
      if (r > boundSphere.r)
        boundSphere.r = s.r;
      }
    }  
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  // header
  fprintf(f, "%d %d\n", 2, numValid);


  //  bounding sphere of model
  fprintf(f, "%f %f %f %f\n", boundSphere.c.x*scale, boundSphere.c.y*scale, boundSphere.c.z*scale, boundSphere.r*scale);

  //  one and only sub-level
  for (int i = 0; i < numSph; i++){
    Sphere s = nodes.index(i);
    if (s.r > 0)
      fprintf(f, "%f %f %f %f\n", s.c.x*scale, s.c.y*scale, s.c.z*scale, s.r*scale);
    };

  //  done
  fclose(f);
  return true;
}

void SphereTree::getLevel(Array<Sphere> *nodes, int level) const{
  unsigned long startI, numS;
  getRow(&startI, &numS, level);

  nodes->setSize(0);
  for (int i = 0; i < numS; i++){
    Sphere s = this->nodes.index(startI+i);
    if (s.r > 0)
      nodes->addItem() = s;
    }
}

void SphereTree::setupTree(int deg, int levs){
  this->degree = deg;
  this->levels = levs;

  int total = 1 + deg;
  int nc = deg;
  for (int level = 2; level < levs; level++){
    nc *= deg;
    total += nc;
    }

  this->nodes.resize(total);
  initNode(0);
}

void SphereTree::growTree(int levs){
  //  compute the size
  int total = 1 + degree;
  int nc = degree;
  for (int level = 2; level < levs; level++){
    nc *= degree;
    total += nc;
    }

  //  get the old size
  int oldSize = this->nodes.getSize();

  //  grow the node list
  this->nodes.resize(total);
  this->levels = levs;

  //  initialise the new nodes
  for (int i = oldSize; i < total; i++){
    STSphere *s = &nodes.index(i);
    s->c.x = 0;
    s->c.y = 0;
    s->c.z = 0;
    s->r = -1;
    s->sAux.c = s->c;
    s->sAux.r = s->r;
    s->errDec = -1;
    s->hasAux = false;
    s->occupancy = 1.0;
    }
}

