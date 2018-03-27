#ifndef RAXVT_CONNECTION_HPP_GUARD
#define RAXVT_CONNECTION_HPP_GUARD

#include <string>

#if defined(__linux__)
# include <linux/limits.h>
#endif

#if !defined(PATH_MAX)
# define PATH_MAX 4096
#endif

namespace raxvt
{
  struct connection
  {
    int fd;

    static char* unix_sockname();

    void send(const char* data, std::size_t length);
    void send(const char* data);
    void send(int data);

    bool recv(std::string& data, std::size_t* length = nullptr);
    bool recv(int& data);
  };
}

#endif /* !RAXVT_CONNECTION_HPP_GUARD */
