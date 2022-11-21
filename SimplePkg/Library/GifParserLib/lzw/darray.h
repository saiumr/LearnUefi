/* This file is a part of darray
 *
 * Copyright 2019, Jeffery Stager
 *
 * darray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * darray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with darray.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DARRAY_H
#define DARRAY_H

#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

struct darray;

#define dapusht(d, t) dapushn(d, sizeof(t), (UINT8 *) &(t));

struct darray *danew(UINTN cap);
VOID dafree(struct darray *d);
UINT8 *dapeel(struct darray *d);
INT32 dapush(struct darray *d, UINT8 byte);
INT32 dapushn(struct darray *d, UINTN len, UINT8 *byte);
UINTN dalen(struct darray *d);
UINT8 *daptr(struct darray *d);

#endif
