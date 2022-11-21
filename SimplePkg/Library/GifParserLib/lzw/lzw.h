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

#ifndef LZW_H
#define LZW_H

#include "darray.h"

VOID lzw_compress(UINT8 bit_size,
                  UINTN size,
                  UINT8 *src,
                  UINTN *out_len,
                  UINT8 **output);
VOID lzw_compress_gif(UINT8 bit_size,
                      UINTN size,
                      UINT8 *src,
                      UINTN *out_len,
                      UINT8 **output);
VOID lzw_decompress(UINT8 bit_size,
                    UINTN size,
                    UINT8 *src,
                    UINTN *out_len,
                    UINT8 **output);

#endif
