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

#include "DecodeParam.h"
#include <string.h>

//  compare two strings case insensitive (stricmp is non-standard)
//  just returns 0/1 (0 if equal to be same as strcmp)
char cap(char c) {
  if (c >= 'a' && c <= 'z')
    return c + 'A' - 'a';
  return c;
}

int strcompare(const char *s1, const char *s2) {
  int n1 = strlen(s1);
  int n2 = strlen(s2);

  if (n1 != n2)
    return 1;

  for (int i = 0; i < n1; i++) {
    if (cap(s1[i]) != cap(s2[i]))
      return 1;
  }

  return 0; //  equal
}

bool decodeFloatParam(int argc, const char *argv[], const FloatParam params[]) {
  for (int i = 0; i < argc; i++) {
    // is it a parameter we are dealing with
    if (argv[i] && argv[i][0] == '-') {
      //  try to find matching parameter
      int j = 0;
      while (params[j].name != NULL &&
             strcompare(argv[i] + 1, params[j].name) != 0) {
        j++;
      }

      if (params[j].name != NULL) {
        //  check we have next parameter
        if (argv[i + 1] == NULL) {
          printf("Missing Parameter for %s\n", argv[i]);
          return false;
        }

        //  read float value
        float v;
        if (sscanf(argv[i + 1], "%f", &v) != 1) {
          printf("Need Float Parameter for %s\n", argv[i]);
          return false;
        }

        //  fill value
        *params[j].var = v;

        // erase parameter
        argv[i] = argv[i + 1] = NULL;
      }
    }
  }

  return true;
}

bool decodeIntParam(int argc, const char *argv[], const IntParam params[]) {
  for (int i = 0; i < argc; i++) {
    // is it a parameter we are dealing with
    if (argv[i] && argv[i][0] == '-') {
      //  try to find matching parameter
      int j = 0;
      while (params[j].name != NULL &&
             strcompare(argv[i] + 1, params[j].name) != 0) {
        j++;
      }

      if (params[j].name != NULL) {
        //  check we have next parameter
        if (argv[i + 1] == NULL) {
          printf("Missing Parameter for %s\n", argv[i]);
          return false;
        }

        //  read int value
        int v;
        if (sscanf(argv[i + 1], "%d", &v) != 1) {
          printf("Need Int Parameter for %s\n", argv[i]);
          return false;
        }

        //  fill value
        *params[j].var = v;

        // erase parameter
        argv[i] = argv[i + 1] = NULL;
      }
    }
  }

  return true;
}

bool decodeBoolParam(int argc, const char *argv[], const BoolParam params[]) {
  for (int i = 0; i < argc; i++) {
    // is it a parameter we are dealing with
    if (argv[i] && argv[i][0] == '-') {
      //  try to find matching parameter
      int j = 0;
      while (params[j].name != NULL &&
             strcompare(argv[i] + 1, params[j].name) != 0) {
        j++;
      }

      if (params[j].name != NULL) {
        //  fill value
        *params[j].var = params[j].val;

        // erase parameter
        argv[i] = NULL;
      }
    }
  }

  return true;
}

void writeParam(FILE *f, const FloatParam params[]) {
  int j = 0;
  while (params[j].name != NULL) {
    const char *tabs = "\t";
    if (strlen(params[j].name) < 8)
      tabs = "\t\t";

    fprintf(f, "\t%s%s: %f\n", params[j].name, tabs, *params[j].var);
    j++;
  }
}

void writeParam(FILE *f, const IntParam params[]) {
  int j = 0;
  while (params[j].name != NULL) {
    const char *tabs = "\t";
    if (strlen(params[j].name) < 8)
      tabs = "\t\t";

    fprintf(f, "\t%s%s: %d\n", params[j].name, tabs, *params[j].var);
    j++;
  }
}

void writeParam(FILE *f, const BoolParam params[]) {
  int j = 0;
  while (params[j].name != NULL) {
    const char *tabs = "\t";
    if (strlen(params[j].name) < 8)
      tabs = "\t\t";

    fprintf(f, "\t%s%s: %s\n", params[j].name, tabs,
            (*params[j].var) ? "TRUE" : "FALSE");
    j++;
  }
}
