
/**
 ** SCHED.C ----- dataflow node scheduler
 **/

#include <malloc.h>

#include "libmgk.h"
#include "allocate.h"

unsigned long __mgk_tick = 0L;
static mgk_node *schedule = NULL;
static int ready_qlen = 0;

#define NODE_DYNPRI(n) (unsigned int)(	\
  (n)->priority +			\
  __mgk_tick -				\
  (n)->ready_time			\
)

void
__mgk_enqueue(mgk_node * node)
{
  mgk_node **npp, *np;
  for (npp = &schedule; (np = *npp); npp = &np->next) {
    if (np == node) {
      return;
    }
    if (NODE_DYNPRI(np) < node->priority) {
      break;
    }
  }
  node->ready_time = __mgk_tick;
  node->next = np;
  *npp = node;
  ready_qlen++;
}

void
__mgk_dequeue(mgk_node * node)
{
  mgk_node **npp, *np;
  for (npp = &schedule; (np = *npp); npp = &np->next) {
    if (np == node) {
      *npp = np->next;
      ready_qlen--;
    }
  }
}

mgk_node *
__mgk_next_node(void)
{
  mgk_node *np = schedule;
  if (np) {
    schedule = np->next;
    __mgk_tick++;
    ready_qlen--;
  }
  return (np);
}
