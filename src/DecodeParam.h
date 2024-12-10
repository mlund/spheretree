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

#ifndef _DECODE_PARAM_H_
#define _DECODE_PARAM_H_

#include <stdio.h>

//  compare two strings case insensitive (stricmp is non-standard)
//  just returns 0/1 (0 if equal to be same as strcmp)
int strcompare(const char *s1, const char *s2);

/*
    Decoding float parameters e.g. -max 4.0
*/
struct FloatParam {
  const char *name; //  parameter name e.g. max
  float *var;       //  pointer to store value e.g. 4.0
};

bool decodeFloatParam(int argc, const char *argv[], const FloatParam params[]);

/*
    Decoding int parameters e.g. -iters 9
*/
struct IntParam {
  const char *name; //  parameter name e.g. iters
  int *var;         //  pointer to store value e.g. 9
};

bool decodeIntParam(int argc, const char *argv[], const IntParam params[]);

/*
    Decoding bool params e.g. -pause sets pause to true
*/
struct BoolParam {
  const char *name; //  parameter name e.g. pause
  bool *var;        //  variable to set
  bool val;         //  value to set variable
};

bool decodeBoolParam(int argc, const char *argv[], const BoolParam params[]);

/*
    write out the parameters
*/
void writeParam(FILE *f, const FloatParam params[]);
void writeParam(FILE *f, const IntParam params[]);
void writeParam(FILE *f, const BoolParam params[]);

#endif