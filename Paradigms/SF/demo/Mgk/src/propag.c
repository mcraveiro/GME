
/**
 ** PROPAG.C ---- dataflow propagation routines
 **/

#include "libmgk.h"

mgk_nodep
mgk_current_node(void)
{
  return (__mgk_running_node);
}

void *
mgk_receive(unsigned int port, mgk_data_type * typep)
{
  mgk_inport *ip;
  mgk_value *vp;
  void *result;
  if (!__mgk_running_node) {
    mgk_errno = E_NOTRUNNING;
    *typep = T_NODATA;
    return (NULL);
  }
  if (port >= __mgk_running_node->nin) {
    mgk_errno = E_BADPORTIX;
    *typep = T_NODATA;
    return (NULL);
  }
  ip = &__mgk_running_node->ins[port];
  if (!(vp = ip->head)) {
    mgk_errno = E_SUCCESS;
    *typep = T_NODATA;
    return (NULL);
  }
  ip->head = vp->next;
  ip->qlen--;
  result = __mgk_extract_value(vp, typep, &ip->scalar_val);
  if (*typep & T_BUFFER) {
    mgk_buffer *bp = __mgk_checked_buffer_header(vp->value);
    if (bp && !bp->refval) {
      bp->next = __mgk_running_node->buffer_list;
      __mgk_running_node->buffer_list = bp;
    }
  }
  return (result);
}

mgk_error_code
mgk_propagate(unsigned int port,
	      void *data,
	      mgk_data_type type)
{
  mgk_value *vp, *remval = NULL;
  mgk_outport *op;
  mgk_link *lp;
  int refcnt = 0;
  if (!__mgk_running_node) {
    return (mgk_errno = E_NOTRUNNING);
  }
  if (port >= __mgk_running_node->nout) {
    return (mgk_errno = E_BADPORTIX);
  }
  if (!(op = &__mgk_running_node->outs[port])->links) {
    return (mgk_errno = E_PORTCONN);
  }
  if (!(vp = __mgk_build_value(data, type))) {
    return (mgk_errno);
  }
  for (lp = op->links; lp; lp = lp->l.l2l.nextout) {
    mgk_value *sendval;
    mgk_node *dst;
    mgk_inport *ip;
    switch (lp->type) {
      case LINK_L2L:
	sendval = (lp == op->links) ? vp : __mgk_copy_value(vp);
	dst = lp->l.l2l.dst;
	ip = &dst->ins[lp->dstidx];
	if (ip->head) {
	  ip->tail->next = sendval;
	}
	else {
	  ip->head = sendval;
	}
	ip->tail = sendval;
	sendval->next = NULL;
	ip->qlen++;
	if ((ip->head == ip->tail) &&
	    (dst->status == WAITING) &&
	    (dst->priority > MGK_NODE_STOP_PRIORITY)) {
	  __mgk_update(dst);
	}
	break;
      case LINK_L2R:
	if (!remval) {
	  remval = (lp == op->links) ? vp : __mgk_copy_value(vp);
	}
	sendval = __mgk_alias_value_autofree(remval, &refcnt);
	if (lp->l.l2r.head) {
	  lp->l.l2r.tail->next = sendval;
	}
	else {
	  lp->l.l2r.head = sendval;
	}
	lp->l.l2r.tail = sendval;
	sendval->next = NULL;
	lp->l.l2r.qlen++;
	if (!lp->l.l2r.active) {
	  __mgk_remote_propagate(lp);
	}
	break;
      default:
	return (mgk_errno = E_GENERROR);
    }
  }
  return (mgk_errno = E_SUCCESS);
}

mgk_error_code
mgk_protect_buffer(void *buffer)
{
  mgk_buffer *bp = __mgk_checked_buffer_header(buffer);
  if (bp) {
    if (!__mgk_running_node) {
      return (mgk_errno = E_NOTRUNNING);
    }
    if (bp->refval) {
      return (mgk_errno = E_BUFFERUSED);
    }
    bp->refval = __mgk_running_node;
    return (mgk_errno = E_SUCCESS);
  }
  return (mgk_errno = E_NOTBUFFER);
}

mgk_error_code
mgk_unprotect_buffer(void *buffer)
{
  mgk_buffer *bp = __mgk_checked_buffer_header(buffer);
  if (bp) {
    if (!__mgk_running_node) {
      return (mgk_errno = E_NOTRUNNING);
    }
    if (bp->refval != __mgk_running_node) {
      return (mgk_errno = E_BUFFERUSED);
    }
    bp->refval = NULL;
    return (mgk_errno = E_SUCCESS);
  }
  return (mgk_errno = E_NOTBUFFER);
}

void
mgk_abort_node(int code)
{
  if (__mgk_running_node) {
    longjmp(__mgk_node_abort_context, code);
  }
  mgk_errno = E_NOTRUNNING;
}

unsigned int
mgk_ifany_trigger(void)
{
  unsigned int i, tport;
  mgk_inport *ip;
  if (!__mgk_running_node) {
    mgk_errno = E_NOTRUNNING;
    return ((unsigned int)(-1));
  }
  if (__mgk_running_node->tmode != AT_IFANY) {
    mgk_errno = E_BADTRIGGER;
    return ((unsigned int)(-1));
  }
  mgk_errno = E_SUCCESS;
  tport = __mgk_running_node->last_ifany_port;
  for (i = 0, ip = __mgk_running_node->ins;
       i < __mgk_running_node->nin;
       i++, ip++) {
    tport = (tport + 1) % __mgk_running_node->nin;
    if (ip->links && ip->head) {
      __mgk_running_node->last_ifany_port = tport;
      return (tport);
    }
  }
  return (MGK_U_BADVAL);
}

mgk_error_code
mgk_trigger_mask(mgk_portmask mask)
{
  int i;
  if (!__mgk_running_node) {
    return (mgk_errno = E_NOTRUNNING);
  }
  for (i = 0; i < itemsof(__mgk_running_node->data_mask); i++) {
    mask[i] = __mgk_running_node->data_mask[i];
  }
  return (mgk_errno = E_SUCCESS);
}
