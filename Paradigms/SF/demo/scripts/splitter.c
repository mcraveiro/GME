/*
 * SPLITTER.C
 *
 * Splits a complex FFT output into separate channels.
 * Assumes that the FFT time domain inputs were two separate real data
 * channels.
 * No context used.
 */
#include "scripts.h"

void splitter_script(void)
{
  mgk_data_type rtype,itype;
  int size;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("split\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((size = (mgk_buffer_size(re) / sizeof(float))) >= 4) &&
     ((size & 1) == 0)) {
    int hsize = size / 2;
    float *re1 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *im1 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *re2 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *im2 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    if(re1 && im1 && re2 && im2) {
      int i;
      for(i = 0; i < hsize; i++) {
	int j = i ? (size - i) : 0;
	re1[i] = 0.5f * (re[i] + re[j]);
	re2[i] = 0.5f * (re[i] - re[j]);
	im1[i] = 0.5f * (im[i] - im[j]);
	im2[i] = 0.5f * (im[i] + im[j]);
      }
      mgk_propagate(0,re1,(T_FLOAT | T_BUFFER));
      mgk_propagate(1,im1,(T_FLOAT | T_BUFFER));
      mgk_propagate(2,re2,(T_FLOAT | T_BUFFER));
      mgk_propagate(3,im2,(T_FLOAT | T_BUFFER));
    }
  }
}

