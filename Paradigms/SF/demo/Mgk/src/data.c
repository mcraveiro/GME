
/** 
 ** DATA.C ---- data insertion/removal routines
 **/

#include "libmgk.h"

static mgk_value *last_peek_value = NULL;

mgk_error_code
mgk_write_node_input_port(void *data,
			  mgk_data_type type,
			  mgk_nodep node,
			  unsigned int port)
{
  mgk_inport *ip;
  mgk_value *vp;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[1];
    if (!(vp = __mgk_build_value(data, type))) {
      return (mgk_errno);
    }
    args[0] = vp;
    args[1] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[2] = __mgk_build_value(&port, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_write_node_input_port,
			  3, args,
			  1, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    return (mgk_errno = E_NOTNODE);
  }
  if (port >= node->nin) {
    return (mgk_errno = E_BADPORTIX);
  }
  ip = &node->ins[port];
  if (!(vp = __mgk_build_value(data, type))) {
    return (mgk_errno);
  }
  ip = &node->ins[port];
  if (ip->head) {
    ip->tail->next = vp;
  }
  else {
    ip->head = vp;
  }
  ip->tail = vp;
  vp->next = NULL;
  ip->qlen++;
  if ((ip->head == ip->tail) &&
      (node->status == WAITING) &&
      (node->priority > MGK_NODE_STOP_PRIORITY)) {
    __mgk_update(node);
  }
  return (mgk_errno = E_SUCCESS);
}

