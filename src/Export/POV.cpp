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

#include "POV.h"
#include <stdio.h>

bool exportSpheresPOV(const char *fileName, const Array<Sphere> &sph,
                      float scale, bool useDefFile, float *m, Array<int> *sel,
                      float selColor[], Array<int> *sel1, float selColor1[]) {
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write header
  fprintf(f, "/* POV Sphere Exporter (c) Gareth Bradshaw 2002 */\n");

  //  background color
  fprintf(f, "background { rgb <1, 1, 1> } \n\n");

  if (useDefFile)
    fprintf(f, "#include \"defs.inc\"\n");

  if (sel) {
    fprintf(f, "#ifndef (SELCOLOR)\n");
    if (selColor)
      fprintf(f, "  #declare SELCOLOR= rgb<%f, %f, %f> \n", selColor[0],
              selColor[1], selColor[2]);
    else
      fprintf(f, "  #declare SELCOLOR= rgb<1, 0, 0> \n");
    fprintf(f, "#end\n");
  }

  if (sel1) {
    fprintf(f, "#ifndef (SELCOLOR1)\n");
    if (selColor1)
      fprintf(f, "  #declare SELCOLOR1= rgb<%f, %f, %f> \n", selColor1[0],
              selColor1[1], selColor1[2]);
    else
      fprintf(f, "  #declare SELCOLOR1= rgb<0, 1, 0> \n");
    fprintf(f, "#end\n");
  }

  //  start spheres
  fprintf(f, "union {\n");

  //  dunp spheres
  int numSph = sph.getSize();
  for (int i = 0; i < numSph; i++) {
    Sphere s = sph.index(i);
    fprintf(f, "  sphere { ");
    fprintf(f, "<%f, %f, %f>, %f ", s.c.x * scale, s.c.y * scale, s.c.z * scale,
            s.r * scale);
    if (sel && sel->inList(i))
      fprintf(f, "pigment { SELCOLOR }");
    else if (sel1 && sel1->inList(i))
      fprintf(f, "pigment { SELCOLOR1 }");
    fprintf(f, "}\n");
  }

  //  euler rots
  fprintf(f, "  #ifdef (XROT)\n    rotate<XROT, 0, 0>\n  #end\n");
  fprintf(f, "  #ifdef (YROT)\n    rotate<0, YROT, 0>\n  #end\n");
  fprintf(f, "  #ifdef (ZROT)\n    rotate<0, 0, ZROT>\n  #end\n");

  //  matrix
  if (m) {
    fprintf(f, " matrix <");
    for (int i = 0; i < 12; i++) {
      if (i != 0)
        fprintf(f, ", ");
      fprintf(f, "%f ", m[i]);
    }
    fprintf(f, ">\n");
  } else {
    fprintf(
        f, "  #ifdef (MATRIX)\n    matrix < MATRIX[0],  MATRIX[1],  MATRIX[2], "
           " MATRIX[3],  MATRIX[4],  MATRIX[5],  MATRIX[6],  MATRIX[7],  "
           "MATRIX[8],  MATRIX[9],  MATRIX[10],  MATRIX[11]>\n  #end\n");
  }

  //  finish
  fprintf(f, "  #ifdef (COLOR)\n    pigment { COLOR }\n  #else\n    pigment { "
             "rgb <0.75,0.75,0> }\n  #end\n");
  fprintf(f, "  finish { diffuse 0.9 ambient 0 specular 0 }\n");
  fprintf(f, "  #ifdef (SCALE)\n    scale SCALE\n  #end\n");

  //  flip Z for open GL
  fprintf(f, "  scale <1, 1, -1>\n");
  fprintf(f, "}\n\n");

  //  camera
  fprintf(f, "camera { \n");
  fprintf(f, "  location <0,0,-10>\n");
  fprintf(f, "  look_at <0,0,0> \n");
  fprintf(f, "  sky <0,1,0> \n");
  fprintf(f, "  up <0,1,0> \n");
  fprintf(f, "  angle 20 \n");
  fprintf(f, "} \n\n");

  //  light sourse near vierer's right shoulder
  fprintf(f, "light_source { <0,0,-10> rgb 1*<1,1,1> }\n");

  fclose(f);
  return true;
}

bool exportModelPOV(const char *fileName, const Surface &sur, float scale) {
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write out the model
  fprintf(f, "mesh { \n"); //  begin mesh
  int numTri = sur.triangles.getSize();
  for (int i = 0; i < numTri; i++) {
    fprintf(f, "  triangle { "); //  begin triangle
    const Surface::Triangle *tri = &sur.triangles.index(i);

    for (int j = 0; j < 3; j++) {
      Point3D p = sur.vertices.index(tri->v[j]).p;
      fprintf(f, " <%f, %f, %f>", p.x * scale, p.y * scale, p.z * scale);
      if (j != 2)
        fprintf(f, ", ");
    }

    fprintf(f, "}\n"); // end triangle
  }

  fprintf(f, "  texture { pigment { color rgb<1, 0.75, 0.5> }}\n");
  fprintf(f, "  }\n"); //  end mesh

  //  close file
  fclose(f);
  return true;
}

