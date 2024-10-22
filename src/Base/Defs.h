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

#ifndef _DEFS_H_
#define _DEFS_H_

#ifdef __cplusplus
#include <cmath>
#define finite(a) std::isfinite(a)
#endif

#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937511
#endif

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
#ifndef _AFXDLL

  // disable all printf's from algs
  inline void nullPrint(const char*format, ...){
    //  does nothing
  }

  #define OUTPUTINFO nullPrint

#endif
*/

#ifdef OUTPUTINFO
#undef OUTPUTINFO
#endif

void traceFunc(const char *format, ...);

#ifdef WIN32
#if !defined(_AFXDLL) || defined(CONSOLE)
#define OUTPUTINFO printf
#else
#define OUTPUTINFO traceFunc
#endif
#else
#define OUTPUTINFO printf
#endif

#ifdef WIN32
#define finite(a) _finite(a)
#endif

__inline long intPow(float x, int p) {
  if (p <= 0)
    return 1;

  float tot = x;
  for (int i = 1; i < p; i++)
    tot *= x;

  return (long)tot;
}

#endif
