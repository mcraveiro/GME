/*
 * DBSETUP.C
 *
 * GUI data base setup script.
 * Should be run once for every data base (data set) used in
 * the processing.
 * Initializes the data base, defines the default X axis variable for
 * X-Y plots and initializes these variables.
 * Takes the number of samples, sampling frequency and time/frequency
 * domain flag parameters form the context.
 * No input/output data propagation.
 * Deactivates itself after first run
 */
#include "scripts.h"

void dbsetup_script(void)
{
  mgk_data_type type;
  dbsetup_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  if(cxt && (type & T_BUFFER)) {
    int i;
    int size = cxt->IsFrequency ?
      cxt->Size / 2 :
      cxt->Size;
    char *xname = cxt->IsFrequency ?
      "Frequency" :
      "Time";
    double step = cxt->IsFrequency ?
      cxt->SampRate / cxt->Size :
      1.0 / cxt->SampRate;
    float *data = mgk_allocate_buffer((sizeof(float) * size),FALSE);
    for(i = 0; i < size; i++) {
      data[i] = (float)i * (float)step;
    }
    gui_setup_database(cxt->Name,xname);
    gui_clear_database(cxt->Name);
    gui_append_data(xname, cxt->Name, data, size);
  }
  mgk_set_node_priority(mgk_current_node(),0);
}

