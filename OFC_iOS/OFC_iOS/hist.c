/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: hist.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "hist.h"

/* Function Definitions */

/*
 * Arguments    : const float Y[64]
 *                double no[64]
 * Return Type  : void
 */
void hist(const float Y[64], double no[64])
{
  int k;
  float miny;
  float maxy;
  float edges[65];
  float delta1;
  double nn[65];
  int xind;
  int low_i;
  int low_ip1;
  int high_i;
  int mid_i;
  k = 0;
  while ((k + 1 <= 64) && (!((!rtIsInfF(Y[k])) && (!rtIsNaNF(Y[k]))))) {
    k++;
  }

  if (k + 1 > 64) {
    miny = 0.0F;
    maxy = 0.0F;
  } else {
    miny = Y[k];
    maxy = Y[k];
    while (k + 1 <= 64) {
      if ((!rtIsInfF(Y[k])) && (!rtIsNaNF(Y[k]))) {
        if (Y[k] < miny) {
          miny = Y[k];
        }

        if (Y[k] > maxy) {
          maxy = Y[k];
        }
      }

      k++;
    }
  }

  if (miny == maxy) {
    miny = (miny - 32.0F) - 0.5F;
    maxy = (maxy + 32.0F) - 0.5F;
  }

  if (((miny < 0.0F) != (maxy < 0.0F)) && (((float)fabs(miny) > 1.70141173E+38F)
       || ((float)fabs(maxy) > 1.70141173E+38F))) {
    delta1 = miny / 64.0F;
    maxy /= 64.0F;
    for (k = 0; k < 63; k++) {
      edges[1 + k] = (miny + maxy * (1.0F + (float)k)) - delta1 * (1.0F + (float)
        k);
    }
  } else {
    delta1 = (maxy - miny) / 64.0F;
    for (k = 0; k < 63; k++) {
      edges[1 + k] = miny + (1.0F + (float)k) * delta1;
    }
  }

  edges[0] = ((real32_T)rtMinusInf);
  edges[64] = ((real32_T)rtInf);
  for (k = 0; k < 63; k++) {
    maxy = (float)fabs(edges[k + 1]);
    if ((!rtIsInfF(maxy)) && (!rtIsNaNF(maxy))) {
      if (maxy <= 1.17549435E-38F) {
        maxy = 1.4013E-45F;
      } else {
        frexp(maxy, &xind);
        maxy = (float)ldexp(1.0, xind - 24);
      }
    } else {
      maxy = ((real32_T)rtNaN);
    }

    edges[k + 1] += maxy;
  }

  memset(&nn[0], 0, 65U * sizeof(double));
  xind = 0;
  for (k = 0; k < 64; k++) {
    low_i = 0;
    if (!rtIsNaNF(Y[xind])) {
      if ((Y[xind] >= edges[0]) && (Y[xind] < edges[64])) {
        low_i = 1;
        low_ip1 = 2;
        high_i = 65;
        while (high_i > low_ip1) {
          mid_i = (low_i + high_i) >> 1;
          if (Y[xind] >= edges[mid_i - 1]) {
            low_i = mid_i;
            low_ip1 = mid_i + 1;
          } else {
            high_i = mid_i;
          }
        }
      }

      if (Y[xind] == edges[64]) {
        low_i = 65;
      }
    }

    if (low_i > 0) {
      nn[low_i - 1]++;
    }

    xind++;
  }

  memcpy(&no[0], &nn[0], sizeof(double) << 6);
  no[63] += nn[64];
}

/*
 * File trailer for hist.c
 *
 * [EOF]
 */
