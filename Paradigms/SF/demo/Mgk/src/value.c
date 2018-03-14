/**
 ** VALUE.C ----- dataflow graph data element management routines
 **/

#include <malloc.h>
#include <string.h>

#include "libmgk.h"
#include "allocate.h"

DEFINE_ALLOCATOR(mgk_value, 500, next);

#define TYPE_MASK	((2 * T_ARRAY(0)) - 1)
#define SCALAR_MASK	(T_BUFFER - 1)
#define TYPE_SIZE(t)	((unsigned int)(t) >> 8)

#define T_ALIAS		((unsigned int)(-2))

mgk_value *
__mgk_build_value(void *data, mgk_data_type type)
{
  int itemsize, count;
  mgk_value *vp;
  mgk_buffer *bp;
  if (!data) {
    mgk_errno = E_BADDATA;
    return (NULL);
  }
  if (!(vp = ALLOC_mgk_value())) {
    mgk_errno = E_NOMEM;
    return (NULL);
  }
  vp->create_time = __mgk_tick;
  vp->next = NULL;
  vp->value = &vp->scalar_val;
  switch (type & TYPE_MASK) {
    case T_CHAR:
      vp->type = T_CHAR;
      vp->scalar_val.C = *((char *)(data));
      return (vp);
    case T_SHORTINT:
      vp->type = T_SHORTINT;
      vp->scalar_val.S = *((short *)(data));
      return (vp);
    case T_INTEGER:
      vp->type = T_INTEGER;
      vp->scalar_val.I = *((int *)(data));
      return (vp);
    case T_LONGINT:
      vp->type = T_LONGINT;
      vp->scalar_val.L = *((long *)(data));
      return (vp);
    case T_FLOAT:
      vp->type = T_FLOAT;
      vp->scalar_val.F = *((float *)(data));
      return (vp);
    case T_DOUBLE:
      vp->type = T_DOUBLE;
      vp->scalar_val.D = *((double *)(data));
      return (vp);
    case T_BUFFER | T_UNKNOWN:
    case T_BUFFER | T_CHAR:
    case T_BUFFER | T_SHORTINT:
    case T_BUFFER | T_INTEGER:
    case T_BUFFER | T_LONGINT:
    case T_BUFFER | T_FLOAT:
    case T_BUFFER | T_DOUBLE:
      if ((bp = __mgk_checked_buffer_header(data))) {
	vp->type = type & TYPE_MASK;
	if (bp->refval) {
	  void *dtacopy = __mgk_copy_buffer(data);
	  if (!dtacopy) {
	    FREE_mgk_value(vp);
	    return (NULL);
	  }
	  vp->value = dtacopy;
	  bp = __mgk_buffer_header(dtacopy);
	}
	else {
	  vp->value = data;
	}
	bp->refval = vp;
	return (vp);
      }
      break;
    case T_ARRAY(0) | T_CHAR:
      itemsize = sizeof(char);
      if (type == T_STRING) {
	count = strlen((char *)data) + 1;
	goto array2;
      }
      goto array1;
    case T_ARRAY(0) | T_SHORTINT:
      itemsize = sizeof(short);
      goto array1;
    case T_ARRAY(0) | T_INTEGER:
      itemsize = sizeof(int);
      goto array1;
    case T_ARRAY(0) | T_LONGINT:
      itemsize = sizeof(long);
      goto array1;
    case T_ARRAY(0) | T_FLOAT:
      itemsize = sizeof(float);
      goto array1;
    case T_ARRAY(0) | T_DOUBLE:
      itemsize = sizeof(double);
    array1:
      count = ((unsigned int)(type)) >> 8;
    array2:
      if ((itemsize * count) > 0) {
	void *dtacopy = __mgk_allocate_buffer(itemsize * count);
	if (dtacopy) {
	  bp = __mgk_buffer_header(dtacopy);
	  memcpy(dtacopy, data, (itemsize * count));
	  vp->value = dtacopy;
	  vp->type = (type & SCALAR_MASK) | T_BUFFER;
	  bp->refval = vp;
	  return (vp);
	}
      }
      break;
    default:
      break;
  }
  FREE_mgk_value(vp);
  mgk_errno = E_BADDATA;
  return (NULL);
}

