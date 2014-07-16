/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
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

#ifndef _MALLOC_CONFIG_HEADER_H_
#define _MALLOC_CONFIG_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <kernel/utils.h>

/* wrapper for FreeRTOS malloc implementation */

#define portBYTE_ALIGNMENT 8
#if portBYTE_ALIGNMENT == 8
	#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#define vTaskSuspendAll() NOP
#define xTaskResumeAll() NOP
#define mtCOVERAGE_TEST_MARKER()
#define traceMALLOC( pvAddress, uiSize )
#define traceFREE( pvAddress, uiSize )
#define portPOINTER_SIZE_TYPE uint32_t

#define configASSERT(x) ASSERT(x)


#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_CONFIG_HEADER_H_ */
