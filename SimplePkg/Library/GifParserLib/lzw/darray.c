/* This file is a part of darray
 *
 * Copyright 2019, Jeffery Stager
 *
 * darray is FreePool software: you can redistribute it and/or modify
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

#include "darray.h"

struct darray {
  UINTN len;
  UINTN cap;
  UINT8 *ptr;
};

static INT32 increase_mem(struct darray *d) {
  UINT8 *tmp;
  UINTN old_cap;

  old_cap = d->cap;
  d->cap = (d->cap)<<1;
  tmp = ReallocatePool(old_cap, d->cap, d->ptr);
  if (!tmp) {
    d->cap = old_cap;
    return -1;
  }
  d->ptr = tmp;
  return 0;
}

/* **************************************** */
/* Public */
/* **************************************** */

struct darray *danew(UINTN cap) {
  struct darray *out;

  out = AllocatePool(sizeof(struct darray));
  if (!out) return NULL;
  out->len = 0;
  out->cap = cap;
  out->ptr = AllocatePool(cap);
  if (!out->ptr) {
    FreePool(out);
    return NULL;
  }
  return out;
}

VOID dafree(struct darray *d) {
  if (!d) return;
  FreePool(d->ptr);
  FreePool(d);
}

UINT8 *dapeel(struct darray *d) {
  UINT8 *result;

  if (!d) return NULL;
  result = daptr(d);
  FreePool(d);
  return result;
}

INT32 dapush(struct darray *d, UINT8 byte) {
  if (!d) return -1;
  if (d->len >= d->cap) {
    if (increase_mem(d)) return -1;
  }
  d->ptr[d->len++] = byte;
  return 0;
}

INT32 dapushn(struct darray *d, UINTN len, UINT8 *bytes) {
  if (!d) return -1;
  if (!bytes) return -1;
  while ((d->len + len) >= d->cap) {
    if (increase_mem(d)) return -1;
  }
  CopyMem(d->ptr + d->len, bytes, len);
  d->len += len;
  return 0;
}

UINTN dalen(struct darray *d) {
  return d->len;
}

UINTN dacap(struct darray *d) {
  return d->cap;
}

UINT8 *daptr(struct darray *d) {
  return d->ptr;
}