void *
__mgk_parse_value(mgk_value * vp, mgk_data_type * typep)
{
  if (vp && (vp->type == T_ALIAS)) {
    vp = vp->value;
  }
  if (vp) {
    *typep = vp->type;
    return (vp->value);
  }
  *typep = T_NODATA;
  return (NULL);
}

mgk_value *
__mgk_copy_value(mgk_value * vp)
{
  mgk_data_type type;
  void *data = __mgk_parse_value(vp, &type);
  return (data ? __mgk_build_value(data, type) : NULL);
}

mgk_value *
__mgk_alias_value(mgk_value * vp)
{
  mgk_value *valias;
  if (vp && (vp->type == T_ALIAS)) {
    vp = vp->value;
  }
  if (vp && ((valias = ALLOC_mgk_value()))) {
    valias->create_time = vp->create_time;
    valias->next = NULL;
    if (!(vp->type & T_BUFFER)) {
      valias->type = vp->type;
      valias->scalar_val = vp->scalar_val;
      valias->value = &valias->scalar_val;
    }
    else {
      valias->type = T_ALIAS;
      valias->value = vp;
      vp->scalar_val.I = (-1);
    }
    return (valias);
  }
  return (NULL);
}

mgk_value *
__mgk_alias_value_autofree(mgk_value * vp, int *refcnt)
{
  mgk_value *valias;
  if (vp && (vp->type == T_ALIAS)) {
    vp = vp->value;
  }
  if (vp && ((valias = ALLOC_mgk_value()))) {
    valias->create_time = vp->create_time;
    valias->next = NULL;
    if (!(vp->type & T_BUFFER)) {
      valias->type = vp->type;
      valias->scalar_val = vp->scalar_val;
      valias->value = &valias->scalar_val;
    }
    else {
      valias->type = T_ALIAS;
      valias->value = vp;
      vp->scalar_val.I = ++(*refcnt);
    }
    return (valias);
  }
  return (NULL);
}

void *
__mgk_extract_value(mgk_value * vp, mgk_data_type * tp, mgk_scalar * s)
{
  int do_destroy = TRUE;
  if (vp && (vp->type == T_ALIAS)) {
    mgk_value *aliased = vp->value;
    FREE_mgk_value(vp);
    vp = aliased;
    if (vp) {
      if (vp->scalar_val.I < 0) {
	do_destroy = FALSE;
      }
      else {
	vp->scalar_val.I--;
	if (vp->scalar_val.I > 0) {
	  do_destroy = FALSE;
	}
      }
    }
  }
  if (!vp) {
    *tp = T_NODATA;
    return (NULL);
  }
  *tp = vp->type;
  if (!do_destroy) {
    return (vp->value);
  }
  if (vp->type & T_BUFFER) {
    void *val = vp->value;
    mgk_buffer *bp = __mgk_checked_buffer_header(val);
    if (bp && (bp->refval == vp)) {
      bp->refval = NULL;
    }
    FREE_mgk_value(vp);
    return (val);
  }
  *s = vp->scalar_val;
  FREE_mgk_value(vp);
  return (s);
}

void
__mgk_free_value(mgk_value * vp)
{
  if (vp && (vp->type == T_ALIAS)) {
    mgk_value *aliased = vp->value;
    FREE_mgk_value(vp);
    if ((vp = aliased)) {
      if (vp->scalar_val.I < 0) {
	return;
      }
      vp->scalar_val.I--;
      if (vp->scalar_val.I > 0) {
	return;
      }
    }
  }
  if (vp) {
    if (vp->type & T_BUFFER) {
      mgk_buffer *bp = __mgk_checked_buffer_header(vp->value);
      if (bp && (bp->refval == vp)) {
	bp->refval = NULL;
	__mgk_free_buffer(vp->value);
      }
    }
    FREE_mgk_value(vp);
  }
}

void
__mgk_free_value_list(mgk_value * vp)
{
  while (vp) {
    mgk_value *next = vp->next;
    __mgk_free_value(vp);
    vp = next;
  }
}
