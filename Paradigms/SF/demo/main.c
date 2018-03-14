/**
 ** MAIN.C ---- main program for the MGK+GUI demo
 **/
#include <string.h>
#include <stdio.h>
#include <mgk60.h>
#include <simgui.h>

#include "System.h"

/* 
 * This runs the application
 * The GUI will call it (can be activated from the 'Run' menu)
 */
void 
runprog(int do_dialog)
{
  for (;;) {
    mgk_run(1);
    gui_update_plots();
  }
}

/*
 * Main program:
 * - initializes MGK
 * - builds the application
 * - configures the GUI based on application parameters
 * - starts the GUI
 */
int 
main(int argc, char **argv)
{
  void register_scripts(void);
  void build(void);
  if (mgk_initialize(&argc, &argv) == E_SUCCESS) {
    register_scripts();
    if (mgk_local_host_index() == 0) {
      build();
      gui_register_runmenu_entry(runprog, "MGK demo");
      gui_debug_to_console();
      /* this should run the GUI init node if priorities are set right! */
      mgk_run(1);
      gui_start();
    }
    else {
      char buf[100];
      sprintf(buf,"stdout.%d",mgk_local_host_index());
      freopen(buf,"w",stdout);
      sprintf(buf,"stderr.%d",mgk_local_host_index());
      freopen(buf,"w",stderr);
      for (;;) {
	if (!mgk_run(1000)) {
	  mgk_wait_for_comm_event();
	}
      }
    }
    return (0);
  }
  return (-1);
}
