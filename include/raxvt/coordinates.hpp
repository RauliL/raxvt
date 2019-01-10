#ifndef RAXVT_COORDINATES_HPP_GUARD
#define RAXVT_COORDINATES_HPP_GUARD

namespace raxvt
{
  class coordinates
  {
  public:
    int row;
    int col;

    coordinates()
      : row(0)
      , col(0) {}

    coordinates(const coordinates& that)
      : row(that.row)
      , col(that.col) {}

    coordinates& operator=(const coordinates& that)
    {
      row = that.row;
      col = that.col;

      return *this;
    }

    int compare(const coordinates& that) const
    {
      const int a = row;
      const int b = col;
      const int c = that.row;
      const int d = that.col;

      if (a > c || (a == c && b > d))
      {
        return 1;
      }
      else if (a < c || (a == c && b < d))
      {
        return -1;
      } else {
        return 0;
      }
    }

    inline bool operator==(const coordinates& that) const
    {
      return compare(that) == 0;
    }

    inline bool operator!=(const coordinates& that) const
    {
      return compare(that) != 0;
    }

    inline bool operator>(const coordinates& that) const
    {
      return compare(that) > 0;
    }

    inline bool operator<(const coordinates& that) const
    {
      return compare(that) < 0;
    }

    inline bool operator>=(const coordinates& that) const
    {
      return compare(that) >= 0;
    }

    inline bool operator<=(const coordinates& that) const
    {
      return compare(that) <= 0;
    }
  };
}

#endif /* !RAXVT_COORDINATES_HPP_GUARD */
