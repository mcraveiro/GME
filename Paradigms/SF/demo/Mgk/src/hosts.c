/**
 ** HOSTS.C ----- host related calls
 **/

#include "libmgk.h"

int
mgk_get_number_of_hosts(void)
{
  return (__mgk_num_hosts);
}

int
mgk_local_host_index(void)
{
  int i;
  for(i = 0; i < __mgk_num_hosts; i++) {
    if(&__mgk_host_table[i] == __mgk_local_host) {
      return(i);
    }
  }
  return(-1);
}

mgk_hostp
mgk_get_host(unsigned int which)
{
  if (which < (unsigned int)__mgk_num_hosts) {
    return (&__mgk_host_table[which]);
  }
  else {
    return (NULL);
  }
}

int
mgk_is_a_host(mgk_hostp host)
{
  return (__mgk_check_host(host) ? TRUE : FALSE);
}

mgk_hostp
mgk_local_host(void)
{
  return (__mgk_local_host);
}
