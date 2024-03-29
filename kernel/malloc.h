/*
 * Copyright (C) 2013 Beat Küng <beat-kueng@gmx.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef MALLOC_HEADER_H_
#define MALLOC_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/types.h>
/*
 * allocate num bytes of memory. returns NULL on error.
 * before the first call, the memory regions must be setup!
 */
void* kmalloc(size_t num);

void kfree(void* ptr);

/*
 * get number of free bytes (may be inaccurate due to fragmentation)
 */
size_t kfreeMallocSpace();

/*
 * get total number of malloc'able bytes
 */
size_t ktotalMallocSpace();

/*
 * this is called after finalizing the memory regions & setting up the MMU
 */
int initMalloc();



#ifdef __cplusplus
}
#endif
#endif /* MALLOC_HEADER_H_ */

