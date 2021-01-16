/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: log21.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "log21.h"

/* Function Definitions */

/*
 * Arguments    : const float x_data[]
 *                const int x_size[2]
 *                float f_data[]
 *                int f_size[2]
 * Return Type  : void
 */
void b_log2(const float x_data[], const int x_size[2], float f_data[], int
            f_size[2])
{
  int k;
  float f;
  int eint;
  float inte;
  f_size[0] = 1;
  f_size[1] = (signed char)x_size[1];
  for (k = 0; k + 1 <= x_size[1]; k++) {
    if (x_data[k] == 0.0F) {
      f = ((real32_T)rtMinusInf);
    } else if ((!rtIsInfF(x_data[k])) && (!rtIsNaNF(x_data[k]))) {
      f = (float)frexp(x_data[k], &eint);
      inte = (float)eint;
      if (f == 0.5F) {
        f = inte - 1.0F;
      } else {
        f = (float)log(f) / 0.693147182F + inte;
      }
    } else {
      f = x_data[k];
    }

    f_data[k] = f;
  }
}

/*
 * File trailer for log21.c
 *
 * [EOF]
 */
