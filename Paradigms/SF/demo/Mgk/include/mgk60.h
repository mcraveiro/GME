/***
 *** MGK60.H
 ***	Multigraph Kernel include file (v 6.0)
 ***	Copyright 1986-1996, Vanderbilt University. All rights reserved.
 ***	This copyright notice is included for precautionary purposes
 ***	and does not constitute an admission that the material to
 ***	which it has been affixed has been made public or otherwise
 ***	disclosed without restriction.
 ***/

#ifndef __MGK60_H_INCLUDED__
#define __MGK60_H_INCLUDED__

/* ===== kernel error codes/error handling ===== */
#ifndef DEFERROR
#define DEFERROR(a,b)		a,
#define LASTERROR		E_NUMERRORS
enum __mgk_error_codes {
#endif

  DEFERROR(E_GENERROR = -1, "general error code")
  DEFERROR(E_SUCCESS = 0, "")
  DEFERROR(E_SCRIPTNOTFOUND, "script not found")
  DEFERROR(E_NOTNODE, "not a node")
  DEFERROR(E_NOTBUFFER, "not a buffer")
  DEFERROR(E_BUFFERUSED, "buffer in use")
  DEFERROR(E_NOTHOST, "not a host descriptor")
  DEFERROR(E_BADPORTIX, "port index outside index range")
  DEFERROR(E_BADTRIGGER, "invalid trigger mode")
  DEFERROR(E_PORTCONN, "selected port is (is not) connected")
  DEFERROR(E_BADDATA, "bad data type propagated")
  DEFERROR(E_NOMEM, "insufficient memory")
  DEFERROR(E_NOTRUNNING, "no node is executing")
  DEFERROR(E_COMM, "communication or protocol error")
  LASTERROR

};

#define MGK_I_BADVAL 		\
	(-1)			/* returned by int functions on error */
#define MGK_U_BADVAL 		\
	((unsigned int)(-1))	/* same for unsigned int fns */
#define MGK_P_BADVAL 		\
	(0)			/* returned by pointer funs on error */

typedef enum __mgk_error_codes mgk_error_code;
extern mgk_error_code mgk_errno;

/* ===== remote connection stuff ===== */

#ifndef __LIBMGK_H_INCLUDED__
typedef void *mgk_hostp;	/* handle to host descriptor */
#endif

typedef struct {		/* host info structure (all chars => no bitrev) */
  char name[100];		/* host name */
  char cpuname[100];		/* architecture */
  char opsys[100];		/* op sys name */
  char revbytes;		/* byte order */
  char ieeefloat;		/* IEEE floating pt format ? */
  char bitsperbyte;		/* how many bits in a (char) */
  char bitsperword;		/* how many bits in a (short) */
  char bitsperlong;		/* how many bits in a (long) */
  char bitsperfloat;		/* how many bits in a (float) */
  char bitsperdouble;		/* how many bits in a (double) */
  char alignment;		/* how to align things */
} mgk_hostinfo;

int  mgk_get_number_of_hosts(void);
int mgk_local_host_index(void);
mgk_hostp mgk_get_host(unsigned int which);
mgk_error_code mgk_get_host_info(mgk_hostp host);
int   mgk_is_a_host(mgk_hostp host);
mgk_hostp mgk_local_host(void);

#define MGK_LOCALHOST		((mgk_hostp)(0))

/* ===== propagated data types + buffer stuff ===== */

typedef unsigned int mgk_data_type, *mgk_data_typep;

#define T_NODATA	(unsigned int)(-1)
#define T_UNKNOWN	0U	/* unspecified (only valid OR-ed to T_BUFFER) */
#define T_CHAR		1U	/* propagated data types: character */
#define T_SHORTINT	2U	/* short integer */
#define T_INTEGER	4U	/* integer */
#define T_LONGINT	5U	/* long integer */
#define T_FLOAT		6U	/* short float */
#define T_DOUBLE	7U	/* double precision */
#define T_BUFFER	32U	/* buffer (OR-able with scalar types) */
#define T_ARRAY(items)	(64U | (items << 8))
				/* array (MUST be OR-ed with a scalar type) */
				/* kernel will convert to buffer upon send */
				/* receiver will see (T_BUFFER | TYPE) */
