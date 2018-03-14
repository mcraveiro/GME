/**
 ** BUILD.C ----- dataflow graph builder functions
 **/

#include <malloc.h>
#include <string.h>

#include "libmgk.h"
#include "allocate.h"

DEFINE_ALLOCATOR(mgk_mask_trigger, 50, next);
DEFINE_ALLOCATOR(mgk_remnode, 50, next);

mgk_node **__mgk_local_nodes = NULL;
int __mgk_num_local_nodes = 0;
static int max_table_size = 0;
static mgk_remnode *stub_list = NULL;

static int
grow_table(void)
{
  static int table_alloc = 250;
  static int table_alloc_inc = 256;
  mgk_node **ntab = malloc(sizeof(mgk_node *) * table_alloc);
  if (ntab) {
    if (__mgk_num_local_nodes > 0) {
      memcpy(ntab,
	     __mgk_local_nodes,
	     (sizeof(mgk_node *) * __mgk_num_local_nodes));
    }
    if (__mgk_local_nodes) {
      free(__mgk_local_nodes);
    }
    __mgk_local_nodes = ntab;
    max_table_size = table_alloc;
    table_alloc += table_alloc_inc;
    table_alloc_inc *= 2;
    return (TRUE);
  }
  return (FALSE);
}

mgk_nodep
mgk_create_node(mgk_script fn, unsigned int nin, unsigned int nout,
		unsigned int priority,
		mgk_trigger_mode tmode)
{
  mgk_node *node;
  char *memptr;
  int i, crcbyte;
  long crc;

  if (fn == NULL) {
    mgk_errno = E_SCRIPTNOTFOUND;
    return (NULL);
  }
  if ((nin >= MGK_MAX_PORT_COUNT) || (nout >= MGK_MAX_PORT_COUNT)) {
    mgk_errno = E_BADPORTIX;
    return (NULL);
  }
  if ((tmode != AT_IFALL) && (tmode != AT_IFANY) && (tmode != AT_SPEC)) {
    mgk_errno = E_BADTRIGGER;
    return (NULL);
  }
  if (__mgk_num_local_nodes == max_table_size) {
    if (!grow_table()) {
      mgk_errno = E_NOMEM;
      return (NULL);
    }
  }
  node = malloc((sizeof(mgk_node)) +
		(sizeof(mgk_inport) * nin) +
		(sizeof(mgk_outport) * nout));
  if (!node) {
    mgk_errno = E_NOMEM;
    return (NULL);
  }
  memptr = (char *)node;
  memset(node, 0, sizeof(mgk_node));
  node->tag = OBJ_NODE;
  node->script = fn;
  node->ins = (mgk_inport *) (memptr += sizeof(mgk_node));
  node->outs = (mgk_outport *) (memptr += (sizeof(mgk_inport) * nin));
  node->nin = nin;
  node->nout = nout;
  if (nin > 0) {
    int i;
    memset(node->ins, 0, (sizeof(mgk_inport) * nin));
    for(i = 0; i < (int)nin; i++) {
      mgk_set_portmask_bit(node->input_mask,i,1);
    }
  }
  if (nout > 0) {
    memset(node->outs, 0, (sizeof(mgk_outport) * nout));
  }
  node->priority = max(min(priority, MGK_MAX_NODE_PRIORITY),
		       MGK_NODE_STOP_PRIORITY);
  node->tmode = tmode;
  __mgk_update(node);
  crc = (long)node ^ (long)node->script ^
    node->nin ^ node->nout ^
    __mgk_num_local_nodes;
  for (memptr = (char *)(&crc), i = crcbyte = 0; i < sizeof(crc); i++) {
    crcbyte ^= memptr[i];
  }
  node->ID = __mgk_num_local_nodes | ((long)crcbyte << 24);
  __mgk_local_nodes[__mgk_num_local_nodes++] = node;
  return (node);
}

