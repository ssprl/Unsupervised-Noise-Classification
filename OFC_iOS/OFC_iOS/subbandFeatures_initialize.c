/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: subbandFeatures_initialize.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 15-Aug-2017 21:08:55
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "subbandFeatures.h"
#include "subbandFeatures_initialize.h"

/* Function Definitions */

/*
 * Arguments    : void
 * Return Type  : void
 */
void subbandFeatures_initialize(void)
{
  rt_InitInfAndNaN(8U);
  prevFrame_not_empty_init();
}

/*
 * File trailer for subbandFeatures_initialize.c
 *
 * [EOF]
 */
