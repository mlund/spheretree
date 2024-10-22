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

#include "ErrorLog.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
char errorLogDir[1024] = "./Errors";

#ifdef _AFXDLL
#include "CallStack.h"
#endif

void writeErrorLog(const char *cond, const char *file, int line,
                   const char *msg, ...) {
  char fileName[1024] = "";

  long currentTime = time(NULL);
  char *now = asctime(localtime(&currentTime));
  if (now)
    sprintf(fileName, "%s/%.6s %.4s.txt", errorLogDir, &now[4], &now[20]);

  FILE *f = fopen(fileName, "a");
  if (!f)
    f = fopen("errors.txt", "a");

  if (f) {
    //  write out header
    if (now)
      fprintf(f, "Error Logged At %s", now);
    else
      fprintf(f, "Error Logged At ???");
    if (file)
      fprintf(f, "\tFile\t\t\t:\t%s (%d)\n", file, line);
    if (cond)
      fprintf(f, "\tFailed Expression\t:\t%s\n", cond);
    if (msg) {
      fprintf(f, "\tProgrammers Comment\t:\t", msg);
      va_list params;
      va_start(params, msg);
      vfprintf(f, msg, params);
      fprintf(f, "\n");
    }

#ifdef _AFXDLL
    //  dump call stack
    dumpCallStack(f, "\t\t");
#endif

    //  footer
    fprintf(f, "End Error Log\n");
    for (int i = 0; i < 80; i++)
      fprintf(f, "/");
    fprintf(f, "\n");
    fclose(f);
  }
}