bool exportSphereTreePOV(const char *fileName, const SphereTree &st,
                         bool topLevel, float scale, bool useDefFile) {
  //  open file
  FILE *f = fopen(fileName, "w");
  if (!f)
    return false;

  //  write header
  fprintf(f, "/* POV SphereTree Exporter (c) Gareth Bradshaw 2002 */\n");

  //  background color
  fprintf(f, "background { rgb <1, 1, 1> } \n\n");

  if (useDefFile)
    fprintf(f, "#include \"defs.inc\"\n");

  //  write out the level one spheres
  float colors[][3] = {{1, 0, 0},     {0, 1, 0}, {0, 0, 1},
                       {1, 0.5, 0},   {1, 0, 1}, {0, 1, 1},
                       {1, 0.5, 0.5}, {1, 1, 0}, {0.5, 0.5, 1}};
  const int numColors = 9;

  if (topLevel) {
    //  write out the top level of spheres
    unsigned long start, num;
    st.getRow(&start, &num, 1);
    fprintf(f, "union{\n");
    for (int i = 0; i < num; i++) {
      //  get sphere
      Sphere s = st.nodes.index(start + i);

      //  write out sphere
      fprintf(f, "  sphere { ");
      fprintf(f, "<%f, %f, %f>, %f ", s.c.x * scale, s.c.y * scale,
              s.c.z * scale, s.r * scale);
      int colI = i % numColors;
      fprintf(f, "pigment { rgb<%f,%f,%f> }", colors[colI][0], colors[colI][1],
              colors[colI][2]);
      fprintf(f, "}\n");
    }

    //  euler rots
    fprintf(f, "  #ifdef (XROT)\n    rotate<XROT, 0, 0>\n  #end\n");
    fprintf(f, "  #ifdef (YROT)\n    rotate<0, YROT, 0>\n  #end\n");
    fprintf(f, "  #ifdef (ZROT)\n    rotate<0, 0, ZROT>\n  #end\n");

    //  finish union
    fprintf(f, "  #ifdef (SCALE)\n    scale SCALE\n  #end\n");
    fprintf(f, "}\n\n");
  } else {
    // write out the second level of spheres
    unsigned long start, num;
    st.getRow(&start, &num, 1);
    fprintf(f, "union{\n");

    for (int i = 0; i < num; i++) {
      // child sphere less the bits that overlap (other spheres - parent)
      fprintf(f, "  difference {\n");

      //  union of children
      fprintf(f, "    union{\n");
      int firstChild = st.getFirstChild(start + i);
      for (int j = 0; j < st.degree; j++) {
        Sphere s = st.nodes.index(firstChild + j);
        if (s.r > 0) {
          fprintf(f, "      sphere { ");
          fprintf(f, "<%f, %f, %f>, %f ", s.c.x * scale, s.c.y * scale,
                  s.c.z * scale, s.r * scale);
          fprintf(f, "}\n");
        }
      }
      fprintf(f, "      }\n");

      //  union of other spheres - parent
      fprintf(f, "    difference {\n");

      //  union of other spheres
      fprintf(f, "      union{\n");
      for (int k = 0; k < num; k++) {
        Sphere s = st.nodes.index(start + k);
        if (s.r > 0 && k != i) {
          fprintf(f, "        sphere { ");
          fprintf(f, "<%f, %f, %f>, %f ", s.c.x * scale, s.c.y * scale,
                  s.c.z * scale, s.r * scale);
          fprintf(f, "}\n");
        }
      }
      fprintf(f, "        }\n");

      //  sphere
      Sphere s = st.nodes.index(start + i);
      fprintf(f, "      sphere { ");
      fprintf(f, "<%f, %f, %f>, %f ", s.c.x * scale, s.c.y * scale,
              s.c.z * scale, s.r * scale);
      fprintf(f, "}\n");

      //  end difference
      fprintf(f, "      }\n");

      //  end intersection
      int colI = i % numColors;
      fprintf(f, "    pigment { rgb<%f,%f,%f> }\n", colors[colI][0],
              colors[colI][1], colors[colI][2]);
      fprintf(f, "    }\n");
    }

    //  euler rots
    fprintf(f, "  #ifdef (XROT)\n    rotate<XROT, 0, 0>\n  #end\n");
    fprintf(f, "  #ifdef (YROT)\n    rotate<0, YROT, 0>\n  #end\n");
    fprintf(f, "  #ifdef (ZROT)\n    rotate<0, 0, ZROT>\n  #end\n");

    //  finish union
    fprintf(f, "  #ifdef (SCALE)\n    scale SCALE\n  #end\n");
    fprintf(f, "}\n\n");
  }
  //  camera
  fprintf(f, "camera { \n");
  fprintf(f, "  location <0,0,10>\n");
  fprintf(f, "  look_at <0,0,0> \n");
  fprintf(f, "  sky <0,1,0> \n");
  fprintf(f, "  up <0,1,0> \n");
  fprintf(f, "  right <1,0,0> \n"); //  aspect ratio of image ??
  fprintf(f, "  angle 20 \n");
  fprintf(f, "} \n\n");

  //  light sourse near vierer's right shoulder
  fprintf(f, "light_source { <0,0,10> rgb 1*<1,1,1> }\n");

  fclose(f);
  return true;
}
