/**
 ** BUFFER.C ----- buffer management
 **/

#include <malloc.h>
#include <string.h>

#include "libmgk.h"

void *
__mgk_allocate_buffer(unsigned int size)
{
  mgk_buffer *result;
  if (size == 0) {
    mgk_errno = E_NOMEM;
    return (NULL);
  }
  result = malloc(sizeof(mgk_buffer) + size);
  if (!result) {
    mgk_errno = E_NOMEM;
    return (NULL);
  }
  result->tag = OBJ_BUFFER;
  result->size = size;
  result->refval = NULL;
  result->next = NULL;
  return (&result[1]);
}

void *
mgk_allocate_buffer(unsigned int size, int do_zero)
{
  void *result = __mgk_allocate_buffer(size);
  if (result) {
    if (do_zero) {
      memset(result, 0, size);
    }
    if (__mgk_running_node) {
      mgk_buffer *bp = __mgk_buffer_header(result);
      bp->next = __mgk_running_node->buffer_list;
      __mgk_running_node->buffer_list = bp;
    }
  }
  return (result);
}

void *
__mgk_copy_buffer(void *buffer)
{
  mgk_buffer *src = __mgk_checked_buffer_header(buffer);
  void *dst;
  if (!src) {
    mgk_errno = E_NOTBUFFER;
    return (NULL);
  }
  if (!(dst = __mgk_allocate_buffer(src->size))) {
    return (NULL);
  }
  memcpy(dst, buffer, src->size);
  return (dst);
}

void *
mgk_copy_buffer(void *buffer)
{
  void *result = __mgk_copy_buffer(buffer);
  if (result && __mgk_running_node) {
    mgk_buffer *bp = __mgk_buffer_header(result);
    bp->next = __mgk_running_node->buffer_list;
    __mgk_running_node->buffer_list = bp;
  }
  return (result);
}

mgk_error_code
__mgk_free_buffer(void *buffer)
{
  mgk_buffer *buf = __mgk_checked_buffer_header(buffer);
  if (!buf) {
    return (E_NOTBUFFER);
  }
  if (buf->refval) {
    return (E_BUFFERUSED);
  }
  buf->tag = OBJ_INVALID;
  free(buf);
  return (E_SUCCESS);
}

mgk_error_code
mgk_free_buffer(void *buffer)
{
  mgk_buffer *buf = __mgk_checked_buffer_header(buffer);
  if (!buf) {
    return (E_NOTBUFFER);
  }
  if (buf->refval) {
    return (E_BUFFERUSED);
  }
  if (__mgk_running_node) {
    mgk_buffer **bpp = &__mgk_running_node->buffer_list;
    while (*bpp) {
      if (*bpp == buf) {
	*bpp = buf->next;
	break;
      }
      bpp = &(*bpp)->next;
    }
  }
  buf->tag = OBJ_INVALID;
  free(buf);
  return (E_SUCCESS);
}

int
mgk_is_a_buffer(void *buffer)
{
  return (__mgk_check_buffer(buffer) ? TRUE : FALSE);
}

unsigned int
mgk_buffer_size(void *buffer)
{
  mgk_buffer *buf = __mgk_checked_buffer_header(buffer);
  if (buf) {
    mgk_errno = E_SUCCESS;
    return (buf->size);
  }
  mgk_errno = E_NOTBUFFER;
  return (0);
}
