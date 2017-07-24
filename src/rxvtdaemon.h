#ifndef RXVT_DAEMON_H
#define RXVT_DAEMON_H

#include <string>

#include <climits>

#ifndef PATH_MAX
# define PATH_MAX 16384
#endif

#include "rxvtutil.h"

struct rxvt_connection
{
  int fd;

  static char* unix_sockname();

  void send(const char* data, std::size_t length);
  void send(const char* data);
  void send(int data);

  bool recv(std::string& data, std::size_t* length = nullptr);
  bool recv(int& data);
};

#endif

