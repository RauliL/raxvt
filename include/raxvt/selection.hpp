#ifndef RAXVT_SELECTION_HPP_GUARD
#define RAXVT_SELECTION_HPP_GUARD

#include <string>

#include "raxvt/coordinates.hpp"

namespace raxvt
{
  class selection
  {
  public:
    enum class operation
    {
      /** Nothing selected. */
      clear = 0,
      /** Marked a point. */
      init,
      /** Started a selection. */
      begin,
      /** Continued selection. */
      cont,
      /** Selection put in CUT_BUFFER0. */
      done
    };

    /** Selected text. */
    std::wstring text;
    /** Text copied to the clipboard. */
    std::wstring copied_text;
    /** Screen being used. */
    unsigned int screen;
    /** Number of clicks. */
    unsigned int clicks;
    /** Current operation. */
    operation op;
    /** Is the selection rectangular or not. */
    bool rectangular;
    /** Beginning of selection, <= mark. */
    coordinates beginning;
    /** Point of initial click, <= end. */
    coordinates mark;
    /** One character past end point. */
    coordinates end;

    explicit selection()
      : screen(0)
      , clicks(0)
      , op(operation::clear)
      , rectangular(false) {}

    selection(const selection&) = delete;
    void operator=(const selection&) = delete;
  };
}

#endif /* !RAXVT_SELECTION_HPP_GUARD */
