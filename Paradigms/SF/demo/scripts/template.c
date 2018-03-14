/*
 * TEMPLATE.C
 *
 * One input one output script template working on
 * single percision floating point buffer data.
 */
#include "scripts.h"

void template_script(void)
{
  mgk_data_type type;
  float *data;
  data = mgk_receive(0,&type);
  if(data && (type == (T_FLOAT | T_BUFFER))) {
    int size = mgk_buffer_size(data) / sizeof(float);
    /* Do something with the data */
    /* ... */
    /* Propagate the data. The input buffer can be reused as the Kernel */
    /* protects data from aliasing */
    mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
  }
}

