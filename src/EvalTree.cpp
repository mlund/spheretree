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

#include "EvalTree.h"

void evaluateTree(Array<LevelEval> *res, const SphereTree &st,
                  const SEBase *eval) {
  int numLevels = st.levels;
  res->resize(numLevels);

  for (int i = 0; i < numLevels; i++) {
    LevelEval *le = &res->index(i);

    unsigned long start, num;
    st.getRow(&start, &num, i);

    le->numSph = 0;
    le->bestError = FLT_MAX;
    le->worstError = -FLT_MAX;
    le->avgError = 0;

    for (int j = 0; j < num; j++) {
      const Sphere s = st.nodes.index(start + j);
      if (s.r > 0) {
        double err = eval->evalSphere(s);

        le->numSph++;
        le->avgError += err;

        if (err > le->worstError)
          le->worstError = err;

        if (err < le->bestError)
          le->bestError = err;
      }
    }

    le->avgError /= le->numSph;
  }
}

void writeEvaluation(FILE *f, const Array<LevelEval> &res) {
  fprintf(f, "Evaluation :\n");
  int numLevels = res.getSize();
  for (int i = 0; i < numLevels; i++) {
    const LevelEval *le = &res.index(i);

    fprintf(f, "\tLevel %d\n", i);
    fprintf(f, "\t\tNum\t: %d\n", le->numSph);
    fprintf(f, "\t\tWorst\t: %.3f\n", le->worstError);
    fprintf(f, "\t\tBest\t: %.3f\n", le->bestError);
    fprintf(f, "\t\tMean\t: %.3f\n", le->avgError);
  }
}
