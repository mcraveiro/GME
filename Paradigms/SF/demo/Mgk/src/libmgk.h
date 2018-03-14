/***
 *** LIBMGK.H
 ***	Multigraph Kernel internal include file (v 6.0)
 ***	Copyright 1986-1996, Vanderbilt University. All rights reserved.
 ***	This copyright notice is included for precautionary purposes
 ***	and does not constitute an admission that the material to
 ***	which it has been affixed has been made public or otherwise
 ***	disclosed without restriction.
 ***/

#ifndef __LIBMGK_H_INCLUDED__
#define __LIBMGK_H_INCLUDED__

#include <setjmp.h>

/* ===== common macros ===== */

#ifndef NULL
#define NULL		0
#endif
#ifndef TRUE
#define TRUE		1
#endif
#ifndef FALSE
#define FALSE		0
#endif

#ifndef min
#define min(x,y)	(((x) < (y)) ? (x) : (y))
#endif
#ifndef max
#define max(x,y)	(((x) > (y)) ? (x) : (y))
#endif
#ifndef itemsof
#define itemsof(a)	(sizeof(a) / sizeof(a[0]))
#endif

/* ===== forward decls for internal types, then kernel API prototypes */

typedef struct __mgk_host mgk_host, *mgk_hostp;
typedef struct __mgk_node mgk_node, *mgk_nodep;
typedef struct __mgk_remnode mgk_remnode, *mgk_remnodep;
typedef struct __mgk_buffer mgk_buffer, *mgk_bufferp;
typedef struct __mgk_value mgk_value, *mgk_valuep;
typedef struct __mgk_port mgk_port, *mgk_portp;
typedef struct __mgk_inport mgk_inport, *mgk_inportp;
typedef struct __mgk_outport mgk_outport, *mgk_outportp;
typedef struct __mgk_link mgk_link, *mgk_linkp;
typedef struct __mgk_mask_trigger mgk_mask_trigger, *mgk_mask_triggerp;
typedef struct __mgk_script_def mgk_script_def, *mgk_script_defp;
typedef struct __mgk_command_packet mgk_command_packet, *mgk_command_packetp;
typedef struct __mgk_data_packet mgk_data_packet, *mgk_data_packetp;
typedef struct __mgk_buffer_xfer mgk_buffer_xfer, *mgk_buffer_xferp;
typedef struct __mgk_type_info mgk_type_info, *mgk_type_infop;

#include "mgk60.h"

/* ===== kernel data structures ===== */

typedef unsigned long type_tag;
#define OBJ_INVALID 0
#define OBJ_HOST    0x45678915
#define OBJ_NODE    0x98759717
#define OBJ_BUFFER  0x75801253
#define OBJ_REMNODE 0x98548775


struct __mgk_buffer {
  type_tag tag;
  unsigned int size;
  void *refval;
  mgk_buffer *next;
};

typedef union {
  char C;
  short S;
  int I;
  long L;
  float F;
  double D;
} mgk_scalar;

struct __mgk_value {
  mgk_data_type type;
  void *value;
  mgk_value *next;
  mgk_scalar scalar_val;
  unsigned long create_time;
};

struct __mgk_inport {
  mgk_value *head;
  mgk_value *tail;
  mgk_link *links;
  unsigned int qlen;
  unsigned int maxlen;
  mgk_scalar scalar_val;
};

struct __mgk_outport {
  mgk_link *links;
};

struct __mgk_link {
  enum {
    LINK_L2L, LINK_L2R, LINK_R2L
  } type;
  unsigned int srcidx;
  unsigned int dstidx;
  union {
    struct {
      mgk_link *nextin;		/* careful!, next pointers should */
      mgk_link *nextout;	/* be the same in each version */
      mgk_node *src;
      mgk_node *dst;
    } l2l;
    struct {
      mgk_node *src;
      mgk_link *nextout;
      mgk_remnode *dst;
      unsigned int qlen;	/* data queue waiting to be shipped */
      unsigned int maxlen;
      mgk_value *head;
      mgk_value *tail;
      mgk_data_packet *active;	/* packet being sent */
    } l2r;
    struct {
      mgk_link *nextin;
      mgk_remnode *src;
      mgk_node *dst;
    } r2l;
  } l;
};

