/* Take care of precompiled headers */
#include "stdafx.h"

/* Setup MAME compilation environment */

namespace MAME {
  /* Defines */
# define INLINE inline
# define logerror

  /* Disable recurring warnings */
# pragma warning ( disable : 4018 )
# pragma warning ( disable : 4244 )

  /* Work around ANSI compliance problem */

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
  struct __MALLOCPTR {
    void* m_ptr;

    __MALLOCPTR(void) : m_ptr(NULL) { }
    __MALLOCPTR(void* src) : m_ptr(src) { }
    void* operator=(void* rhs) { return (m_ptr = rhs); }
    operator int*() const { return (int*)m_ptr; }
    operator int**() const { return (int**)m_ptr; }
    operator char*() const { return (char*)m_ptr; }
  } malloc(size_t size) {
    return __old_malloc(size);
  }

  /* Bring in the MAME OPL emulation */
# define HAS_YM3812 1
# include "fmopl.c"

}