DEFSVC(write_node_input_port)
{
  if ((argc == 3) && (rargc == 1)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[1], &t1);
    int *idxp = __mgk_parse_value(argv[2], &t2);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER)) {
      mgk_data_type type;
      mgk_scalar svalue;
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      void *value = __mgk_extract_value(argv[0], &type, &svalue);
      int res = mgk_write_node_input_port(value, type, np, *idxp);
      if ((res != E_SUCCESS) && (type & T_BUFFER)) {
	__mgk_free_buffer(value);
      }
      argv[0] = NULL;
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_write_node_output_port(void *data,
			   mgk_data_type type,
			   mgk_nodep node,
			   unsigned int port,
			   unsigned int whichconn)
{
  mgk_link *lp;
  mgk_error_code res = E_SUCCESS;
  unsigned int done_any, idx;
  mgk_value *vp;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[4];
    mgk_value *rargs[1];
    if (!(args[0] = __mgk_build_value(data, type))) {
      return (mgk_errno);
    }
    args[1] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[2] = __mgk_build_value(&port, T_INTEGER);
    args[3] = __mgk_build_value(&whichconn, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_write_node_output_port,
			  4, args,
			  1, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    return (mgk_errno = E_NOTNODE);
  }
  if (port >= node->nout) {
    return (mgk_errno = E_BADPORTIX);
  }
  for (lp = node->outs[port].links, idx = 0, done_any = 0;
       lp;
       lp = lp->l.l2l.nextout, idx++) {
    if ((whichconn == MGK_ALL_OUTPUT_CONNECTIONS) || (whichconn == idx)) {
      switch (lp->type) {
	case LINK_L2L:
	  res = mgk_write_node_input_port(data, type,
					  lp->l.l2l.dst, lp->dstidx);
	  break;
	case LINK_L2R:
	  if (!(vp = __mgk_build_value(data, type))) {
	    return (mgk_errno);
	  }
	  if (lp->l.l2r.head) {
	    lp->l.l2r.tail->next = vp;
	  }
	  else {
	    lp->l.l2r.head = vp;
	  }
	  lp->l.l2r.tail = vp;
	  vp->next = NULL;
	  lp->l.l2r.qlen++;
	  if (!lp->l.l2r.active) {
	    __mgk_remote_propagate(lp);
	  }
	  res = E_SUCCESS;
	  break;
	default:
	  res = E_GENERROR;
	  break;
      }
      if (res != E_SUCCESS) {
	return (mgk_errno = res);
      }
      done_any++;
    }
  }
  if (!done_any) {
    return (mgk_errno = E_PORTCONN);
  }
  return (mgk_errno = res);
}

DEFSVC(write_node_output_port)
{
  if ((argc == 4) && (rargc == 1)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[1], &t1);
    int *idxp = __mgk_parse_value(argv[2], &t2);
    int *selp = __mgk_parse_value(argv[3], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      mgk_data_type type;
      mgk_scalar svalue;
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      void *value = __mgk_extract_value(argv[0], &type, &svalue);
      int res = mgk_write_node_output_port(value, type, np, *idxp, *selp);
      if ((res != E_SUCCESS) && (type & T_BUFFER)) {
	__mgk_free_buffer(value);
      }
      argv[0] = NULL;
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_clear_node_input_port(mgk_nodep node,
			  unsigned int port,
			  unsigned int count)
{
  mgk_inport *ip;
  mgk_value *vp;
  unsigned int i;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[1];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&count, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_clear_node_input_port,
			  3, args,
			  1, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    return (mgk_errno = E_NOTNODE);
  }
  if (port >= node->nin) {
    return (mgk_errno = E_BADPORTIX);
  }
  ip = &node->ins[port];
  for (i = 0; i < count; i++) {
    vp = ip->head;
    if (!vp) {
      break;
    }
    ip->head = vp->next;
    ip->qlen--;
    __mgk_free_value(vp);
  }
  if ((i > 0) && !ip->head) {
    __mgk_update(node);
  }
  return (mgk_errno = E_SUCCESS);
}

DEFSVC(clear_node_input_port)
{
  if ((argc == 3) && (rargc == 1)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *cntp = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int res = mgk_clear_node_input_port(np, *idxp, *cntp);
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_clear_node_output_port(mgk_nodep node,
			   unsigned int port,
			   unsigned int whichconn,
			   unsigned int count)
{
  mgk_link *lp;
  unsigned int i, idx, done_any;
  mgk_value *vp;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[4];
    mgk_value *rargs[1];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&whichconn, T_INTEGER);
    args[3] = __mgk_build_value(&count, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_clear_node_output_port,
			  4, args,
			  1, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[0], &t, &s);
      if (t == T_INTEGER) {
	return (mgk_errno = *errp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    return (mgk_errno = E_NOTNODE);
  }
  if (port >= node->nout) {
    return (mgk_errno = E_BADPORTIX);
  }
  for (lp = node->outs[port].links, idx = 0, done_any = 0;
       lp;
       lp = lp->l.l2l.nextout, idx++) {
    if ((whichconn == MGK_ALL_OUTPUT_CONNECTIONS) || (whichconn == idx)) {
      switch (lp->type) {
	case LINK_L2L:
	  break;
	case LINK_L2R:
	  for (i = 0; i < count; i++) {
	    if (!(vp = lp->l.l2r.head)) {
	      break;
	    }
	    lp->l.l2r.head = vp->next;
	    lp->l.l2r.qlen--;
	    __mgk_free_value(vp);
	  }
	  break;
	default:
	  return (mgk_errno = E_GENERROR);
      }
      done_any++;
    }
  }
  return (mgk_errno = (done_any ? E_SUCCESS : E_PORTCONN));
}

DEFSVC(clear_node_output_port)
{
  if ((argc == 4) && (rargc == 1)) {
    mgk_data_type t1, t2, t3, t4;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *selp = __mgk_parse_value(argv[2], &t3);
    int *cntp = __mgk_parse_value(argv[3], &t4);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) &&
	(t3 == T_INTEGER) && (t4 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int res = mgk_clear_node_output_port(np, *idxp, *selp, *cntp);
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

unsigned int
mgk_node_input_port_length(mgk_nodep node, unsigned int port)
{
  mgk_inport *ip;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_input_port_length,
			  2, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *lenp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*lenp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  if (port >= node->nin) {
    mgk_errno = E_BADPORTIX;
    return (MGK_U_BADVAL);
  }
  ip = &node->ins[port];
  mgk_errno = E_SUCCESS;
  return (ip->qlen);
}

DEFSVC(node_input_port_length)
{
  if ((argc == 2) && (rargc == 2)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int res = mgk_node_input_port_length(np, *idxp);
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

unsigned int
mgk_node_output_port_length(mgk_nodep node,
			    unsigned int port,
			    unsigned int whichconn)
{
  mgk_link *lp;
  unsigned int len, idx, done_any;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&whichconn, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_output_port_length,
			  3, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *lenp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*lenp);
      }
    }
    return (mgk_errno = E_COMM);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  if (port >= node->nout) {
    mgk_errno = E_BADPORTIX;
    return (MGK_U_BADVAL);
  }
  for (lp = node->outs[port].links, len = 0, idx = 0, done_any = 0;
       lp;
       lp = lp->l.l2l.nextout, idx++) {
    if ((whichconn == MGK_ALL_OUTPUT_CONNECTIONS) || (whichconn == idx)) {
      switch (lp->type) {
	case LINK_L2L:
	  break;
	case LINK_L2R:
	  len = max(len, lp->l.l2r.qlen);
	  break;
	default:
	  return (mgk_errno = E_GENERROR);
      }
      done_any++;
    }
  }
  mgk_errno = (done_any ? E_SUCCESS : E_PORTCONN);
  return (done_any ? len : MGK_U_BADVAL);
}

DEFSVC(node_output_port_length)
{
  if ((argc == 3) && (rargc == 2)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *selp = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int res = mgk_node_output_port_length(np, *idxp, *selp);
      rargv[0] = __mgk_build_value(&res, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

void *
mgk_peek_node_input_port(mgk_nodep node,
			 unsigned int port,
			 unsigned int streampos,
			 mgk_data_type * typep)
{
  mgk_inport *ip;
  mgk_value *vp;
  last_peek_value = NULL;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&streampos, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_peek_node_input_port,
			  3, args,
			  2, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[1], &t, &s);
      if (t == T_INTEGER) {
	static int rcnt = 0;
	static mgk_scalar sres[16];
	mgk_errno = *errp;
	return (__mgk_extract_value(rargs[0], typep, &sres[++rcnt & 15]));
      }
      __mgk_free_value(rargs[0]);
    }
    mgk_errno = E_COMM;
    *typep = T_NODATA;
    return (NULL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    *typep = T_NODATA;
    return (MGK_P_BADVAL);
  }
  if (port >= node->nin) {
    mgk_errno = E_BADPORTIX;
    *typep = T_NODATA;
    return (MGK_P_BADVAL);
  }
  ip = &node->ins[port];
  mgk_errno = E_SUCCESS;
  for (vp = ip->head; streampos > 0; streampos--, vp = vp->next) {
    if (!vp) {
      *typep = T_NODATA;
      return (NULL);
    }
  }
  last_peek_value = vp;
  return (__mgk_parse_value(vp, typep));
}

DEFSVC(peek_node_input_port)
{
  if ((argc == 3) && (rargc == 2)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *posp = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      void *value = mgk_peek_node_input_port(np, *idxp, *posp, &t1);
      if ((value) &&
	  (last_peek_value) &&
	  (mgk_errno == E_SUCCESS) &&
	  (t1 != T_NODATA)) {
	rargv[0] = __mgk_alias_value(last_peek_value);
      }
      else {
	int dummy = 0;
	rargv[0] = __mgk_build_value(&dummy, T_INTEGER);
	rargv[0]->type = T_NODATA;
      }
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}


void *
mgk_peek_node_output_port(mgk_nodep node,
			  unsigned int port,
			  unsigned int whichconn,
			  unsigned int streampos,
			  mgk_data_type * typep)
{
  mgk_link *lp;
  mgk_value *vp;
  unsigned int idx;
  last_peek_value = NULL;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[4];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&whichconn, T_INTEGER);
    args[3] = __mgk_build_value(&streampos, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_peek_node_output_port,
			  4, args,
			  2, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[1], &t, &s);
      if (t == T_INTEGER) {
	static int rcnt = 0;
	static mgk_scalar sres[16];
	mgk_errno = *errp;
	return (__mgk_extract_value(rargs[0], typep, &sres[++rcnt & 15]));
      }
      __mgk_free_value(rargs[0]);
    }
    mgk_errno = E_COMM;
    *typep = T_NODATA;
    return (NULL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    *typep = T_NODATA;
    return (MGK_P_BADVAL);
  }
  if (port >= node->nout) {
    mgk_errno = E_BADPORTIX;
    *typep = T_NODATA;
    return (MGK_P_BADVAL);
  }
  for (lp = node->outs[port].links, vp = NULL, idx = 0;
       lp;
       lp = lp->l.l2l.nextout, idx++) {
    if (whichconn == idx) {
      switch (lp->type) {
	case LINK_L2L:
	  break;
	case LINK_L2R:
	  vp = lp->l.l2r.head;
	  break;
	default:
	  mgk_errno = E_GENERROR;
	  *typep = T_NODATA;
	  return (NULL);
      }
      break;
    }
  }
  if (!lp) {
    *typep = T_NODATA;
    mgk_errno = E_PORTCONN;
    return (NULL);
  }
  mgk_errno = E_SUCCESS;
  while (vp && (streampos > 0)) {
    vp = vp->next;
    streampos--;
  }
  if (!vp) {
    *typep = T_NODATA;
    return (NULL);
  }
  last_peek_value = vp;
  return (__mgk_parse_value(vp, typep));
}

DEFSVC(peek_node_output_port)
{
  if ((argc == 4) && (rargc == 2)) {
    mgk_data_type t1, t2, t3, t4;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *selp = __mgk_parse_value(argv[2], &t3);
    int *posp = __mgk_parse_value(argv[3], &t4);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) &&
	(t3 == T_INTEGER) && (t4 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      void *value = mgk_peek_node_output_port(np, *idxp, *selp, *posp, &t1);
      if ((value) &&
	  (last_peek_value) &&
	  (mgk_errno == E_SUCCESS) &&
	  (t1 != T_NODATA)) {
	rargv[0] = __mgk_alias_value(last_peek_value);
      }
      else {
	int dummy = 0;
	rargv[0] = __mgk_build_value(&dummy, T_INTEGER);
	rargv[0]->type = T_NODATA;
      }
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}
