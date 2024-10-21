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

#include "OBJLoader.h"
#include <string.h>
#include <iostream>
char CAP(char c){
  if (c >= 'a' && c < 'z')
    return c + ('A' - 'a');
  else
    return c;
}

bool loadOBJ(Surface *sur, const char *fileName, float boxSize){
  //  init the model
  sur->vertices.setSize(0);
  sur->triangles.setSize(0);

  //  open the file
  FILE *f = fopen(fileName, "r");
  if (!f)
    return false;

  //  read the file and process each line
  char buffer[1024];
  while (!feof(f) && fgets(buffer, 1023, f)){
    //  get length
    int len = strlen(buffer);

    //  trim off any leading spaces
    int i;
    for (i = 0; i < len; i++){
      if (buffer[i] != ' ' && buffer[i] != '\t')
        break;
      }
    if (CAP(buffer[i]) == 'V' && buffer[i+1] == ' '){
      //  do new VERTEX
      float x, y, z;
      sscanf(buffer+i+1, "%f %f %f", &x, &y, &z);
      
      Surface::Point *p = &sur->vertices.addItem();
      p->p.x = x;
      p->p.y = y;
      p->p.z = z;
      }
    else if (CAP(buffer[i]) == 'F'){
      //  do new FACE
      Surface::Triangle *tri = &sur->triangles.addItem();

      for (int j = 0; j < 3; j++){
        //  read token
        const char *token = strtok(j == 0 ? (buffer+i+1) : NULL, " ");

        //  decode token
        int a = 0, b = 0, c = 0;
        if (token == NULL ||
            (sscanf(token, "%d/%d/%d", &a, &b, &c) != 3  &&
             sscanf(token, "%d//%d", &a, &c) != 2)){
          fclose(f);
          return false;
          }

        tri->v[j] = a-1;
        tri->f[j] = -1;      //  neighbouring face not given in OBJ
        }

      //  setup triangle normal
      Vector3D V1, V2;
      V1.difference(sur->vertices.index(tri->v[1]).p, sur->vertices.index(tri->v[0]).p);
      V2.difference(sur->vertices.index(tri->v[2]).p, sur->vertices.index(tri->v[0]).p);
      tri->n.cross(V1,V2);
      tri->n.norm();
      }
    }

  //  setup bounding box
  sur->setupBoundingBox();

  //  scale the box :)
  if (boxSize > 0)
    sur->fitIntoBox(boxSize);

  //  work out vertex normals
  sur->setupNormals(0, sur->vertices.getSize(), 0, sur->triangles.getSize());

  //  connection to other triangles
  sur->setupAdjacent(0, sur->triangles.getSize());

  //  fix neighbours
  sur->stitchTriangles();

  fclose(f);
  return true;
}

/*
    Quick dump of the surface to OBJ format
*/
bool saveOBJ(Surface &sur, const char *fileName, float scale){
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write out header info
  fprintf(f, "g Group0\n");

  //  dump vertex positions and normals
  int numVert = sur.vertices.getSize();
  for (int i = 0; i < numVert; i++){
    const Surface::Point *p = &sur.vertices.index(i);

    fprintf(f, "v %f %f %f\n", p->p.x, p->p.y, p->p.z);
    fprintf(f, "vn %f %f %f\n", p->n.x, p->n.y, p->n.z);
    }

  //  write out the triangle
  int numTri = sur.triangles.getSize();
  for (int i = 0; i < numTri; i++){
    const Surface::Triangle *tri = &sur.triangles.index(i);

    fprintf(f, "f ");
    for (int j = 0; j < 3; j++)
      fprintf(f, "%d//%d ", tri->v[j]+1, tri->v[j]+1);
    fprintf(f, "\n");
    }

  fclose(f);
  return true;
}
