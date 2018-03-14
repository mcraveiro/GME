#include <string.h>
#include <mgk60.h>
#include "LPfilter.h"

int FiltReset__System = 0;

lpfilter_script_context LPfilter_actor_0_context;


void register_scripts()
{
   mgk_register_script(lpfilter_script,"lpfilter_script");
}


void build()
{
   mgk_nodep LPfilter_actor_0   = mgk_create_node_indirect( "lpfilter_script" ,  1,  1, 100, AT_IFALL, 0 );

   LPfilter_actor_0_context.Buffer = 0;
   LPfilter_actor_0_context.LastReset = 0;
   LPfilter_actor_0_context.Factor = 0.1;
   LPfilter_actor_0_context.Reset = &FiltReset__System;
   mgk_set_node_context( LPfilter_actor_0, &LPfilter_actor_0_context,T_ARRAY(sizeof(lpfilter_script_context)) | T_CHAR);

   mgk_connect_nodes( LPfilter_actor_0  ,  0,                   ,  0 );
}
