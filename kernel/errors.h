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


#ifndef ERRORS_HEADER_H_
#define ERRORS_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* errors: error numbers will always be returned as negative numbers, but
 * defined as positive number
 */

#define SUCCESS							0

#define E_FORMAT 						10
#define E_BUFFER_FULL 					11
#define E_NO_SUCH_RESOURCE 				12
#define E_NOT_INIT		 				13 /* something was not initialized */
#define E_INVALID_PARAM					14
#define E_UNSUPPORTED					15


#ifdef __cplusplus
}
#endif
#endif /* ERRORS_HEADER_H_ */

