/* This file is a part of liblzw
 *
 * Copyright 2019, Jeffery Stager
 *
 * liblzw is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblzw is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liblzw.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LZW_TABLE_H
#define LZW_TABLE_H

#include "darray.h"

#define LZW_MAX_ENTRY_EXP 12
#define LZW_MAX_ENTRIES (1 << LZW_MAX_ENTRY_EXP)

enum lzw_table_type {
  LZW_TABLE_COMPRESS,
  LZW_TABLE_DECOMPRESS
};

struct lzw_entry {
  UINTN len;
  UINTN prev;
  UINTN code;
  UINT8 val;
};

struct lzw_table {
  enum lzw_table_type type;
  UINTN n_entries;
  UINTN size;
  CHAR8 *initialized;
  struct lzw_entry *entries;
};

VOID lzw_table_init(struct lzw_table *t,
                    enum lzw_table_type type,
                    UINT8 bit_size);
VOID lzw_table_deinit(struct lzw_table *t);
VOID lzw_table_add(struct lzw_table *t, struct lzw_entry *e);
INT32 lzw_table_lookup_entry(struct lzw_table *t,
                           struct lzw_entry *e,
                           UINTN *out_code);
INT32 lzw_table_lookup_code(struct lzw_table *t,
                          UINTN code,
                          struct lzw_entry *e);
VOID lzw_table_str(struct lzw_table *t,
                   UINTN code,
                   struct darray **out_buf);
UINT8 lzw_entry_head(struct lzw_table *t, struct lzw_entry *e);

#endif
