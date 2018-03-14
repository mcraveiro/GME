/*
 * GUISETUP.C
 *
 * GUI setup script.
 * Should run before the GUI toolkit is started.
 * It creates the necessary controls for the data generators and the
 * envelope/filter reset mechanism.
 * The script takes the number of channels and sampling frequency
 * parameters (these are necessary to create the controls) from its
 * context.
 * The script has no input or output data.
 * After running once, the script deactivates the node.
 * The node using this script should be created with the highest
 * priority so it runs first.
 */
#include "scripts.h"

void guisetup_script(void)
{
  mgk_data_type type;
  guisetup_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  if(cxt && (type & T_BUFFER)) {
    int i;
    char name[100];
    gui_define_button(ENV_RESET);
    gui_define_button(FILT_RESET);
    for(i = 1; i <= cxt->NumChannels; i++) {
      sprintf(name, CT_SINAMP, i);
      gui_define_control(name, 0.1, 10.0, 1.0);
      sprintf(name, CT_SINFREQ, i);
      gui_define_control(name, 0.0, cxt->SampRate * 0.49, cxt->SampRate * 0.05 * i);
      sprintf(name, CT_NOISEAMP, i);
      gui_define_control(name, 0.1, 10.0, 1.0);
    }
  }
  mgk_set_node_priority(mgk_current_node(),0);
}

