/*
 * FFT_SCR.C
 *
 * Time domain complex FFT script.
 * No context used.
 */
#include "scripts.h"

static
#if defined(__GNUC__) || defined(__cplusplus)
inline
#endif
int
ilog2(int value)
{
  int ilog;
  for(ilog = 0; (1 << ilog) != value; ilog++) {
    if((1 << ilog) == 0) {
      return(-1);
    }
  }
  return(ilog);
}

void fft_script(void)
{
  mgk_data_type rtype,itype;
  int logsize;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("fft\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((logsize = ilog2(mgk_buffer_size(re) / sizeof(float))) >= 3)) {
    fft32(re,im,logsize,0);
    mgk_propagate(0,re,rtype);
    mgk_propagate(1,im,itype);
  }
}

