/**
 ** RUN.C ---- execute a few dataflow nodes
 **/

#include "libmgk.h"

unsigned int
mgk_run(unsigned int num_nodes_to_run)
{
  unsigned int count = 0;
  while (num_nodes_to_run > 0) {
    mgk_node *next = __mgk_next_node();
    if (!next) {
      break;
    }
    __mgk_execute_node(next);
    count++;
    num_nodes_to_run--;
  }
  return (count);
}

void
mgk_set_comm_priority(unsigned int commpri)
{
  return;
}

void
mgk_wait_for_comm_event(void)
{
  return;
}
