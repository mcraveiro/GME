/**
 ** CONNECT.C ----- dataflow graph node connection stuff
 **/

#include <malloc.h>
#include <string.h>

#include "libmgk.h"
#include "allocate.h"

DEFINE_ALLOCATOR(mgk_link, 200, l.l2l.nextin);

static int dfltlen = 1000;

unsigned int
mgk_set_default_connection_length(unsigned int newlen)
{
  unsigned int retval = dfltlen;
  dfltlen = newlen;
  return (retval);
}

static mgk_error_code
do_connect(mgk_nodep src, unsigned int srcport,
	   mgk_nodep dst, unsigned int dstport,
	   unsigned int maxlen, int callremote)
{
  mgk_link *lp;
  mgk_value *args[7];
  mgk_value *rargs[1];
  mgk_remnode *rsrc, *rdst;
  mgk_data_type t;
  mgk_scalar s;
  int *errp;
  /* check types, limits */
  if (__mgk_check_node(src)) {
    if (srcport >= src->nout) {
      return (mgk_errno = E_BADPORTIX);
    }
    rsrc = NULL;
  }
  else if (__mgk_check_remnode(src)) {
    rsrc = (mgk_remnode *) src;
  }
  else {
    return (mgk_errno = E_NOTNODE);
  }
  if (__mgk_check_node(dst)) {
    if (dstport >= dst->nin) {
      return (mgk_errno = E_BADPORTIX);
    }
    rdst = NULL;
  }
  else if (__mgk_check_remnode(dst)) {
    rdst = (mgk_remnode *) dst;
  }
  else {
    return (mgk_errno = E_NOTNODE);
  }
  /* check for nodes already connected (local side only) */
  if (!rsrc) {
    for (lp = src->outs[srcport].links; lp; lp = lp->l.l2l.nextout) {
      if ((lp->srcidx == srcport) && (lp->dstidx == dstport)) {
	switch (lp->type) {
	  case LINK_L2L:
	    if (!rdst && (lp->l.l2l.src == src) && (lp->l.l2l.dst == dst)) {
	      return (mgk_errno == E_PORTCONN);
	    }
	    break;
	  case LINK_L2R:
	    if (rdst && (lp->l.l2r.src == src) && (lp->l.l2r.dst == rdst)) {
	      return (mgk_errno == E_PORTCONN);
	    }
	    break;
	  default:
	    break;
	}
      }
    }
  }
  if (!rdst) {
    for (lp = dst->ins[dstport].links; lp; lp = lp->l.l2l.nextin) {
      if ((lp->srcidx == srcport) && (lp->dstidx == dstport)) {
	switch (lp->type) {
	  case LINK_L2L:
	    if (!rsrc && (lp->l.l2l.src == src) && (lp->l.l2l.dst == dst)) {
	      return (mgk_errno == E_PORTCONN);
	    }
	    break;
	  case LINK_R2L:
	    if (rsrc && (lp->l.r2l.src == rsrc) && (lp->l.r2l.dst == dst)) {
	      return (mgk_errno == E_PORTCONN);
	    }
	    break;
	  default:
	    break;
	}
      }
    }
  }
  if (rsrc && rdst) {
    if (!callremote) {
      return (mgk_errno = E_GENERROR);
    }
    args[0] = __mgk_build_value(&rsrc->ID, T_LONGINT);
    args[1] = __mgk_build_value(&rsrc->host->index, T_INTEGER);
    args[2] = __mgk_build_value(&srcport, T_INTEGER);
    args[3] = __mgk_build_value(&rdst->ID, T_LONGINT);
    args[4] = __mgk_build_value(&rdst->host->index, T_INTEGER);
    args[5] = __mgk_build_value(&dstport, T_INTEGER);
    args[6] = __mgk_build_value(&maxlen, T_INTEGER);
    if (__mgk_remote_call(rsrc->host,
			  cmd_connect_nodes,
			  7, args,
			  1, rargs)) {
      errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!(lp = ALLOC_mgk_link())) {
    return (mgk_errno = E_NOMEM);
  }
  if (rsrc) {
    if (callremote) {
      args[0] = __mgk_build_value(&rsrc->ID, T_LONGINT);
      args[1] = __mgk_build_value(&rsrc->host->index, T_INTEGER);
      args[2] = __mgk_build_value(&srcport, T_INTEGER);
      args[3] = __mgk_build_value(&dst->ID, T_LONGINT);
      args[4] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      args[5] = __mgk_build_value(&dstport, T_INTEGER);
      args[6] = __mgk_build_value(&maxlen, T_INTEGER);
      if (__mgk_remote_call(rsrc->host,
			    cmd_connect_nodes,
			    7, args,
			    1, rargs)) {
	errp = __mgk_extract_value(rargs[0], &t, &s);
	mgk_errno = (t == T_INTEGER) ? *errp : E_COMM;
      }
      else {
	mgk_errno = E_COMM;
      }
    }
    else {
      mgk_errno = E_SUCCESS;
    }
    if (mgk_errno == E_SUCCESS) {
      lp->type = LINK_R2L;
      lp->srcidx = srcport;
      lp->dstidx = dstport;
      lp->l.r2l.src = rsrc;
      lp->l.r2l.dst = dst;
      lp->l.r2l.nextin = dst->ins[dstport].links;
      dst->ins[dstport].links = lp;
      dst->ins[dstport].maxlen = maxlen;
    }
    else {
      FREE_mgk_link(lp);
    }
    return (mgk_errno);
  }
  if (rdst) {
    if (callremote) {
      args[0] = __mgk_build_value(&src->ID, T_LONGINT);
      args[1] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      args[2] = __mgk_build_value(&srcport, T_INTEGER);
      args[3] = __mgk_build_value(&rdst->ID, T_LONGINT);
      args[4] = __mgk_build_value(&rdst->host->index, T_INTEGER);
      args[5] = __mgk_build_value(&dstport, T_INTEGER);
      args[6] = __mgk_build_value(&maxlen, T_INTEGER);
      if (__mgk_remote_call(rdst->host,
			    cmd_connect_nodes,
			    7, args,
			    1, rargs)) {
	errp = __mgk_extract_value(rargs[0], &t, &s);
	mgk_errno = (t == T_INTEGER) ? *errp : E_COMM;
      }
      else {
	mgk_errno = E_COMM;
      }
    }
    else {
      mgk_errno = E_SUCCESS;
    }
    if (mgk_errno == E_SUCCESS) {
      lp->type = LINK_L2R;
      lp->srcidx = srcport;
      lp->dstidx = dstport;
      lp->l.l2r.src = src;
      lp->l.l2r.dst = rdst;
      lp->l.l2r.qlen = 0;
      lp->l.l2r.maxlen = maxlen;
      lp->l.l2r.head = NULL;
      lp->l.l2r.tail = NULL;
      lp->l.l2r.active = NULL;
      lp->l.l2r.nextout = src->outs[srcport].links;
      src->outs[srcport].links = lp;
    }
    else {
      FREE_mgk_link(lp);
    }
    return (mgk_errno);
  }
  lp->type = LINK_L2L;
  lp->srcidx = srcport;
  lp->dstidx = dstport;
  lp->l.l2l.src = src;
  lp->l.l2l.dst = dst;
  lp->l.l2l.nextin = dst->ins[dstport].links;
  lp->l.l2l.nextout = src->outs[srcport].links;
  src->outs[srcport].links = lp;
  dst->ins[dstport].links = lp;
  dst->ins[dstport].maxlen = maxlen;
  return (mgk_errno = E_SUCCESS);
}

mgk_error_code
mgk_connect_nodes(mgk_nodep src, unsigned int srcport,
		  mgk_nodep dst, unsigned int dstport)
{
  return (do_connect(src, srcport, dst, dstport, dfltlen, TRUE));
}

mgk_error_code
mgk_connect_nodes_len(mgk_nodep src, unsigned int srcport,
		      mgk_nodep dst, unsigned int dstport,
		      unsigned int maxlen)
{
  return (do_connect(src, srcport, dst, dstport, maxlen, TRUE));
}

DEFSVC(connect_nodes)
{
  if ((argc == 7) && (rargc == 1)) {
    mgk_data_type t1, t2, t3, t4, t5, t6, t7;
    long *sidp = __mgk_parse_value(argv[0], &t1);
    int *shixp = __mgk_parse_value(argv[1], &t2);
    int *sidxp = __mgk_parse_value(argv[2], &t3);
    long *didp = __mgk_parse_value(argv[3], &t4);
    int *dhixp = __mgk_parse_value(argv[4], &t5);
    int *didxp = __mgk_parse_value(argv[5], &t6);
    int *maxlp = __mgk_parse_value(argv[6], &t7);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER) &&
	(t4 == T_LONGINT) && (t5 == T_INTEGER) && (t6 == T_INTEGER) &&
	(t7 == T_INTEGER)) {
      mgk_node *src = __mgk_locate_node(*shixp, *sidp);
      mgk_node *dst = __mgk_locate_node(*dhixp, *didp);
      do_connect(src, *sidxp, dst, *didxp, *maxlp, FALSE);
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

static mgk_error_code
do_disconnect(mgk_nodep src, unsigned int srcport,
	      mgk_nodep dst, unsigned int dstport,
	      int callremote)
{
  mgk_link *slp, *dlp, **slpp, **dlpp;
  mgk_value *args[6];
  mgk_value *rargs[1];
  mgk_remnode *rsrc, *rdst;
  mgk_data_type t;
  mgk_scalar s;
  int *errp;
  /* check types, limits */
  if (__mgk_check_node(src)) {
    if (srcport >= src->nout) {
      return (mgk_errno = E_BADPORTIX);
    }
    rsrc = NULL;
  }
  else if (__mgk_check_remnode(src)) {
    rsrc = (mgk_remnode *) src;
  }
  else {
    return (mgk_errno = E_NOTNODE);
  }
  if (__mgk_check_node(dst)) {
    if (dstport >= dst->nin) {
      return (mgk_errno = E_BADPORTIX);
    }
    rdst = NULL;
  }
  else if (__mgk_check_remnode(dst)) {
    rdst = (mgk_remnode *) dst;
  }
  else {
    return (mgk_errno = E_NOTNODE);
  }
  /* find connections (local side only) */
  if (rsrc) {
    slpp = NULL;
    slp = NULL;
  }
  else {
    for (slpp = &src->outs[srcport].links;
	 (slp = *slpp);
	 slpp = &slp->l.l2l.nextout) {
      if ((slp->srcidx == srcport) && (slp->dstidx == dstport)) {
	switch (slp->type) {
	  case LINK_L2L:
	    if (!rdst && (slp->l.l2l.src == src) && (slp->l.l2l.dst == dst)) {
	      break;
	    }
	    continue;
	  case LINK_L2R:
	    if (rdst && (slp->l.l2r.src == src) && (slp->l.l2r.dst == rdst)) {
	      break;
	    }
	    continue;
	  default:
	    continue;
	}
	break;
      }
    }
    if (!slp) {
      return (mgk_errno = E_PORTCONN);
    }
  }
  if (rdst) {
    dlpp = NULL;
    dlp = NULL;
  }
  else {
    for (dlpp = &dst->ins[dstport].links;
	 (dlp = *dlpp);
	 dlpp = &dlp->l.l2l.nextin) {
      if ((dlp->srcidx == srcport) && (dlp->dstidx == dstport)) {
	switch (dlp->type) {
	  case LINK_L2L:
	    if (!rsrc && (dlp->l.l2l.src == src) && (dlp->l.l2l.dst == dst)) {
	      break;
	    }
	    continue;
	  case LINK_R2L:
	    if (rsrc && (dlp->l.r2l.src == rsrc) && (dlp->l.r2l.dst == dst)) {
	      break;
	    }
	    continue;
	  default:
	    continue;
	}
	break;
      }
    }
    if (!dlp) {
      return (mgk_errno = E_PORTCONN);
    }
  }
  if (rsrc && rdst) {
    if (!callremote) {
      return (mgk_errno = E_GENERROR);
    }
    args[0] = __mgk_build_value(&rsrc->ID, T_LONGINT);
    args[1] = __mgk_build_value(&rsrc->host->index, T_INTEGER);
    args[2] = __mgk_build_value(&srcport, T_INTEGER);
    args[3] = __mgk_build_value(&rdst->ID, T_LONGINT);
    args[4] = __mgk_build_value(&rdst->host->index, T_INTEGER);
    args[5] = __mgk_build_value(&dstport, T_INTEGER);
    if (__mgk_remote_call(rsrc->host,
			  cmd_disconnect_nodes,
			  6, args,
			  1, rargs)) {
      errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (rsrc) {
    if (callremote) {
      args[0] = __mgk_build_value(&rsrc->ID, T_LONGINT);
      args[1] = __mgk_build_value(&rsrc->host->index, T_INTEGER);
      args[2] = __mgk_build_value(&srcport, T_INTEGER);
      args[3] = __mgk_build_value(&dst->ID, T_LONGINT);
      args[4] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      args[5] = __mgk_build_value(&dstport, T_INTEGER);
      if (__mgk_remote_call(rsrc->host,
			    cmd_disconnect_nodes,
			    6, args,
			    1, rargs)) {
	errp = __mgk_extract_value(rargs[0], &t, &s);
	mgk_errno = (t == T_INTEGER) ? *errp : E_COMM;
      }
      else {
	mgk_errno = E_COMM;
      }
    }
    else {
      mgk_errno = E_SUCCESS;
    }
    if (mgk_errno == E_SUCCESS) {
      *dlpp = dlp->l.r2l.nextin;
      FREE_mgk_link(dlp);
    }
    return (mgk_errno);
  }
  if (rdst) {
    if (callremote) {
      args[0] = __mgk_build_value(&src->ID, T_LONGINT);
      args[1] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      args[2] = __mgk_build_value(&srcport, T_INTEGER);
      args[3] = __mgk_build_value(&rdst->ID, T_LONGINT);
      args[4] = __mgk_build_value(&rdst->host->index, T_INTEGER);
      args[5] = __mgk_build_value(&dstport, T_INTEGER);
      if (__mgk_remote_call(rsrc->host,
			    cmd_disconnect_nodes,
			    6, args,
			    1, rargs)) {
	errp = __mgk_extract_value(rargs[0], &t, &s);
	mgk_errno = (t == T_INTEGER) ? *errp : E_COMM;
      }
      else {
	mgk_errno = E_COMM;
      }
    }
    else {
      mgk_errno = E_SUCCESS;
    }
    if (mgk_errno == E_SUCCESS) {
      *slpp = slp->l.l2r.nextout;
      if (slp->l.l2r.active) {
	slp->l.l2r.active->backlink = NULL;
      }
      FREE_mgk_link(slp);
    }
    return (mgk_errno);
  }
  if (slp != dlp) {
    return (mgk_errno = E_GENERROR);
  }
  *slpp = slp->l.l2l.nextout;
  *dlpp = dlp->l.l2l.nextin;
  FREE_mgk_link(slp);
  return (mgk_errno = E_SUCCESS);
}

mgk_error_code
mgk_disconnect_nodes(mgk_nodep src, unsigned int srcport,
		     mgk_nodep dst, unsigned int dstport)
{
  return (do_disconnect(src, srcport, dst, dstport, TRUE));
}

DEFSVC(disconnect_nodes)
{
  if ((argc == 6) && (rargc == 1)) {
    mgk_data_type t1, t2, t3, t4, t5, t6;
    long *sidp = __mgk_parse_value(argv[0], &t1);
    int *shixp = __mgk_parse_value(argv[1], &t2);
    int *sidxp = __mgk_parse_value(argv[2], &t3);
    long *didp = __mgk_parse_value(argv[3], &t4);
    int *dhixp = __mgk_parse_value(argv[4], &t5);
    int *didxp = __mgk_parse_value(argv[5], &t6);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER) &&
	(t4 == T_LONGINT) && (t5 == T_INTEGER) && (t6 == T_INTEGER)) {
      mgk_node *src = __mgk_locate_node(*shixp, *sidp);
      mgk_node *dst = __mgk_locate_node(*dhixp, *didp);
      do_disconnect(src, *sidxp, dst, *didxp, FALSE);
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}
