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

#include "DomainSampler.h"
#include "../Base/Defs.h"

int findInfo(const SamplerInfo infos[], float v, int n) {
  //  binary search
  int minI = 0, maxI = n - 1;
  while (minI < maxI - 1) {
    int midI = (minI + maxI) / 2;
    if (infos[midI].val < v) {
      //  use midI..maxI
      minI = midI;
    } else {
      //  use minI..midI
      maxI = midI;
    }
  }

  if (infos[minI].val < v)
    return maxI;
  else
    return minI;
}

int samplerInfoCompare(const void *elem1, const void *elem2) {
  SamplerInfo *i1 = (SamplerInfo *)elem1;
  SamplerInfo *i2 = (SamplerInfo *)elem2;
  float d = i1 - i2;
  if (d < 0)
    return -1;
  else if (d > 0)
    return +1;
  else
    return 0;
}

void sampleDomain(Array<int> *counts, SamplerInfo infos[], int numVals,
                  int numSamples) {
  //  setup counts
  counts->reallocate(numVals);

  //  initialise domain info & counts
  for (int i = 0; i < numVals; i++) {
    SamplerInfo *inf = &infos[i];
    inf->sourceI = i;

    counts->index(i) = 0;
  }

  //  order the infos in increasing order
  qsort(infos, numVals, sizeof(SamplerInfo), samplerInfoCompare);

  //  get total of the domain values
  float total = 0.0f;
  for (int i = 0; i < numVals; i++) {
    total += infos[i].val;
  }

  //   make cumulative and work out maxSamples
  float totalSofar = 0.0f;
  for (int i = 0; i < numVals; i++) {
    SamplerInfo *s = &infos[i];
    s->maxSamples = numSamples * (s->val / total);
    totalSofar += s->val;
    s->val = totalSofar;
  }

  //  generate samples
  for (int i = 0; i < numSamples; i++) {
    //  pick a random triangle
    float r = rand() / (float)RAND_MAX;
    int j = findInfo(infos, r * total, numVals);

    if (j == -1) {
      OUTPUTINFO("Problem attributing sample");
      i--;
    } else {
      SamplerInfo *s = &infos[j];
      if (counts->index(s->sourceI) <= s->maxSamples) {
        counts->index(s->sourceI)++; //  do sample
      } else {
        i--; //  reject sample
      }
    }
  }
}

void sampleDomain(Array<int> *counts, const Array<float> &vals,
                  int numSamples) {
  //  initialise domain info
  int numVals = vals.getSize();
  SamplerInfo *infos = new SamplerInfo[numVals];
  for (int i = 0; i < numVals; i++)
    infos[i].val = vals.index(i);

  //   do sample
  sampleDomain(counts, infos, numVals, numSamples);

  delete infos;
}
