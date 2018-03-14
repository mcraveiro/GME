/*
 * DATAGEN.C
 *
 * Data generator script.
 * Generates a mized sine wave and noise signal.
 * Gets generation parameters from GUI controls.
 * Gets data parameters (channel, sampling rate and buffer size) from context.
 */
#include "scripts.h"

void datagen_script(void)
{
  mgk_data_type type;
  datagen_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("gen\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    double samp;
    double sfreq;
    double namp;
    float *data;
    char cname[100];
    sprintf(cname,CT_SINAMP,cxt->Channel);
    samp = gui_get_control_value(cname);
    sprintf(cname,CT_SINFREQ,cxt->Channel);
    sfreq = gui_get_control_value(cname);
    sprintf(cname,CT_NOISEAMP,cxt->Channel);
    namp = gui_get_control_value(cname);
    data = mgk_allocate_buffer((sizeof(float) * cxt->Size),0);
    if(data) {
      int i;
      for(i = 0; i < cxt->Size; i++) {
	data[i] = (float)(
	  samp * cos(2.0 * M_PI * sfreq * i / cxt->Frequency) +
	  namp * (double)(rand() - (RAND_MAX / 2)) * (2.0 / RAND_MAX));
      }
      mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
    }
  }
}