struct __mgk_mask_trigger {
  mgk_portmask mask;
  mgk_script script;
  mgk_mask_trigger *next;
};

typedef enum {
  WAITING,
  READY,
  RUNNING
} node_status;

struct __mgk_node {
  type_tag tag;
  mgk_script script;
  mgk_value *context;
  mgk_inport *ins;
  mgk_outport *outs;
  unsigned int nin;
  unsigned int nout;
  unsigned int priority;
  unsigned int last_ifany_port;
  unsigned long ID;
  mgk_portmask input_mask;
  mgk_portmask data_mask;
  mgk_trigger_mode tmode;
  mgk_mask_trigger *trigger_list;
  mgk_mask_trigger *active_trigger;
  unsigned long ready_time;
  mgk_buffer *buffer_list;
  node_status status;
  mgk_node *next;
};

/* ===== data structures for distributed version ===== */

struct __mgk_remnode {
  type_tag tag;
  mgk_host *host;
  unsigned long ID;
  mgk_remnode *next;
};

#ifdef DEFSVC
#undef DEFSVC
#endif
#define DEFSVC(name)	cmd_##name,

typedef enum {
#include "remcmd.h"
  REMOTE_COMMAND_COUNT
} mgk_remcmd;

#ifdef DEFSVC
#undef DEFSVC
#endif
#define DEFSVC(name)	int __mgk_svc_##name(int argc,mgk_value **argv,	    \
					     int rargc,mgk_value **rargv);
#include "remcmd.h"

#ifdef DEFSVC
#undef DEFSVC
#endif
#define DEFSVC(name)	int __mgk_svc_##name(int argc,mgk_value **argv,	    \
                                             int rargc,mgk_value **rargv)

#define MAX_REMCMD_ARGS		8
#define MAX_REMCMD_REPLIES	4

struct __mgk_data_packet {
  unsigned long serial_no;
  unsigned long node_ID;
  unsigned int port;
  mgk_value *data;
  mgk_link *backlink;
  mgk_data_packet *next;
};

struct __mgk_command_packet {
  unsigned long serial_no;
  mgk_remcmd command_id;
  int argc;
  int rargc;
  mgk_value *argv[MAX_REMCMD_ARGS];
  mgk_value *rargv[MAX_REMCMD_REPLIES];
  int num_incomplete;
  int done;
  int success;
  mgk_command_packet *next;
};

struct __mgk_buffer_xfer {
  unsigned long serial_no;
  mgk_value *data;
  void *buffer;
  unsigned int items_left;
  unsigned int xfer_pos;
  void *packet;
  int argidx;
  mgk_type_info *type;
  mgk_buffer_xfer *next;
};

struct __mgk_type_info {
  mgk_data_type type_code;
  unsigned int native_size;
  unsigned int packed_size;
  unsigned int packed_size2;
  unsigned int packed_size4;
  unsigned int max_cmd_pkt_pack_count;
  unsigned int max_data_pkt_pack_count;
  unsigned int buf_xfer_count;
  void (*pack) (void *data, unsigned int count);
  void (*unpk) (void *data, unsigned int count);
  void (*send) (void *data, unsigned int count, mgk_host * hp);
  void (*recv) (void *data, unsigned int count, mgk_host * hp);
};

