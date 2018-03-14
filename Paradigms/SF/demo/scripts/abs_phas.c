/*
 * ABS_PHAS.C
 *
 * Converts a complex input pair into absolute value and phase outputs.
 * No context used.
 */
#include "scripts.h"

void abs_phase_script(void)
{
  mgk_data_type rtype,itype;
  int size;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("abs+phase\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((size = (mgk_buffer_size(re) / sizeof(float))) > 0)) {
    int i;
    for(i = 0; i < size; i++) {
      float magn = (float)(sqrt((re[i] * re[i]) + (im[i] * im[i])));
      float phase = (float)(atan2(im[i],re[i]) * 180.0 / M_PI);
      re[i] = magn;
      im[i] = phase;
    }
    mgk_propagate(0,re,rtype);
    mgk_propagate(1,im,itype);
  }
}

