#include "emman.h"

#include <ecb.h>

#include <string.h>

#ifndef _WIN32
# include <unistd.h>
#endif

#if _POSIX_MAPPED_FILES > 0
# define USE_MMAP 1
# include <sys/mman.h>
# ifndef MAP_FAILED
#  define MAP_FAILED ((void *)-1)
# endif
# ifndef MAP_ANONYMOUS
#  ifdef MAP_ANON
#   define MAP_ANONYMOUS MAP_ANON
#  else
#   undef USE_MMAP
#  endif
# endif
#endif

void *
chunk_alloc (size_t size, int populate)
{
  #if USE_MMAP
    void *ptr = MAP_FAILED;

    #ifdef MAP_POPULATE
      if (populate & 1)
        ptr = mmap (0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    #endif

    if (ptr == MAP_FAILED)
      ptr = mmap (0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED)
      return 0;

    return ptr;
  #else
    return std::malloc(size);
  #endif
}

void
chunk_free (void *ptr, size_t size)
{
  #if USE_MMAP
    /* we assume the OS never mmaps at address 0 */
    if (ptr)
      munmap (ptr, size);
  #else
    return free (ptr);
  #endif
}

