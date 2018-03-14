/*
 * LPFILTER.C
 *
 * Low-pass filter script.
 * Uses exponential averaging:
 * out = history = in * Factor + history * (1 - Factor)
 * The filter factor comes from the context.
 * Uses a storage buffer and reset mechanism similar to the
 * envelope script.
 */
#include "scripts.h"

void lpfilter_script(void)
{
  mgk_data_type type;
  lpfilter_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("lpfilter\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    int i,size,do_reset = FALSE;
    float *data = mgk_receive(0,&type);
    if((data) &&
       (type == (T_FLOAT | T_BUFFER)) &&
       ((size = mgk_buffer_size(data) / sizeof(float)) > 0)) {
      if(!cxt->Buffer) {
	cxt->Buffer = mgk_allocate_buffer((sizeof(float) * size),0);
	if(!cxt->Buffer) {
	  return;
	}
	mgk_protect_buffer(cxt->Buffer);
	do_reset = TRUE;
      }
      if(gui_get_button_value(FILT_RESET)) {
	(*cxt->Reset)++;
      }
      if(*cxt->Reset != cxt->LastReset) {
	do_reset = TRUE;
	cxt->LastReset = *cxt->Reset;
      }
      if(do_reset) {
	for(i = 0; i < size; i++) {
	  ((float *)(cxt->Buffer))[i] = data[i];
	}
      }
      else {
	for(i = 0; i < size; i++) {
	  data[i] = ((float *)(cxt->Buffer))[i] = (float)(
	    data[i] * cxt->Factor +
	    ((float *)(cxt->Buffer))[i] * (1.0 - cxt->Factor));
	}
      }
      mgk_propagate(0,data,(T_BUFFER | T_FLOAT));
    }
  }
}

