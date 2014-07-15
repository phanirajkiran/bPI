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

#ifndef MMU_ARCH_HEADER_H_
#define MMU_ARCH_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define ARCH_HAS_MMU


#define PAGE_BITS 12
#define PAGE_SIZE (1<<PAGE_BITS)


void initMMU();


#ifdef __cplusplus
}
#endif
#endif /* MMU_ARCH_HEADER_H_ */



