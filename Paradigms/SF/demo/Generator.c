#include <string.h>
#include <mgk60.h>
#include "Generator.h"


datagen_script_context Generator_actor_0_context;


void register_scripts()
{
   mgk_register_script(datagen_script,"datagen_script");
}


void build()
{
   mgk_nodep Generator_actor_0  = mgk_create_node_indirect( "datagen_script"  ,  0,  1,  10, AT_IFALL, 0 );

   mgk_set_node_context( Generator_actor_0, &Generator_actor_0_context,T_ARRAY(sizeof(datagen_script_context)) | T_CHAR);

}
