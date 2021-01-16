/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: power.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "power.h"

/* Function Definitions */

/*
 * Arguments    : const float a[256]
 *                float y[256]
 * Return Type  : void
 */
void power(const float a[256], float y[256])
{
  int k;
  for (k = 0; k < 256; k++) {
    y[k] = (float)sqrt(a[k]);
  }
}

/*
 * File trailer for power.c
 *
 * [EOF]
 */
