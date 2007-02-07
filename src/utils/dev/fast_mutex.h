/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#ifdef _DEBUG
#ifdef CHECK_MEMORY_LEAKS

#include <foreign/nvwa/fast_mutex.h>

#endif // CHECK_MEMORY_LEAKS
#endif // _DEBUG
