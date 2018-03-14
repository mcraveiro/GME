

/**
 ** SCRIPT.C ----- indirect script registry
 **/

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "libmgk.h"

typedef struct __sdef {
  struct __sdef *next;
  mgk_script fn;
  char name[1];
} sdef;

static sdef *slist = NULL;

void
mgk_register_script(mgk_script fn, char *name)
{
  sdef *sp;
  for (sp = slist; sp; sp = sp->next) {
    if (strcmp(sp->name, name) == 0) {
      sp->fn = fn;
      return;
    }
  }
  sp = malloc(sizeof(sdef) + strlen(name) + 1);
  if (!sp) {
    return;
  }
  sp->next = slist;
  slist = sp;
  sp->fn = fn;
  strcpy(sp->name, name);
}

mgk_script
__mgk_lookup_script(char *name)
{
  sdef *sp;
  for (sp = slist; sp; sp = sp->next) {
    if (strcmp(sp->name, name) == 0) {
      return (sp->fn);
    }
  }
  return (NULL);
}

char *
__mgk_script_name(mgk_script fn)
{
  static char name[100];
  sdef *sp;
  for (sp = slist; sp; sp = sp->next) {
    if (sp->fn == fn) {
      return (sp->name);
    }
  }
  sprintf(name, "<script #%p>", sp->fn);
  return (name);
}
