/*
**
** DUMMY file, to be included by fmopl.c
** Also fixes compilation issues unser MSVC6.
**
*/

/* Work around ANSI compliance problem (see fmopl.cpp) */

/* Assignment of void* to a non-void pointer will generate an error,
   so we must use an intermediate type that has the appropriate
   operators defined in order to make the pointer conversion possible */

/* Make sure malloc(...) is declared NOW */

#include <stdlib.h>
#include <malloc.h>

/* Force use of surrogate malloc(...) while still leaving
   the original malloc(...) visible as __old_malloc(...) */

#define __old_malloc malloc
#define malloc __new_malloc

/* Define surrogate malloc(...)/__new_malloc(...) */

__MALLOCPTR malloc(size_t size) {
  return __old_malloc(size);
}
