/*----------------------------------------------------------------------*
 * File:	rxvtutil.C
 *----------------------------------------------------------------------*
 *
 * All portions of code are copyright by their respective author/s.
 * Copyright (c) 2004-2006 Marc Lehmann <schmorp@schmorp.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *----------------------------------------------------------------------*/

// we include emman.c here to avoid relying on a C compiler, or
// on the c++ compiler not complaining about .c, which is unlikely,
// but...
// This must be the first include, because the _GNU_SOURCE and
// _XOPEN_SOURCE macros, used by emman.c, must be defined before
// inclusion of any header.
#include "emman.c"

#include <cstdlib>
#include <cstring>
#include <cinttypes>

#include "rxvtutil.h"

void*
zero_initialized::operator new(std::size_t s)
{
  void *p = std::malloc(s);

  std::memset(p, 0, s);

  return p;
}

void
zero_initialized::operator delete(void* p, std::size_t s)
{
  std::free(p);
}

static void *temp_buf;
static uint32_t temp_len;

void *
rxvt_temp_buf (int len)
{
  if (len > temp_len)
    {
      std::free(temp_buf);
      temp_buf = std::malloc(len);
      temp_len = len;
    }

  return temp_buf;
}
