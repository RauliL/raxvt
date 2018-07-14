#ifndef RAXVT_UTILS_HPP_GUARD
#define RAXVT_UTILS_HPP_GUARD

#include <string>
#include <vector>

namespace raxvt
{
  namespace utils
  {
    std::string basename(const std::string&);
    std::string trim(const std::string&);
    std::vector<std::string> split(const std::string&, char);
  }
}

#endif /* !RAXVT_UTILS_HPP_GUARD */
