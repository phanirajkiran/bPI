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

#include <kernel/malloc.h>
#include <kernel/types.h>

/*
 * this overrides the standard operator new & delete C++ methods
 */

void *operator new(size_t size) {
    return kmalloc(size);
}
 
void *operator new[](size_t size) {
    return kmalloc(size);
}
 
void operator delete(void *p) {
    kfree(p);
}
 
void operator delete[](void *p) {
    kfree(p);
}

