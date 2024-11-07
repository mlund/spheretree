#ifndef _VOL_INT_C_
#define _VOL_INT_C_

#include "Surface.h"
void computeMassProperties(REAL *mass, REAL R[3], REAL J[3][3],
                           const Surface &sur);

#endif