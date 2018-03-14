/*
 * ENVELOPE.C
 *
 * Computes envelope (= peak hold) for an input signal.
 * The context stores the previous maximums in a buffer allocated
 * by the script first time it is executed.
 * The other elements of the context are used to reset the envelopes:
 * There is a global reset count parameter and a local copy of it.
 * Anytime the two are different, the script resets the envelope.
 * The script also monitors the GUI envlope reset button and
 * whenever depressed it increments the global reset count parameter.
 * This is to ensure that all envelope scripts notice the "reset" event.
 */
#include "scripts.h"

void envelope_script(void)
{
  mgk_data_type type;
  envelope_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("env\n");
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
      if(gui_get_button_value(ENV_RESET)) {
	(*(cxt->Reset))++;
      }
      if(*(cxt->Reset) != cxt->LastReset) {
	do_reset = TRUE;
	cxt->LastReset = *(cxt->Reset);
      }
      if(do_reset) {
	for(i = 0; i < size; i++) {
	  ((float *)(cxt->Buffer))[i] = data[i];
	}
      }
      else {
	for(i = 0; i < size; i++) {
	  data[i] = ((float *)(cxt->Buffer))[i] =
	    max(data[i],((float *)(cxt->Buffer))[i]);
	}
      }
      mgk_propagate(0,data,(T_BUFFER | T_FLOAT));
    }
  }
}

