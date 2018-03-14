
/**
 ** INIT.C ---- global data and initialization code
 **/

#include <stdio.h>
#include <stdlib.h>

#include "libmgk.h"

static mgk_host the_host =
{
  OBJ_HOST,
  0
};

mgk_error_code mgk_errno = E_SUCCESS;
mgk_node *__mgk_running_node = NULL;
jmp_buf __mgk_node_abort_context;
mgk_host *__mgk_host_table = &the_host;
mgk_host *__mgk_local_host = &the_host;
int __mgk_num_hosts = 1;

mgk_error_code
mgk_initialize(int *argc, char ***argv)
{
  return (E_SUCCESS);
}

int
__mgk_remote_call(mgk_host * host, mgk_remcmd cmd,
		  int argc, mgk_value ** argv,
		  int rargc, mgk_value ** rargv)
{
  fprintf(stderr, "'__mgk_remote_call' called in sequential version!\n");
  abort();
  return(0);
}

void
__mgk_remote_propagate(mgk_link * lk)
{
  fprintf(stderr, "'__mgk_remote_propagate' called in sequential version!\n");
  abort();
}
