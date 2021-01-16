/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: xcorr.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "xcorr.h"

/* Function Definitions */

/*
 * Arguments    : const float x[64]
 *                const float varargin_1[64]
 *                float c[127]
 * Return Type  : void
 */
void xcorr(const float x[64], const float varargin_1[64], float c[127])
{
  int k;
  int ihi;
  float s;
  int i;
  memset(&c[0], 0, 127U * sizeof(float));
  for (k = 0; k < 64; k++) {
    ihi = -k;
    s = 0.0F;
    for (i = 0; i <= ihi + 63; i++) {
      s += varargin_1[i] * x[k + i];
    }

    c[k + 63] = s;
  }

  for (k = 0; k < 63; k++) {
    ihi = 64 - k;
    s = 0.0F;
    for (i = 0; i <= ihi - 2; i++) {
      s += varargin_1[(k + i) + 1] * x[i];
    }

    c[62 - k] = s;
  }
}

/*
 * File trailer for xcorr.c
 *
 * [EOF]
 */
