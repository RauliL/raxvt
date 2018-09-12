/*----------------------------------------------------------------------*
 * File:	rxvtc.C
 *----------------------------------------------------------------------*
 *
 * All portions of code are copyright by their respective author/s.
 * Copyright (c) 2003-2006 Marc Lehmann <schmorp@schmorp.de>
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
#include "../config.h"

#include <cstring>
#include <csignal>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "./libptytty.h"

#include "raxvt/connection.hpp"

extern char** environ;

static const int STATUS_SUCCESS = EXIT_SUCCESS;
static const int STATUS_FAILURE = EXIT_FAILURE;
static const int STATUS_CONNECTION_FAILED = 2;

struct client : raxvt::connection
{
  client()
  {
    sockaddr_un sa;
    char* sockname = raxvt::connection::unix_sockname();

    if (std::strlen(sockname) >= sizeof(sa.sun_path))
    {
      std::fputs("socket name too long, aborting.\n", stderr);
      std::exit(STATUS_FAILURE);
    }

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      std::perror("unable to create communications socket");
      std::exit(STATUS_FAILURE);
    }

    sa.sun_family = AF_UNIX;
    std::strcpy(sa.sun_path, sockname);
    std::free(sockname);

    if (connect(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)))
    {
      std::perror("unable to connect to the rxvt-unicode daemon");
      std::exit(STATUS_CONNECTION_FAILED);
    }
  }
};

extern char **environ;

int
main(int argc, const char *const *argv)
{
  // instead of getcwd we could opendir (".") and pass the fd for fchdir *g*
  char cwd[PATH_MAX];

  if (!getcwd(cwd, sizeof(cwd)))
  {
    std::perror("unable to determine current working directory");
    std::exit(STATUS_FAILURE);
  }

  client c;

  {
    sigset_t ss;

    sigemptyset(&ss);
    sigaddset(&ss, SIGHUP);
    sigaddset(&ss, SIGPIPE);
    sigprocmask(SIG_BLOCK, &ss, 0);
  }

  if (argc >= 2 && !std::strcmp(argv[1], "-k"))
  {
    c.send("QUIT");
    std::exit(STATUS_SUCCESS);
  }

  c.send("NEW");

  for (char* const* var = environ; *var; ++var)
  {
    c.send("ENV");
    c.send(*var);
  }

  const char* base = std::strrchr(argv[0], '/');
  base = base ? base + 1 : argv[0];

  c.send("ARG");
  c.send(std::strcmp(base, RXVTNAME "c") ? base : RXVTNAME);

  c.send("ARG");
  c.send("-cd");

  c.send("ARG");
  c.send(cwd);

  for (int i = 1; i < argc; i++)
  {
    c.send("ARG");
    c.send (argv[i]);
  }

  c.send("END");

  std::string tok;
  int cint;

  for (;;)
  {
    if (!c.recv(tok))
    {
      std::fprintf(stderr, "protocol error: unexpected eof from server.\n");
      break;
    }
    else if (!tok.compare("MSG") && c.recv(tok))
    {
      std::fprintf(stderr, "%s", tok.c_str());
    }
    else if (!tok.compare("GETFD") && c.recv(cint))
    {
      if (!ptytty::send_fd (c.fd, cint))
      {
        std::fprintf(stderr, "unable to send fd %d: ", cint);
        std::perror(0);
        std::exit(STATUS_FAILURE);
      }
    }
    else if (!tok.compare("END"))
    {
      int success;

      if (c.recv(success))
      {
        std::exit(success ? STATUS_SUCCESS : STATUS_FAILURE);
      }
    } else {
      std::fprintf(stderr, "protocol error: received unsupported token '%s'.\n", tok.c_str());
      break;
    }
  }

  return STATUS_FAILURE;
}

