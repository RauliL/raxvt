#ifndef EMMAN_H_
#define EMMAN_H_

#include <stddef.h>

#if __cplusplus
extern "C" {
#endif

void *chunk_alloc (size_t size, int populate);
void chunk_free (void *ptr, size_t size);

#if __cplusplus
}
#endif

#endif

