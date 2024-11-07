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
    COMPOSITE SPHERE REDUCER - WILL TRY A NUMBER OF ALGORITHMS AND CHOOSE THE
   ONE THAT GIVES THE BEST RESULTS. TYPICALLY USED TO CHOOSE BETWEEN MERGE AND
   EXPAND AS THERE ARE SOME PLACES WHERE THE EXPAND ALGORITHM WILL DO REALLY BAD
   - EG A TUBE
*/
#ifndef _SR_COMPOSITE_H_
#define _SR_COMPOSITE_H_

#include "SRBase.h"
#include "SEBase.h"
#include "../Storage/Array.h"

class SRComposite : public SRBase {
public:
  SEBase *eval;
  bool useRefit;

  SRComposite();

  void resetReducers();
  void addReducer(SRBase *red);

  void setupForLevel(int level, int degree,
                     const SurfaceRep *surRep = NULL) const;
  void getSpheres(Array<Sphere> *spheres, int n, const SurfaceRep &surRep,
                  const Sphere *filterSphere = NULL,
                  float parSphereErr = -1) const;

private:
  Array<SRBase *> reducers;
};

#endif