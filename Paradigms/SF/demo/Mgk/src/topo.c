/**
 ** TOPO.C ----- calls to inspect graph topology
 **/

#include <malloc.h>
#include <string.h>

#include "libmgk.h"

unsigned int
mgk_node_input_count(mgk_nodep node)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[1];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    if (__mgk_remote_call(rn->host,
			  cmd_node_input_count,
			  1, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *ninp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*ninp);
      }
    }
    mgk_errno = E_COMM;
    return (MGK_U_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  mgk_errno = E_SUCCESS;
  return (node->nin);
}

unsigned int
mgk_node_output_count(mgk_nodep node)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[1];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    if (__mgk_remote_call(rn->host,
			  cmd_node_output_count,
			  1, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *noutp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*noutp);
      }
    }
    mgk_errno = E_COMM;
    return (MGK_U_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  mgk_errno = E_SUCCESS;
  return (node->nout);
}

DEFSVC(node_input_count)
{
  if ((argc == 1) && (rargc == 2)) {
    mgk_data_type t;
    long *idp = __mgk_parse_value(argv[0], &t);
    if (t == T_LONGINT) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int cnt = mgk_node_input_count(np);
      rargv[0] = __mgk_build_value(&cnt, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

DEFSVC(node_output_count)
{
  if ((argc == 1) && (rargc == 2)) {
    mgk_data_type t;
    long *idp = __mgk_parse_value(argv[0], &t);
    if (t == T_LONGINT) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int cnt = mgk_node_output_count(np);
      rargv[0] = __mgk_build_value(&cnt, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

unsigned int
mgk_node_input_connection_count(mgk_nodep node,
				unsigned int port)
{
  mgk_link *lp;
  unsigned int count;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_input_connection_count,
			  2, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *nump = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*nump);
      }
    }
    mgk_errno = E_COMM;
    return (MGK_U_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  if (port >= node->nin) {
    mgk_errno = E_BADPORTIX;
    return (MGK_U_BADVAL);
  }
  for (lp = node->ins[port].links, count = 0;
       lp;
       lp = lp->l.l2l.nextin, count++);
  mgk_errno = E_SUCCESS;
  return (count);
}

unsigned int
mgk_node_output_connection_count(mgk_nodep node,
				 unsigned int port)
{
  mgk_link *lp;
  unsigned int count;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_output_connection_count,
			  2, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *nump = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*nump);
      }
    }
    mgk_errno = E_COMM;
    return (MGK_U_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (MGK_U_BADVAL);
  }
  if (port >= node->nout) {
    mgk_errno = E_BADPORTIX;
    return (MGK_U_BADVAL);
  }
  for (lp = node->outs[port].links, count = 0;
       lp;
       lp = lp->l.l2l.nextout, count++);
  mgk_errno = E_SUCCESS;
  return (count);
}

DEFSVC(node_input_connection_count)
{
  if ((argc == 2) && (rargc == 2)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int cnt = mgk_node_input_connection_count(np, *idxp);
      rargv[0] = __mgk_build_value(&cnt, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

DEFSVC(node_output_connection_count)
{
  if ((argc == 2) && (rargc == 2)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int cnt = mgk_node_output_connection_count(np, *idxp);
      rargv[0] = __mgk_build_value(&cnt, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_nodep
mgk_node_input_connection(mgk_nodep node, unsigned int port,
			  unsigned int which, unsigned int *connport)
{
  mgk_link *lp;
  unsigned int count;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[4];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&which, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_input_connection,
			  3, args,
			  4, rargs)) {
      mgk_data_type t1, t2, t3, t4;
      mgk_scalar s1, s2, s3, s4;
      long *idp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *hixp = __mgk_extract_value(rargs[1], &t2, &s2);
      int *prtp = __mgk_extract_value(rargs[2], &t3, &s3);
      int *errp = __mgk_extract_value(rargs[3], &t4, &s4);
      if ((t1 == T_LONGINT) && (t2 == T_INTEGER) &&
	  (t3 == T_INTEGER) && (t4 == T_INTEGER)) {
	mgk_errno = *errp;
	*connport = *prtp;
	return (__mgk_locate_node(*hixp, *idp));
      }
    }
    mgk_errno = E_COMM;
    return (MGK_P_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (NULL);
  }
  if (port >= node->nin) {
    mgk_errno = E_BADPORTIX;
    return (NULL);
  }
  mgk_errno = E_SUCCESS;
  for (lp = node->ins[port].links, count = 0;
       lp;
       lp = lp->l.l2l.nextin, count++) {
    if (count == which) {
      *connport = lp->srcidx;
      switch (lp->type) {
	case LINK_L2L:
	  return (lp->l.l2l.src);
	case LINK_R2L:
	  return ((mgk_nodep) lp->l.r2l.src);
	default:
	  mgk_errno = E_GENERROR;
	  return (NULL);
      }
    }
  }
  mgk_errno = E_BADPORTIX;
  return (NULL);
}

mgk_nodep
mgk_node_output_connection(mgk_nodep node, unsigned int port,
			   unsigned int which, unsigned int *connport)
{
  mgk_link *lp;
  unsigned int count;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[4];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&port, T_INTEGER);
    args[2] = __mgk_build_value(&which, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_node_output_connection,
			  3, args,
			  4, rargs)) {
      mgk_data_type t1, t2, t3, t4;
      mgk_scalar s1, s2, s3, s4;
      long *idp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *hixp = __mgk_extract_value(rargs[1], &t2, &s2);
      int *prtp = __mgk_extract_value(rargs[2], &t3, &s3);
      int *errp = __mgk_extract_value(rargs[3], &t4, &s4);
      if ((t1 == T_LONGINT) && (t2 == T_INTEGER) &&
	  (t3 == T_INTEGER) && (t4 == T_INTEGER)) {
	mgk_errno = *errp;
	*connport = *prtp;
	return (__mgk_locate_node(*hixp, *idp));
      }
    }
    mgk_errno = E_COMM;
    return (MGK_P_BADVAL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (NULL);
  }
  if (port >= node->nout) {
    mgk_errno = E_BADPORTIX;
    return (NULL);
  }
  mgk_errno = E_SUCCESS;
  for (lp = node->outs[port].links, count = 0;
       lp;
       lp = lp->l.l2l.nextout, count++) {
    if (count == which) {
      *connport = lp->dstidx;
      switch (lp->type) {
	case LINK_L2L:
	  return (lp->l.l2l.dst);
	case LINK_L2R:
	  return ((mgk_nodep) lp->l.l2r.dst);
	default:
	  mgk_errno = E_GENERROR;
	  return (NULL);
      }
    }
  }
  mgk_errno = E_BADPORTIX;
  return (NULL);
}

DEFSVC(node_input_connection)
{
  if ((argc == 3) && (rargc == 4)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *cntp = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      unsigned int connidx = 0;
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      mgk_node *conn = mgk_node_input_connection(np, *idxp, *cntp, &connidx);
      if (__mgk_check_node(conn)) {
	rargv[0] = __mgk_build_value(&conn->ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      }
      else if (__mgk_check_remnode(conn)) {
	mgk_remnode *rconn = (mgk_remnode *) conn;
	rargv[0] = __mgk_build_value(&rconn->ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&rconn->host->index, T_INTEGER);
      }
      else {
	long ID = 0;
	int hix = -1;
	rargv[0] = __mgk_build_value(&ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&hix, T_INTEGER);
      }
      rargv[2] = __mgk_build_value(&connidx, T_INTEGER);
      rargv[3] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

DEFSVC(node_output_connection)
{
  if ((argc == 3) && (rargc == 4)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *idxp = __mgk_parse_value(argv[1], &t2);
    int *cntp = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER) && (t3 == T_INTEGER)) {
      unsigned int connidx = 0;
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      mgk_node *conn = mgk_node_output_connection(np, *idxp, *cntp, &connidx);
      if (__mgk_check_node(conn)) {
	rargv[0] = __mgk_build_value(&conn->ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&__mgk_local_host->index, T_INTEGER);
      }
      else if (__mgk_check_remnode(conn)) {
	mgk_remnode *rconn = (mgk_remnode *) conn;
	rargv[0] = __mgk_build_value(&rconn->ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&rconn->host->index, T_INTEGER);
      }
      else {
	long ID = 0;
	int hix = -1;
	rargv[0] = __mgk_build_value(&ID, T_LONGINT);
	rargv[1] = __mgk_build_value(&hix, T_INTEGER);
      }
      rargv[2] = __mgk_build_value(&connidx, T_INTEGER);
      rargv[3] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}
