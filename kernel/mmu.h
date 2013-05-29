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

/*!
 * Memory Managment Unit methods
 */

#ifndef MMU_HEADER_H_
#define MMU_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <mmu_arch.h>


typedef struct {
	/* points to the next free frame idx if free, otherwise MSB bit is set 
	 */
	uint next;

} mem_frame;

#define getFrameIdx(phys_addr) ((ulong)(phys_addr) >> PAGE_BITS)
#define getPhysicalAddrFromFrame(frame_idx) ((ulong)(frame_idx) << PAGE_BITS)

int initFrames();


#ifdef ARCH_HAS_MMU
#define HAS_MMU

#include <kernel/mem.h>

#endif /* ARCH_HAS_MMU */



#ifdef __cplusplus
}
#endif
#endif /* MMU_HEADER_H_ */

