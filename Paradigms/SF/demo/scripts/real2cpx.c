/*
 * REAL2CPX.C
 *
 * Complex data generator script.
 * Propagates its input data unchanged on its first output.
 * Generates an identically sized zero-filled data buffer for the second
 * output.
 * Can be used to generate the complex input for the FFT script from
 * single channel real data.
 * Uses no context.
 */
#include "scripts.h"

void real2complex_script(void)
{
  mgk_data_type type;
  float *data;
  printf("real2complex\n");
  fflush(stdout);
  data = mgk_receive(0,&type);
  if(data && (type == (T_FLOAT | T_BUFFER))) {
    int i,size = mgk_buffer_size(data) / sizeof(float);
    float *zero = mgk_allocate_buffer((sizeof(float) * size),0);
    if(zero) {
      for(i = 0; i < size; i++) {
	zero[i] = 0.0f;
      }
      mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
      mgk_propagate(1,zero,(T_FLOAT | T_BUFFER));
    }
  }
}

