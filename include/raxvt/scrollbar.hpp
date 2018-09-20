#ifndef RAXVT_SCROLLBAR_HPP_GUARD
#define RAXVT_SCROLLBAR_HPP_GUARD

#include "../../config.h"

#include <algorithm>
#include <memory>

#include <X11/Xlib.h>

struct rxvt_term;

namespace raxvt
{
  class scrollbar
  {
  public:
    enum class style
    {
      next = 1,
      xterm = 2,
      plain = 3,
      rxvt = 4
    };

    enum class state
    {
      off,
      idle,
      motion,
      up,
      down
    };

    enum class alignment
    {
      centre,
      top,
      bottom
    };

    static std::shared_ptr<scrollbar> make(rxvt_term* terminal);

    explicit scrollbar(rxvt_term* terminal);
    virtual ~scrollbar();

    virtual enum style style() const = 0;

    virtual int color() const;

    inline enum state state() const
    {
      return m_state;
    }

    inline void state(enum state state)
    {
      m_state = state;
    }

    inline int top() const
    {
      return m_top;
    }

    inline int bottom() const
    {
      return m_bottom;
    }

    inline int size() const
    {
      return std::max<int>(m_end - m_beginning, 0);
    }

    inline int total_width() const
    {
      return m_width + m_shadow * 2;
    }

    virtual int min_height() const;

    inline int position(int y) const
    {
      return y - m_beginning;
    }

    inline enum alignment alignment() const
    {
      return m_alignment;
    }

    inline ::Window window() const
    {
      return m_window;
    }

    inline ::Cursor leftptr_cursor() const
    {
      return m_leftptr_cursor;
    }

    /**
     * Map or unmap an scrollbar.
     */
    void map(bool map);

    /**
     * Update the scrollbar view w.r.t. slider heights etc.
     */
    bool show(bool refresh);

    void resize();

    virtual bool is_up_button_available(int y) const;
    virtual bool is_down_button_available(int y) const;

    inline bool is_above_slider_available(int y) const
    {
      return y < m_top;
    }

    inline bool is_below_slider_available(int y) const
    {
      return y > m_bottom;
    }

  protected:
    /**
     * Initializes the scrollbar after initial window creation.
     */
    virtual void initialize() = 0;

    virtual bool render(bool update) = 0;

    /**
     * Updates style dependent data.
     */
    virtual void update_data();

  protected:
    /** Terminal where the scrollbar has been installed to. */
    rxvt_term* m_terminal;
    /** State of the scrollbar. */
    enum state m_state;
    /** Slider sub-window begin height. */
    int m_beginning;
    /** Slider sub-window end height. */
    int m_end;
    /** Slider top position. */
    int m_top;
    /** Slider bottom position. */
    int m_bottom;
    /** Scrollbar width. */
    int m_width;
    /** Scrollbar shadow width. */
    int m_shadow;
    /** Last bottom position. */
    int m_last_bottom;
    /** Last top position. */
    int m_last_top;
    /** Last state. */
    enum state m_last_state;
    enum alignment m_alignment;
    ::Window m_window;
    ::Cursor m_leftptr_cursor;
  };
}

#endif /* !RAXVT_SCROLLBAR_HPP_GUARD */
