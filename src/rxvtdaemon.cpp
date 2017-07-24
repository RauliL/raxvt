/*----------------------------------------------------------------------*
 * File:	rxvtdaemon.C
 *----------------------------------------------------------------------*
 *
 * All portions of code are copyright by their respective author/s.
 * Copyright (c) 2003-2007 Marc Lehmann <schmorp@schmorp.de>
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

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cerrno>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <basedir.h>

#include "rxvtdaemon.h"

char*
rxvt_connection::unix_sockname()
{
  const char* path = std::getenv("RAXVT_SOCKET");

  if (!path)
  {
    const char* runtime_dir = xdgRuntimeDirectory(nullptr);
    struct utsname u;
    char buffer[PATH_MAX];

    uname(&u);
    if (runtime_dir)
    {
      std::snprintf(buffer, PATH_MAX, "%s/raxvtd-%s", runtime_dir, u.nodename);
      std::free(const_cast<char*>(runtime_dir));
    } else {
      std::snprintf(buffer, PATH_MAX, "/tmp/raxvtd-%s", u.nodename);
    }

    return strdup(buffer);
  }

  return strdup(path);
}

void
rxvt_connection::send(const char *data, std::size_t length)
{
  uint8_t s[2];

  if (length > 65535)
  {
    length = 65535;
  }

  s[0] = length >> 8;
  s[1] = length;

  ::write(fd, s, 2);
  ::write(fd, data, length);
}

void
rxvt_connection::send(const char* data)
{
  send(data, std::strlen(data));
}

bool
rxvt_connection::recv(std::string& data, std::size_t* length)
{
  uint8_t s[2];
  std::size_t l;
  char* buffer;
  bool success;

  if (::read(fd, s, 2) != 2)
  {
    return false;
  }

  l = (s[0] << 8) + s[1];
  if (l > 65535)
  {
    return false;
  }

  if (length)
  {
    *length = l;
  }

  buffer = new char[l];
  success = ::read(fd, buffer, l) == l;
  if (success)
  {
    data.reserve(l);
    data.assign(buffer, l);
  }
  delete[] buffer;

  return success;
}

void
rxvt_connection::send(int data)
{
  uint8_t s[4];

  s[0] = data >> 24;
  s[1] = data >> 16;
  s[2] = data >> 8;
  s[3] = data;

  ::write(fd, s, 4);
}

bool
rxvt_connection::recv(int& data)
{
  uint8_t s[4];

  if (::read(fd, s, 4) != 4)
  {
    return false;
  }

  data = (((((s[0] << 8) | s[1]) << 8) | s[2]) << 8) | s[3];

  return true;
}