mgk_nodep
mgk_create_node_indirect(char *scriptname,
			 unsigned int nin, unsigned int nout,
			 unsigned int priority,
			 mgk_trigger_mode tmode, mgk_hostp host)
{
  if ((host == MGK_LOCALHOST) || (host == __mgk_local_host)) {
    return (mgk_create_node(__mgk_lookup_script(scriptname),
			    nin, nout, priority, tmode));
  }
  if (__mgk_check_host(host)) {
    mgk_value *args[5], *rargs[2];
    args[0] = __mgk_build_value(scriptname, T_STRING);
    args[1] = __mgk_build_value(&nin, T_INTEGER);
    args[2] = __mgk_build_value(&nout, T_INTEGER);
    args[3] = __mgk_build_value(&priority, T_INTEGER);
    args[4] = __mgk_build_value(&tmode, T_INTEGER);
    if (__mgk_remote_call(host,
			  cmd_create_node_indirect,
			  5, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      long *idp = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_LONGINT) &&
	  (t2 == T_INTEGER) &&
	  ((mgk_errno = *errp) == E_SUCCESS)) {
	return ((mgk_node *) __mgk_remnode_stub(host, *idp));
      }
      return (NULL);
    }
    mgk_errno = E_COMM;
    return (NULL);
  }
  mgk_errno = E_NOTHOST;
  return (NULL);
}

