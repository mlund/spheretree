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
    Definitions for basic types :

        BOOLEAN, BYTE, REAL & INTEGER

    change these to improve memory/cpu performance
*/
#ifndef _TYPES_H_
#define _TYPES_H_
#include <limits.h>
#include <float.h>

//  BOOLEAN  (using ANSI now)
// #define BOOLEAN unsigned int
// #ifndef TRUE
//  #define TRUE  0xFF
//  #define FALSE 0x00
// #endif

//  BYTE
typedef unsigned char OCTET;
#define OCTET_MAX 255
#define OCTET_MIN 0

//  INTEGER
typedef int INT;
// #define INT_MAX     //  already done
// #define INT_MIN

//  REAL
typedef double REAL;
#define REAL_MAX DBL_MAX
#define REAL_TINY DBL_EPSILON
#define REAL_MIN -DBL_MAX

//  EPSILON
#define EPSILON 1E-10
#define EPSILON_LARGE 1E-5

#endif
