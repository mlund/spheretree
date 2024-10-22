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
    Base definition for all our exceptions,
      all exceptions carry information about :

      the condition which failed
      the file and line number

   when using MRC
    we inherit from MFC's base class when using MFC so that MFC
      can catch the excaption and display a message box (for menu items etc.)
*/
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#ifdef _AFXDLL
#include <afxwin.h>
#endif

class Exception
#ifdef _AFXDLL
    : public virtual CException
#endif
{

public:
  const char *cond, *file;
  int line;

  Exception();
  Exception(const char *cond, const char *file, int line);

  virtual bool getMessage(char *str, int maxSize) const;
#ifdef _AFXDLL
  virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
                               PUINT pnHelpContext = NULL) {
    return getMessage(lpszError, nMaxError);
  }
#endif
};

#endif