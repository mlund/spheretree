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
    tests sphere trees generated with various methods
*/
#include <stdio.h>
#include <string.h>

#include "Surface/Surface.h"
#include "Surface/OBJLoader.h"
#include "API/STGOctree.h"
#include "DecodeParam.h"

/*
    options and their default values
*/
int depth = 3;        //  depth of the sphere-tree
bool nopause = false; //  will we pause before starting
bool eval = false;    //  do we evaluate the sphere-tree after construction

/*
    info for decoding parameters
*/
const IntParam intParams[] = {{"depth", &depth}, {NULL, NULL}};

const BoolParam boolParams[] = {{"nopause", &nopause, TRUE}, {NULL, NULL}};

/*
    forward declarations
*/
void waitForKey();
int error(const char *errorMsg, const char *errorMsg1 = NULL);
bool constructTree(const char *file);

/*
    MAINLINE
*/
int main(int argc, const char *argv[]) {
  printf("MakeTreeOctree - Gareth Bradshaw Feb 2003\n");

  /*
     parse command line
  */
  decodeIntParam(argc, argv, intParams);
  decodeBoolParam(argc, argv, boolParams);
  printf("Options : \n");
  writeParam(stdout, intParams);
  writeParam(stdout, boolParams);

  /*
     look for filenames and construct trees
  */
  int numFiles = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i] != NULL) {
      constructTree(argv[i]);
      numFiles++;
    }
  }

  /*
     check we had a file name
  */
  if (numFiles == 0)
    error("no files given :(");

  waitForKey();
}

/*
    construct sphere-tree for the model
*/
bool constructTree(const char *file) {
  printf("FileName : %s\n\n", file);

  /*
      generate the filename
  */
  char inputFile[1024];
  strcpy(inputFile, file);
  int len = strlen(inputFile);
  int i;
  for (i = len - 1; i >= 0; i--) {
    if (inputFile[i] == '.')
      break;
  }

  /*
      load the surface model
  */
  Surface sur;
  bool loaded = false;
  if (strcompare("obj", &inputFile[len - 3]) == 0)
    loaded = loadOBJ(&sur, inputFile);
  else
    loaded = sur.loadSurface(inputFile);

  if (!loaded) {
    printf("ERROR : Unable to load input file (%s)\n\n", inputFile);
    return false;
  }
  if (i >= 0)
    inputFile[i] = '\0';

  /*
      scale box
  */
  float boxScale = sur.fitIntoBox(1000);

  /*
      make sphere-tree
  */
  SphereTree tree;
  tree.setupTree(8, depth + 1);
  waitForKey();

  STGOctree treegen;
  treegen.setSurface(sur);
  treegen.constructTree(&tree);

  /*
     save sphere-tree
  */
  char sphereFile[1024];
  sprintf(sphereFile, "%s-octree.sph", inputFile);
  if (tree.saveSphereTree(sphereFile, 1.0f / boxScale)) {
    FILE *f = fopen(sphereFile, "a");
    if (f) {
      fprintf(f, "Options : \n");
      writeParam(f, intParams);
      writeParam(f, boolParams);
      fprintf(f, "\n");
      fclose(f);
    }
    return true;
  } else {
    return false;
  }
}

/*
    error handler
*/
int error(const char *errorMsg, const char *errorMsg1) {
  if (errorMsg1)
    printf("ERROR : %s (%s)\n", errorMsg, errorMsg1);
  else
    printf("ERROR : %s\n", errorMsg);

  waitForKey();
  exit(-1);
}

void waitForKey() {
  if (!nopause) {
    printf("Press ENTER to continue....\n");
    char buffer[2];
    fread(buffer, 1, 1, stdin);
  }
}
