/**
 ** UPDATE.C ---- figure out node's schedulability
 **/

#include "libmgk.h"

void
__mgk_update(mgk_node * np)
{
  int i;
  int maskv;
  mgk_inport *ip;
  mgk_mask_trigger *tp;
  node_status newstat;

  newstat = WAITING;
  if (np->priority != MGK_NODE_STOP_PRIORITY) {
    for (i = 0; i < itemsof(np->data_mask); i++) {
      np->data_mask[i] = 0;
    }
    for (i = 0, ip = np->ins; i < (int)np->nin; i++, ip++) {
      if (ip->head) {
	mgk_set_portmask_bit(np->data_mask, i, 1);
      }
    }
    switch (np->tmode) {
      case AT_IFALL:
	for (i = maskv = 0; i < itemsof(np->data_mask); i++) {
	  maskv |= (~np->data_mask[i] & np->input_mask[i]);
	}
	newstat = maskv ? WAITING : READY;
	break;
      case AT_IFANY:
	for (i = maskv = 0; i < itemsof(np->data_mask); i++) {
	  maskv |= (np->data_mask[i] & np->input_mask[i]);
	}
	newstat = maskv ? READY : WAITING;
	break;
      case AT_SPEC:
	for (tp = np->trigger_list; tp; tp = tp->next) {
	  for (i = maskv = 0; i < itemsof(np->data_mask); i++) {
	    maskv |= (~np->data_mask[i] & np->input_mask[i] & tp->mask[i]);
	  }
	  if (maskv == 0) {
	    break;
	  }
	}
	newstat = ((np->active_trigger = tp)) ? READY : WAITING;
	break;
    }
  }
  switch (np->status) {
    case RUNNING:
      break;
    case READY:
      if (newstat == WAITING) {
	np->status = WAITING;
	__mgk_dequeue(np);
      }
      break;
    case WAITING:
      if (newstat == READY) {
	np->status = READY;
	__mgk_enqueue(np);
      }
      break;
  }
}
