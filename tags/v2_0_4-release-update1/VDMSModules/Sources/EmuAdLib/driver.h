/*
**
** DUMMY file, to be included by fmopl.c
** Also fixes compilation issues unser MSVC6.
**
*/

#ifndef __DRIVER_H_
#define __DRIVER_H_

/* Work around ANSI compliance problem (see fmopl.cpp) */

/* Assignment of void* to a non-void pointer will generate an error,
   so we must use an intermediate type that has the appropriate
   operators defined in order to make the pointer conversion possible */

/* Make sure malloc(...) is declared NOW */
#include <stdlib.h>
#include <malloc.h>

/* Keep a reference to the standard malloc(...) */
void* (*__old_malloc)(size_t size) = malloc;

/* Force use of surrogate malloc(...) */
#define malloc __new_malloc

/* Define surrogate malloc(...)/__new_malloc(...) */
__MALLOCPTR malloc(size_t size) {
  return __old_malloc(size);
}

#endif /* __DRIVER_H_ */
