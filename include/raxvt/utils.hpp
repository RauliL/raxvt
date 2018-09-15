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

    template<class T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi)
    {
      T result = v;

      if (result > hi)
      {
        result = hi;
      }
      if (result < lo)
      {
        result = lo;
      }

      return result;
    }
  }
}

#endif /* !RAXVT_UTILS_HPP_GUARD */
