/*
 * PLOTTER.C
 *
 * Data plotter script.
 * It just enters its input data into the data base of the GUI toolkit.
 * The actual plot updades will be done under the control of the GUI event
 * loop.
 * The script requires a preset character string context which contains the
 * (space separated) names for the GUI variable and data set.
 */
#include "scripts.h"

void plotter_script(void)
{
  char varname[100];
  mgk_data_type type;
  plotter_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("plot\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    int size;
    float *data = mgk_receive(0,&type);
    if((data) &&
       (type == (T_FLOAT | T_BUFFER)) &&
       ((size = mgk_buffer_size(data) / sizeof(float)) > 0)) {
      sprintf(varname,"Chan%02d",cxt->Channel);
      gui_clear_data(varname,cxt->DBase);
      gui_append_data(varname,cxt->DBase,data,size);
    }
  }
}

