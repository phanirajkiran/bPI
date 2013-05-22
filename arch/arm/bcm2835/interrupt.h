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

#ifndef BCM2835_INTERRUPT_HEADER_H_
#define BCM2835_INTERRUPT_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define ARMCTRL_IC_BASE          (ARM_BASE + 0x200)     /* ARM interrupt controller */

#define ARM_IRQ_PEND0            (ARMCTRL_IC_BASE) /* Top IRQ bits */
#define ARM_I0_TIMER             0 /* timer IRQ */
#define ARM_I0_MAIL              1 /* Mail IRQ */
#define ARM_I0_BELL0             2 /* Doorbell 0 */
#define ARM_I0_BELL1             3 /* Doorbell 1 */

#define ARM_IRQ_PEND1            (ARMCTRL_IC_BASE+0x4)  /* All bank1 IRQ bits */
#define ARM_IRQ_PEND2            (ARMCTRL_IC_BASE+0x8)  /* All bank2 IRQ bits */

#define ARM_IRQ_ENABLE0          (ARMCTRL_IC_BASE+0x18) /* basic IRQ's */
#define ARM_IRQ_ENABLE1          (ARMCTRL_IC_BASE+0x10) /* bank1 IRQ's */
#define ARM_IRQ_ENABLE2          (ARMCTRL_IC_BASE+0x14) /* bank2 IRQ's */

#define ARM_IRQ_DISABLE0         (ARMCTRL_IC_BASE+0x24) /* basic IRQ's */
#define ARM_IRQ_DISABLE1         (ARMCTRL_IC_BASE+0x1c) /* bank1 IRQ's */
#define ARM_IRQ_DISABLE2         (ARMCTRL_IC_BASE+0x20) /* bank2 IRQ's */

#define ARM_IRQ0_BASE            64
#define ARM_IRQ1_BASE            0
#define ARM_IRQ2_BASE            32

/* interrupt numbers */
#define ARM_IRQ_NR_TIMER		64

#ifndef __ASSEMBLY__

void enableTimerIRQ();
void disableTimerIRQ();
/* for other devices: see manual page 113 */

#endif /* __ASSEMBLY__ */

#ifdef __cplusplus
}
#endif
#endif /* BCM2835_INTERRUPT_HEADER_H_ */



