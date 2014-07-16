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

/*
 * method calls emmited by the compiler. never called by the kernel itself
 */

#include <kernel/panic.h>
#include <kernel/malloc.h>
#include <kernel/utils.h>

void* malloc(size_t size) {
	return kmalloc(size);
}
void* calloc(size_t num, size_t size) {
	size_t total = num*size;
	void* ptr = kmalloc(total);
	if(ptr) memset(ptr, 0, total);
	return ptr;
}
void* realloc(void* ptr, size_t size) {
	panic("call to realloc!\n");
	return NULL;
}
void free(void* ptr) {
	kfree(ptr);
}

/*
 * called when division by 0 happens. this should never happen, it's a serious
 * error
 */
void raise() {
	panic("Thou shalt not divide by 0!\n");
}


void abort() {
	panic("abort() called\n");
}


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

//static int
//remap_handle (int fh)
//{
//return 0;
//}

void initialise_monitor_handles(void) {
}

//static int
//get_errno (void)
//{
//return(0);
//}

//static int
//error (int result)
//{
//errno = get_errno ();
//return result;
//}

int _read(int file, char * ptr, int len) {
	return len;
}

int _lseek(int file, int ptr, int dir) {
	return 0;
}

int _write(int file, char * ptr, int len) {
	return len;
}

int _open(const char * path, int flags, ...) {
	return 0;
}

int _close(int file) {
	return 0;
}

void _exit(int n) {
	while (1);
}

int _kill(int n, int m) {
	return (0);
}

int _getpid(int n) {
	return 1;
}

caddr_t _sbrk(int incr) {

	panic("call to sbrk!\n");
}
struct stat;
int _fstat(int file, struct stat * st) {
	return 0;
}

int _stat(const char *fname, struct stat *st) {
	return 0;
}

int _link(void) {
	return -1;
}

int _unlink(void) {
	return -1;
}

void _raise(void) {
	return;
}

struct timezone;
struct timeval;
int _gettimeofday(struct timeval * tp, struct timezone * tzp) {
	return 0;
}

struct tms;
clock_t _times(struct tms * tp) {
	clock_t timeval = 0;

	return timeval;
}
;

int _isatty(int fd) {
	return 1;
}

int _system(const char *s) {
	if (s == NULL)
		return 0;
	return -1;
}

int _rename(const char * oldpath, const char * newpath) {
	return -1;
}

int _fini() {
	return 1;
}