#define T_STRING	(T_CHAR | T_ARRAY(-1))
				/* see above, but use 'strlen' to get size */

/* ===== buffer allocate/free ===== */
void *  mgk_allocate_buffer(unsigned int size, int do_zero);
void *  mgk_copy_buffer(void *buffer);
mgk_error_code mgk_free_buffer(void *buffer);
int  mgk_is_a_buffer(void *buffer);
unsigned int mgk_buffer_size(void *buffer);

/* ===== data flow graph nodes ===== */

#ifndef _MSC_VER
#include <values.h>
#else
#define BITS(type)		(sizeof(type) * 8)
#endif

#define MGK_MAX_PORT_COUNT	256
#define MGK_MAX_NODE_PRIORITY	255
#define MGK_NODE_STOP_PRIORITY	0

#ifndef __LIBMGK_H_INCLUDED__
typedef void *mgk_nodep;	/* handle to node descriptor */
#endif

typedef void (*mgk_script) (void);
void mgk_register_script(mgk_script fn, char *name);

typedef enum {
  AT_IFALL,
  AT_IFANY,
  AT_SPEC			/* special ==> uses bitmask */
} mgk_trigger_mode;

typedef int mgk_portmask[(MGK_MAX_PORT_COUNT + BITS(int) - 1) / BITS(int)];

#define mgk_portmask_bit(m,i)					\
   (((m)[(i) / BITS(int)] & (1 << ((i) % BITS(int)))) ? 1 : 0)
#define mgk_set_portmask_bit(m,i,v)				\
   ((v) ?							\
    (((m)[(i) / BITS(int)] |= (1 << ((i) % BITS(int)))),1) :	\
    (((m)[(i) / BITS(int)] &= ~(1 << ((i) % BITS(int)))),0))


mgk_nodep
mgk_create_node(mgk_script fn,
		unsigned int nin,
		unsigned int nout,
		unsigned int priority,
		mgk_trigger_mode tmode);
mgk_nodep
mgk_create_node_indirect(char *scriptname,
			 unsigned int nin,
			 unsigned int nout,
			 unsigned int priority,
			 mgk_trigger_mode tmode,
			 mgk_hostp host);

mgk_error_code
mgk_add_node_trigger_mask(mgk_nodep node,
			  mgk_portmask mask);
mgk_error_code
mgk_add_node_trigger_mask_disp(mgk_nodep node,
			       mgk_portmask mask,
			       mgk_script fn);
mgk_error_code
mgk_add_node_trigger_mask_disp_indirect(mgk_nodep node,
					mgk_portmask mask,
					char *scriptname);

mgk_error_code
mgk_set_node_priority(mgk_nodep node,
		      unsigned int priority);
unsigned int
  mgk_node_priority(mgk_nodep node);

mgk_error_code
mgk_set_node_context(mgk_nodep node,
		     void *context,
		     mgk_data_type type);
void *
  mgk_node_context(mgk_nodep node,
		   mgk_data_typep typep);

/* ===== connect/disconnect nodes ===== */

mgk_error_code mgk_connect_nodes(mgk_nodep src, unsigned int srcport,
		  mgk_nodep dst, unsigned int dstport);
mgk_error_code mgk_connect_nodes_len(mgk_nodep src, unsigned int srcport,
		      mgk_nodep dst, unsigned int dstport,
		      unsigned int maxlen);
unsigned int   mgk_set_default_connection_length(unsigned int newlen);
mgk_error_code mgk_disconnect_nodes(mgk_nodep src, unsigned int srcport,
		     mgk_nodep dst, unsigned int dstport);

/* ===== inspect graph topology ===== */

unsigned int   mgk_node_input_count(mgk_nodep node);
unsigned int   mgk_node_output_count(mgk_nodep node);

unsigned int   mgk_node_input_connection_count(mgk_nodep node,
				  unsigned int port);
