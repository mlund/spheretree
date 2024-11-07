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

#include "VRML.h"
#include <stdio.h>

bool exportSpheresVRML(const char *fileName, const Array<Sphere> &sph,
                       float scale) {
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write header
  fprintf(f, "#VRML V2.0 utf8\n#Gareth Bradshaw \n");

  //  set the color
  // fprintf(f, "Material { diffuseColor 0.75 0.75 0 } \n");

  //  dump spheres
  int numSph = sph.getSize();
  for (int i = 0; i < numSph; i++) {
    Sphere s = sph.index(i);
    fprintf(f, "Transform{ \n");
    fprintf(f, "  translation %f %f %f \n", s.c.x * scale, s.c.y * scale,
            s.c.z * scale);
    fprintf(f, "  children [ \n");
    fprintf(f, "    Shape { \n");
    fprintf(f, "      geometry Sphere { radius  %f } \n", s.r * scale);
    fprintf(f, "      }\n");
    fprintf(f, "    ]\n");
    fprintf(f, "  }\n");
  }

  fclose(f);
  return true;
}

void exportTree(FILE *f, const char *pre, const SphereTree &st, int node,
                int level, float scale) {
  if (level >= st.levels)
    return;

  Sphere s = st.nodes.index(node);
  if (s.r <= 0)
    return;

  fprintf(f, "Transform{ \n");
  fprintf(f, "  translation %f %f %f \n", s.c.x * scale, s.c.y * scale,
          s.c.z * scale);
  fprintf(f, "  children [ \n");
  fprintf(f, "    Shape { \n");
  fprintf(f, "      geometry DEF %s  Sphere { radius  %f } \n", pre,
          s.r * scale);
  fprintf(f, "      }\n");
  fprintf(f, "    ]\n");
  fprintf(f, "  }\n");

  char buffer[1024];
  int firstChild = st.getFirstChild(node);
  for (int i = 0; i < st.degree; i++) {
    sprintf(buffer, "%s-%d", pre, i);
    exportTree(f, buffer, st, firstChild + i, level + 1, scale);
  }
}

bool exportTreeVRML(const char *fileName, const SphereTree &st, float scale) {
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write header
  fprintf(f, "#VRML V2.0 utf8\n# Gareth Bradshaw \n");

  //  set the color
  // fprintf(f, "Material { diffuseColor 0.75 0.75 0 } \n");

  //  dump spheres
  exportTree(f, "Obj2", st, 0, 0, scale);

  fclose(f);
  return true;
}