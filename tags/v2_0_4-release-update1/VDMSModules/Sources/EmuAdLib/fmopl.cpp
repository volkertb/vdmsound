/* Take care of precompiled headers */
#include "stdafx.h"

/* Setup MAME compilation environment */

namespace MAME {
  /* Defines */
# define INLINE inline
# define logerror(x)

  /* Disable recurring warnings */
# pragma warning ( disable : 4018 )
# pragma warning ( disable : 4244 )

  /* Work around ANSI compliance problem (see driver.h) */
  struct __MALLOCPTR {
    void* m_ptr;

    __MALLOCPTR(void) : m_ptr(NULL) { }
    __MALLOCPTR(void* src) : m_ptr(src) { }
    void* operator=(void* rhs) { return (m_ptr = rhs); }
    operator int*() const { return (int*)m_ptr; }
    operator int**() const { return (int**)m_ptr; }
    operator char*() const { return (char*)m_ptr; }
  };

  /* Bring in the MAME OPL emulation */
# define HAS_YM3812 1
# include "fmopl.c"

}