unsigned int  mgk_node_output_connection_count(mgk_nodep node,
				   unsigned int port);
mgk_nodep mgk_node_input_connection(mgk_nodep node,
			  unsigned int port,
			  unsigned int whichconn,
			  unsigned int *connport);
mgk_nodep mgk_node_output_connection(mgk_nodep node,
			   unsigned int port,
			   unsigned int whichconn,
			   unsigned int *connport);

/* ===== "manual" data insertion/removel/inspect calls ===== */

#define MGK_ALL_OUTPUT_CONNECTIONS	(unsigned int)(-1)

mgk_error_code mgk_write_node_input_port(void *data,
			  mgk_data_type type,
			  mgk_nodep node,
			  unsigned int port);
mgk_error_code mgk_write_node_output_port(void *data,
			   mgk_data_type type,
			   mgk_nodep node,
			   unsigned int port,
			   unsigned int whichconn);

mgk_error_code mgk_clear_node_input_port(mgk_nodep node,
			  unsigned int port,
			  unsigned int count);
mgk_error_code mgk_clear_node_output_port(mgk_nodep node,
			   unsigned int port,
			   unsigned int whichconn,
			   unsigned int count);

unsigned int   mgk_node_input_port_length(mgk_nodep node,
			     unsigned int port);
unsigned int   mgk_node_output_port_length(mgk_nodep node,
			      unsigned int port,
			      unsigned int whichconn);

void *   mgk_peek_node_input_port(mgk_nodep node,
			   unsigned int port,
			   unsigned int streampos,
			   mgk_data_typep typep);

void *  mgk_peek_node_output_port(mgk_nodep node,
			    unsigned int port,
			    unsigned int whichconn,
			    unsigned int streampos,
			    mgk_data_typep typep);

/* ====== node script interface ===== */

mgk_nodep mgk_current_node(void);
void *  mgk_receive(unsigned int port,
	      mgk_data_typep typep);
mgk_error_code mgk_propagate(unsigned int port,
	      void *data,
	      mgk_data_type type);

mgk_error_code mgk_protect_buffer(void *buffer);
mgk_error_code mgk_unprotect_buffer(void *buffer);

void   mgk_abort_node(int code);
unsigned int   mgk_ifany_trigger(void);
mgk_error_code mgk_trigger_mask(mgk_portmask mask);

/* ===== tracing/exception handling ===== */

#define TRC_PROPAG	1	/* debug mask bits: data propagation */
#define TRC_SCHED	2	/* dataflow scheduling */
#define TRC_BUFFER	4	/* buffer allocation/free */
#define TRC_BCHECK	8	/* buffer check (buff. list traversal) */
#define TRC_MALLOC	16	/* any Kernel memory allocation/free */
#define TRC_COMMUN	32	/* remote calls/communications */
#define TRC_LOWCOMM	64	/* low-level communication (msg hex dumps) */
#define TRC_CQUEUE	128	/* communication queues */
#define TRC_USER	256	/* user script trace messages */
#define TRC_MISC	512	/* anything else */
#define TRC_ALL		1023	/* all of the above */
#define TRC_DEFAULT	(TRC_ALL & ~(TRC_BCHECK | TRC_LOWCOMM | TRC_CQUEUE))

typedef void  (*mgk_actor_abort_handler) (mgk_nodep node,			      int code);
typedef void  (*mgk_port_overflow_handler) (mgk_nodep node,
				int port,
				int outflag,
				int outdest,
				int curlen,
				int maxlen);
void  mgk_perror(char *);
void  mgk_set_node_abort_handler(mgk_actor_abort_handler hd);
void  mgk_set_overflow_handler(mgk_port_overflow_handler hd);
void  mgk_set_tracing(int tmode, int stepflag);
void  mgk_enable_warnings(int flag);

/* ===== execution ===== */
mgk_error_code mgk_initialize(int *argc, char ***argv);
unsigned int  mgk_run(unsigned int num_nodes_to_run);
void  mgk_set_comm_priority(unsigned int commpri);
void  mgk_wait_for_comm_event(void);

#endif
