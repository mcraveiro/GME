#include <stdio.h>
#include "mgk60.h"

#ifndef  DMODE
#define  DMODE 	0
#endif

#ifndef  BSIZE
#define  BSIZE	20
#endif

static void
sadd(void)
{
  void *in1, *in2;
  mgk_data_type t1, t2;
  printf("sadd on %d\n", mgk_local_host_index());
  fflush(stdout);
  in1 = mgk_receive(0, &t1);
  in2 = mgk_receive(1, &t2);
  if (in1 && in2 && (t1 == T_DOUBLE) && (t2 == T_DOUBLE)) {
    double res = *((double *)(in1)) + *((double *)(in2));
    mgk_propagate(0, &res, T_DOUBLE);
  }
}

static void
ssub(void)
{
  void *in1, *in2;
  mgk_data_type t1, t2;
  printf("ssub on %d\n", mgk_local_host_index());
  fflush(stdout);
  in1 = mgk_receive(0, &t1);
  in2 = mgk_receive(1, &t2);
  if (in1 && in2 && (t1 == T_DOUBLE) && (t2 == T_DOUBLE)) {
    double res = *((double *)(in1)) - *((double *)(in2));
    mgk_propagate(0, &res, T_DOUBLE);
  }
}

static void 
bgen(void)
{
  mgk_data_type t;
  double *bp;
  mgk_receive(0, &t);
  if ((bp = mgk_allocate_buffer((sizeof(double) * BSIZE), 0))) {
    int i;
    for (i = 0; i < BSIZE; i++) {
      bp[i] = (i == 0) ? (double)BSIZE : (double)i;
    }
    mgk_propagate(0, bp, (T_DOUBLE | T_BUFFER));
  }
}

static void 
bprint(void)
{
  mgk_data_type t;
  double *bp = mgk_receive(0, &t);
  if (bp && (t == (T_BUFFER | T_DOUBLE))) {
    int i, size = mgk_buffer_size(bp) / sizeof(double);
    for (i = 0; i < size; i++) {
      printf("%04d %12.4f\n",i,bp[i]);
    }
    mgk_propagate(0, bp, T_DOUBLE);
  }
}

int
main(int argc, char **argv)
{
  if (mgk_initialize(&argc, &argv) == E_SUCCESS) {
    mgk_register_script(sadd, "sadd");
    mgk_register_script(ssub, "ssub");
    mgk_register_script(bgen, "bgen");
    mgk_register_script(bprint, "bprinter");

    if (mgk_local_host_index() == 0) {
      mgk_nodep nde1, nde2, nde3, dum1;
      double data1, data2, data3, data4, *dp;
      mgk_data_type tp;
      mgk_hostp h1, h2, h3;
      int i;
      printf("number of hosts: %d\n", mgk_get_number_of_hosts());
      printf("local host index is %d\n", mgk_local_host_index());
      printf("local host is %p\n", mgk_local_host());
      for (i = 0; i < mgk_get_number_of_hosts(); i++) {
	printf("host %d is %p\n", i, mgk_get_host(i));
      }

      h1 = mgk_local_host();
      h2 = mgk_get_host(1);
      h3 = mgk_get_host(2);

      dum1 = mgk_create_node(sadd, 1, 1, MGK_NODE_STOP_PRIORITY, AT_IFALL);
      if (DMODE == 0) {
	nde1 = mgk_create_node_indirect("sadd", 2, 1, 100, AT_IFALL, h1);
	nde2 = mgk_create_node_indirect("ssub", 2, 1, 100, AT_IFALL, h2);
	nde3 = mgk_create_node_indirect("sadd", 2, 1, 100, AT_IFALL, h3);

	mgk_connect_nodes(nde1, 0, nde3, 0);
	mgk_connect_nodes(nde2, 0, nde3, 1);
	mgk_connect_nodes(nde3, 0, dum1, 0);
	data1 = 5.0;
	data2 = 2.0;
	data3 = 3.0;
	data4 = 7.0;
	if (argc > 1)
	  sscanf(argv[1], "%le", &data1);
	if (argc > 2)
	  sscanf(argv[2], "%le", &data2);
	if (argc > 3)
	  sscanf(argv[3], "%le", &data3);
	if (argc > 4)
	  sscanf(argv[4], "%le", &data4);
	mgk_write_node_input_port(&data1, T_DOUBLE, nde1, 0);
	mgk_write_node_input_port(&data2, T_DOUBLE, nde1, 1);
	mgk_write_node_input_port(&data3, T_DOUBLE, nde2, 0);
	mgk_write_node_input_port(&data4, T_DOUBLE, nde2, 1);
      }
      else {
	if (DMODE == 1) {
	  nde1 = mgk_create_node_indirect("bgen", 1, 1, 100, AT_IFALL, h1);
	  nde2 = mgk_create_node_indirect("bprinter", 1, 1, 100, AT_IFALL, h2);
	}
	else {
	  nde1 = mgk_create_node_indirect("bgen", 1, 1, 100, AT_IFALL, h2);
	  nde2 = mgk_create_node_indirect("bprinter", 1, 1, 100, AT_IFALL, h1);
	}
	mgk_connect_nodes(nde1, 0, nde2, 0);
	mgk_connect_nodes(nde2, 0, dum1, 0);
	data1 = 0.0;
	mgk_write_node_input_port(&data1, T_DOUBLE, nde1, 0);
      }
      if (mgk_get_number_of_hosts() == 1) {
	while (mgk_run(1));
	dp = mgk_peek_node_input_port(dum1, 0, 0, &tp);
      }
      else {
	for (;;) {
	  if (!mgk_run(1)) {
	    mgk_wait_for_comm_event();
	  }
	  dp = mgk_peek_node_input_port(dum1, 0, 0, &tp);
	  if (dp) {
	    break;
	  }
	}
      }
      if (dp && (tp == T_DOUBLE)) {
	printf("The result is %f\n", *dp);
      }
      return (0);
    }
    else {
      char name[100];
      sprintf(name, "stdout.%d", mgk_local_host_index());
      freopen(name, "w", stdout);
      sprintf(name, "stderr.%d", mgk_local_host_index());
      freopen(name, "w", stderr);
      for (;;) {
	if (!mgk_run(1000)) {
	  mgk_wait_for_comm_event();
	}
      }
    }
  }
  return (-1);
}
