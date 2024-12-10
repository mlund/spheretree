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
    Macros to check condition during debug only, in release the macros evaluate
   to nothing

    CHECK_DEBUGn takes n parameters to the message
*/
#ifndef _CHECK_DEBUG_H_
#define _CHECK_DEBUG_H_

#include "Exception.h"
#include "ErrorLog.h"

#ifndef _DEBUG
//  definitions for relase mode
#define CHECK_DEBUG0(cond)
#define CHECK_DEBUG(cond, msg)
#define CHECK_DEBUG1(cond, msg, a)
#define CHECK_DEBUG2(cond, msg, a, b)
#define CHECK_DEBUG3(cond, msg, a, b, c)
#define CHECK_DEBUG4(cond, msg, a, b, c, d)
#else
#define CHECK_DEBUG0(cond)                                                     \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, NULL);                            \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#define CHECK_DEBUG(cond, msg)                                                 \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, msg);                             \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#define CHECK_DEBUG1(cond, msg, a)                                             \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, msg, a);                          \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#define CHECK_DEBUG2(cond, msg, a, b)                                          \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, msg, a, b);                       \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#define CHECK_DEBUG3(cond, msg, a, b, c)                                       \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, msg, a, b, c);                    \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#define CHECK_DEBUG4(cond, msg, a, b, c, d)                                    \
  if (!(cond)) {                                                               \
    writeErrorLog(#cond, __FILE__, __LINE__, msg, a, b, c, d);                 \
    throw new Exception(#cond, __FILE__, __LINE__);                            \
  }

#endif

#endif