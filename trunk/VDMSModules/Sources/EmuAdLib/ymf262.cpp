/* Take care of precompiled headers */
#include "stdafx.h"

/* Setup MAME compilation environment */

namespace MAME {
  /* Defines */
# define INLINE static inline
# define logerror

  /* Disable recurring warnings */
# pragma warning ( disable : 4018 )
# pragma warning ( disable : 4244 )

  /* Bring in the MAME OPL emulation */
# define HAS_YMF262 1
# include "ymf262.c"

  /* Cleanup */
# undef HAS_YMF262
# undef logerror
# undef INLINE

}
