/**
 ** ALLOCATE.H ---- bulk allocator macros for small structures
 **/

#ifndef __ALLOCATE_H_INCLUDED__
#define __ALLOCATE_H_INCLUDED__

#include <malloc.h>

#if defined(__GNUC__) || defined(__cplusplus)
#define ALLOC_INLINE	inline
#else
#define ALLOC_INLINE
#endif

#define DEFINE_ALLOCATOR(Type,Count,NextLink)			\
								\
static Type *free_list_##Type;					\
static int alloc_count_##Type;					\
								\
static ALLOC_INLINE Type *					\
ALLOC_##Type(void)						\
{								\
  Type *p;							\
  int i;							\
  if(!free_list_##Type) {					\
    if(alloc_count_##Type == 0) {				\
      alloc_count_##Type = Count * sizeof(Type);		\
      for(i = 3; ; i++) {					\
	if(((1 << i) - 32) >= alloc_count_##Type) {		\
	  alloc_count_##Type = ((1 << i) - 32) / sizeof(Type);	\
	  break;						\
	}							\
      }								\
    }								\
    if(!(p = malloc(sizeof(Type) * alloc_count_##Type))) {	\
      return(NULL);						\
    }								\
    for(i = 0; i < alloc_count_##Type; i++) {			\
      p->NextLink = free_list_##Type;				\
      free_list_##Type = p;					\
      p++;							\
    }								\
  }								\
  p = free_list_##Type;						\
  free_list_##Type = p->NextLink;				\
  return(p);							\
}								\
								\
static ALLOC_INLINE void					\
FREE_##Type(Type *p)						\
{								\
  p->NextLink = free_list_##Type;				\
  free_list_##Type = p;						\
}								\
								\
static Type *free_list_##Type = NULL;				\
static int alloc_count_##Type = 0

#endif