struct __mgk_host {
  type_tag tag;
  int index;
  unsigned long sent_msg_count;
  unsigned long acq_msg_count;
  unsigned long last_recvd_msg;
  unsigned long cmd_send_count;
  unsigned long data_send_count;
  unsigned long buffer_send_count;
  mgk_command_packet *queued_cmds;			/* caller */
  mgk_command_packet *last_queued_cmd;			/* caller */
  mgk_command_packet *send_incomplete_cmds;		/* caller */
  mgk_command_packet *sent_cmds;			/* caller */
  mgk_command_packet *reply_incomplete_cmds;		/* caller */
  mgk_command_packet *received_incomplete_cmds;		/* callee */
  mgk_command_packet *executed_cmds;			/* callee */
  mgk_data_packet *queued_data;				/* caller */
  mgk_data_packet *received_incomplete_data;		/* callee */
  mgk_buffer_xfer *queued_to_send_buffers;		/* caller */
  mgk_buffer_xfer *send_buffers;			/* caller */
  mgk_buffer_xfer *queued_to_recv_buffers;		/* callee */
  mgk_buffer_xfer *receive_buffers;			/* callee */
  int error;
  int sent_keepalive;
};

/* ===== globals ===== */

extern mgk_node *__mgk_running_node;
extern jmp_buf __mgk_node_abort_context;
extern mgk_node **__mgk_local_nodes;
extern int __mgk_num_local_nodes;
extern mgk_host *__mgk_host_table;
extern mgk_host *__mgk_local_host;
extern int __mgk_num_hosts;
extern unsigned long __mgk_tick;

/* ===== utilities ===== */

#define __mgk_check_buffer(b) (					\
  ((b) != NULL) && 						\
  (((long)(b) & (sizeof(long) - 1)) == 0) &&			\
  (((mgk_bufferp)(b))[-1].tag == OBJ_BUFFER)			\
)

#define __mgk_check_host(h) (					\
  ((h) != NULL) && (((mgk_host *)(h))->tag == OBJ_HOST)		\
)

#define __mgk_check_node(n) (					\
  ((n) != NULL) && (((mgk_node *)(n))->tag == OBJ_NODE)		\
)

#define __mgk_check_remnode(n) (				\
  ((n) != NULL) && (((mgk_node *)(n))->tag == OBJ_REMNODE)	\
)

#define __mgk_buffer_header(b)					\
  (((mgk_bufferp)(b)) - 1)

#define __mgk_checked_buffer_header(b) (			\
  __mgk_check_buffer(b) ?					\
  __mgk_buffer_header(b) :					\
  ((mgk_bufferp)(0))						\
)

void *__mgk_allocate_buffer(unsigned int size);
void *__mgk_copy_buffer(void *buffer);
mgk_error_code __mgk_free_buffer(void *buffer);

#define __mgk_free_buffer_header(bh)	__mgk_free_buffer((bh) + 1)

mgk_script __mgk_lookup_script(char *name);
char *__mgk_script_name(mgk_script fn);

void __mgk_enqueue(mgk_node * node);
void __mgk_dequeue(mgk_node * node);
mgk_node *__mgk_next_node(void);
void __mgk_execute_node(mgk_node * np);

void __mgk_update(mgk_node * node);

void __mgk_free_value(mgk_value * val);
void __mgk_free_value_list(mgk_value * val);

mgk_value *__mgk_build_value(void *data, mgk_data_type type);
mgk_value *__mgk_copy_value(mgk_value * vp);
mgk_value *__mgk_alias_value(mgk_value * vp);
mgk_value *__mgk_alias_value_autofree(mgk_value * vp, int *refcnt);
void *__mgk_parse_value(mgk_value * value, mgk_data_type * typep);
void *__mgk_extract_value(mgk_value * value, mgk_data_type * tp, mgk_scalar * sp);

mgk_remnode *__mgk_remnode_stub(mgk_host * h, unsigned long ID);
mgk_node *__mgk_retrieve_node_by_ID(unsigned long ID);
mgk_node *__mgk_locate_node(unsigned int hostidx, unsigned long ID);

int __mgk_remote_call(mgk_host * host, mgk_remcmd cmd,
		      int argc, mgk_value ** argv,
		      int rargc, mgk_value ** rargv);
void __mgk_remote_propagate(mgk_link * lk);

#endif /* whole file */
