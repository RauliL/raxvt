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
#include "./emman.c"

#include <cstdlib>
#include <cstring>
#include <cinttypes>

#include "rxvtutil.h"
#include "raxvt/utils.hpp"

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

namespace raxvt
{
  namespace utils
  {
    std::string
    basename(const std::string& input)
    {
      auto index = input.find_last_of('/');

      return index == std::string::npos ? input : input.substr(index + 1);
    }

    std::string
    trim(const std::string& input)
    {
      const auto length = input.length();
      std::string::size_type i;
      std::string::size_type j;

      for (i = 0; i < length; ++i)
      {
        if (!std::isspace(input[i]))
        {
          break;
        }
      }
      for (j = length; j != 0; --j)
      {
        if (!std::isspace(input[j - 1]))
        {
          break;
        }
      }
      if (i != 0 || j != length)
      {
        return input.substr(i, j - i);
      }

      return input;
    }

    std::vector<std::string>
    split(const std::string& input, char delimiter)
    {
      const auto length = input.length();
      std::string::size_type begin = 0;
      std::string::size_type end = 0;
      std::vector<std::string> result;

      for (std::string::size_type i = 0; i < length; ++i)
      {
        if (input[i] == delimiter)
        {
          if (end - begin > 0)
          {
            result.push_back(trim(input.substr(begin, end - begin)));
          } else {
            result.push_back(std::string());
          }
          begin = end = i + 1;
        } else {
          ++end;
        }
      }
      if (end - begin > 0)
      {
        result.push_back(trim(input.substr(begin, end - begin)));
      }

      return result;
    }
  }
}
