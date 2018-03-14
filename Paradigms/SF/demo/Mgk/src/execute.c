
/**
 ** EXECUTE.C ---- run a node
 **/

#include "libmgk.h"

void
__mgk_execute_node(mgk_node * np)
{
  mgk_buffer *bp, *protlist;
  np->status = RUNNING;
  if (setjmp(__mgk_node_abort_context) == 0) {
    __mgk_running_node = np;
    switch (np->tmode) {
      case AT_IFANY:
      case AT_IFALL:
	(*np->script) ();
	break;
      case AT_SPEC:
	(*np->active_trigger->script) ();
	break;
    }
    __mgk_running_node = NULL;
    np->status = WAITING;
    __mgk_update(np);
  }
  else {
    __mgk_running_node = NULL;
    np->status = WAITING;
    np->priority = MGK_NODE_STOP_PRIORITY;
  }
  for (bp = np->buffer_list, protlist = NULL; bp; bp = np->buffer_list) {
    np->buffer_list = bp->next;
    if (!bp->refval) {
      __mgk_free_buffer_header(bp);
    }
    else if (bp->refval == np) {
      bp->next = protlist;
      protlist = bp;
    }
    else {
      bp->next = NULL;
    }
  }
  np->buffer_list = protlist;
}