DEFSVC(create_node_indirect)
{
  if ((argc == 5) && (rargc == 2)) {
    mgk_data_type t1, t2, t3, t4, t5;
    char *sname = __mgk_parse_value(argv[0], &t1);
    int *ninp = __mgk_parse_value(argv[1], &t2);
    int *noutp = __mgk_parse_value(argv[2], &t3);
    int *priop = __mgk_parse_value(argv[3], &t4);
    int *tmodep = __mgk_parse_value(argv[4], &t5);
    if ((t1 == (T_CHAR | T_BUFFER)) &&
	(t2 == T_INTEGER) &&
	(t3 == T_INTEGER) &&
	(t4 == T_INTEGER) &&
	(t5 == T_INTEGER)) {
      mgk_node *res = mgk_create_node_indirect(sname,
					       *ninp, *noutp, *priop,
					       (mgk_trigger_mode) (*tmodep),
					       MGK_LOCALHOST);
      long ID = res ? res->ID : 0;
      rargv[0] = __mgk_build_value(&ID, T_LONGINT);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_add_node_trigger_mask_disp(mgk_nodep node,
			       mgk_portmask mask,
			       mgk_script fn)
{
  mgk_mask_trigger *tm;
  if (!__mgk_check_node(node)) {
    return (E_NOTNODE);
  }
  if (!fn) {
    return (E_SCRIPTNOTFOUND);
  }
  if (node->tmode != AT_SPEC) {
    return (E_BADTRIGGER);
  }
  if (!(tm = ALLOC_mgk_mask_trigger())) {
    return (E_NOMEM);
  }
  memcpy(tm->mask, mask, sizeof(mgk_portmask));
  tm->script = fn;
  tm->next = node->trigger_list;
  node->trigger_list = tm;
  __mgk_update(node);
  return (E_SUCCESS);
}

mgk_error_code
mgk_add_node_trigger_mask(mgk_nodep node, mgk_portmask mask)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[1];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(mask, (T_INTEGER | T_ARRAY(itemsof(mask))));
    if (__mgk_remote_call(rn->host,
			  cmd_add_node_trigger_mask,
			  2, args,
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
    return (E_NOTNODE);
  }
  return (mgk_add_node_trigger_mask_disp(node, mask, node->script));
}

DEFSVC(add_node_trigger_mask)
{
  if ((argc == 2) && (rargc == 1)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *maskp = __mgk_parse_value(argv[1], &t2);
    if ((t1 == T_LONGINT) &&
	(t2 == (T_INTEGER | T_BUFFER)) &&
	(mgk_buffer_size(maskp) == sizeof(mgk_portmask))) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      mgk_add_node_trigger_mask(np, maskp);
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_add_node_trigger_mask_disp_indirect(mgk_nodep node,
					mgk_portmask mask,
					char *scriptname)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[3];
    mgk_value *rargs[1];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(mask, (T_INTEGER | T_ARRAY(itemsof(mask))));
    args[2] = __mgk_build_value(scriptname, T_STRING);
    if (__mgk_remote_call(rn->host,
			  cmd_add_node_trigger_mask_disp_indirect,
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
  return (mgk_add_node_trigger_mask_disp(node, mask,
					 __mgk_lookup_script(scriptname)));
}

DEFSVC(add_node_trigger_mask_disp_indirect)
{
  if ((argc == 3) && (rargc == 1)) {
    mgk_data_type t1, t2, t3;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *maskp = __mgk_parse_value(argv[1], &t2);
    char *sc = __mgk_parse_value(argv[2], &t3);
    if ((t1 == T_LONGINT) &&
	(t2 == (T_INTEGER | T_BUFFER)) &&
	(t3 == (T_CHAR | T_BUFFER)) &&
	(mgk_buffer_size(maskp) == sizeof(mgk_portmask))) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      mgk_add_node_trigger_mask_disp_indirect(np, maskp, sc);
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_set_node_priority(mgk_nodep node, unsigned int priority)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[1];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = __mgk_build_value(&priority, T_INTEGER);
    if (__mgk_remote_call(rn->host,
			  cmd_set_node_priority,
			  2, args,
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
    return (E_NOTNODE);
  }
  priority = max(min(priority, MGK_MAX_NODE_PRIORITY), MGK_NODE_STOP_PRIORITY);
  if (node->priority != priority) {
    if (node->status == READY) {
      __mgk_dequeue(node);
    }
    node->priority = priority;
    if (node->status == READY) {
      __mgk_enqueue(node);
    }
  }
  return (E_SUCCESS);
}

DEFSVC(set_node_priority)
{
  if ((argc == 2) && (rargc == 1)) {
    mgk_data_type t1, t2;
    long *idp = __mgk_parse_value(argv[0], &t1);
    int *prip = __mgk_parse_value(argv[1], &t2);
    if ((t1 == T_LONGINT) && (t2 == T_INTEGER)) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      mgk_set_node_priority(np, *prip);
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

unsigned int
mgk_node_priority(mgk_nodep node)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[1];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    if (__mgk_remote_call(rn->host,
			  cmd_node_priority,
			  1, args,
			  2, rargs)) {
      mgk_data_type t1, t2;
      mgk_scalar s1, s2;
      int *prip = __mgk_extract_value(rargs[0], &t1, &s1);
      int *errp = __mgk_extract_value(rargs[1], &t2, &s2);
      if ((t1 == T_INTEGER) && (t2 == T_INTEGER)) {
	mgk_errno = *errp;
	return (*prip);
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
  return (node->priority);
}

DEFSVC(node_priority)
{
  if ((argc == 1) && (rargc == 2)) {
    mgk_data_type t;
    long *idp = __mgk_parse_value(argv[0], &t);
    if (t == T_LONGINT) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      int pri = mgk_node_priority(np);
      rargv[0] = __mgk_build_value(&pri, T_INTEGER);
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_error_code
mgk_set_node_context(mgk_nodep node,
		     void *context, mgk_data_type type)
{
  mgk_value *val;
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[2];
    mgk_value *rargs[1];
    if ((context == NULL) || (type == T_NODATA)) {
      int dummy = 0;
      val = __mgk_build_value(&dummy, T_INTEGER);
      val->type = T_NODATA;
    }
    else if (!(val = __mgk_build_value(context, type))) {
      return (E_BADDATA);
    }
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    args[1] = val;
    if (__mgk_remote_call(rn->host,
			  cmd_set_node_context,
			  2, args,
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
    return (E_NOTNODE);
  }
  if ((context == NULL) || (type == T_NODATA)) {
    val = NULL;
  }
  else if (!(val = __mgk_build_value(context, type))) {
    return (E_BADDATA);
  }
  if (node->context) {
    __mgk_free_value(node->context);
  }
  node->context = val;
  return (E_SUCCESS);
}

DEFSVC(set_node_context)
{
  if ((argc == 2) && (rargc == 1)) {
    mgk_data_type t;
    long *idp = __mgk_parse_value(argv[0], &t);
    if (t == T_LONGINT) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      if (np) {
	if (np->context) {
	  __mgk_free_value(np->context);
	}
	if (argv[1]->type == T_NODATA) {
	  np->context = NULL;
	}
	else {
	  np->context = argv[1];
	  argv[1] = NULL;
	}
	mgk_errno = E_SUCCESS;
      }
      else {
	mgk_errno = E_NOTNODE;
      }
      rargv[0] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

void *
mgk_node_context(mgk_nodep node, mgk_data_type * typep)
{
  if (__mgk_check_remnode(node)) {
    mgk_remnode *rn = (mgk_remnode *) node;
    mgk_value *args[1];
    mgk_value *rargs[2];
    args[0] = __mgk_build_value(&rn->ID, T_LONGINT);
    if (__mgk_remote_call(rn->host,
			  cmd_node_priority,
			  1, args,
			  2, rargs)) {
      mgk_data_type t;
      mgk_scalar s;
      int *errp = __mgk_extract_value(rargs[1], &t, &s);
      if (t == T_INTEGER) {
	static mgk_scalar cxts[16];
	static int ncxt = 0;
	mgk_errno = *errp;
	return (__mgk_extract_value(rargs[0], typep, &cxts[++ncxt & 15]));
      }
      __mgk_free_value(rargs[0]);
    }
    mgk_errno = E_COMM;
    return (NULL);
  }
  if (!__mgk_check_node(node)) {
    mgk_errno = E_NOTNODE;
    return (NULL);
  }
  mgk_errno = E_SUCCESS;
  return (node->context ?
	  __mgk_parse_value(node->context, typep) :
	  (void*) ((*typep = T_NODATA), NULL));
}

DEFSVC(node_context)
{
  if ((argc == 1) && (rargc == 2)) {
    mgk_data_type t;
    long *idp = __mgk_parse_value(argv[0], &t);
    if (t == T_LONGINT) {
      mgk_node *np = __mgk_retrieve_node_by_ID(*idp);
      if (np && np->context) {
	rargv[0] = __mgk_alias_value(np->context);
      }
      else {
	int dummy = 0;
	rargv[0] = __mgk_build_value(&dummy, T_INTEGER);
	rargv[0]->type = T_NODATA;
      }
      mgk_errno = np ? E_SUCCESS : E_NOTNODE;
      rargv[1] = __mgk_build_value(&mgk_errno, T_INTEGER);
      return (TRUE);
    }
  }
  return (FALSE);
}

mgk_node *
__mgk_retrieve_node_by_ID(unsigned long ID)
{
  mgk_node *np;
  int idx;
  return (((__mgk_local_nodes) &&
	   ((idx = (int)(ID & 0x0ffffffL)) < __mgk_num_local_nodes) &&
	   ((np = __mgk_local_nodes[idx])) &&
	   (__mgk_check_node(np)) &&
	   (np->ID == ID)) ?
	  np :
	  NULL);
}

mgk_remnode *
__mgk_remnode_stub(mgk_host * h, unsigned long ID)
{
  mgk_remnode *rp;
  for (rp = stub_list; rp; rp = rp->next) {
    if ((rp->host == h) && (rp->ID == ID)) {
      return (rp);
    }
  }
  rp = ALLOC_mgk_remnode();
  if (!rp) {
    mgk_errno = E_NOMEM;
    return (NULL);
  }
  rp->tag = OBJ_REMNODE;
  rp->host = h;
  rp->ID = ID;
  rp->next = stub_list;
  stub_list = rp;
  return (rp);
}

mgk_node *
__mgk_locate_node(unsigned int hostidx, unsigned long ID)
{
  if (hostidx == (unsigned int)__mgk_local_host->index) {
    return (__mgk_retrieve_node_by_ID(ID));
  }
  if (hostidx < (unsigned int)__mgk_num_hosts) {
    return ((mgk_node *) __mgk_remnode_stub(&__mgk_host_table[hostidx], ID));
  }
  return (NULL);
}
