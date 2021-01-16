/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: norm.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "norm.h"

/* Function Definitions */

/*
 * Arguments    : const float x[64]
 * Return Type  : float
 */
float norm(const float x[64])
{
  float y;
  float scale;
  int k;
  float absxk;
  float t;
  y = 0.0F;
  scale = 1.17549435E-38F;
  for (k = 0; k < 64; k++) {
    absxk = (float)fabs(x[k]);
    if (absxk > scale) {
      t = scale / absxk;
      y = 1.0F + y * t * t;
      scale = absxk;
    } else {
      t = absxk / scale;
      y += t * t;
    }
  }

  return scale * (float)sqrt(y);
}

/*
 * File trailer for norm.c
 *
 * [EOF]
 */
