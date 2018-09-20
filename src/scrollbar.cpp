/*----------------------------------------------------------------------*
 * File:	scrollbar.C
 *----------------------------------------------------------------------*
 *
 * Copyright (c) 1997,1998 mj olesen <olesen@me.QueensU.CA>
 * Copyright (c) 1998      Alfredo K. Kojima <kojima@windowmaker.org>
 *				- N*XTstep like scrollbars
 * Copyright (c) 1999-2001 Geoff Wing <gcw@pobox.com>
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
#include "raxvt/scrollbar.hpp"
#include "rxvt.h"

namespace
{
#if defined(XTERM_SCROLLBAR)
  static const int SB_WIDTH_XTERM = 15;
#endif
#if defined(PLAIN_SCROLLBAR)
  static const int SB_WIDTH_PLAIN = 7;
#endif
#if defined(NEXT_SCROLLBAR)
  static const int SB_WIDTH_NEXT = 19;
#endif
  static const int SB_WIDTH_RXVT = 10;

#if defined(NEXT_SCROLLBAR)
  static const int SB_PADDING = 1;
  static const int SB_BORDER_WIDTH = 1;
  static const int SB_BEVEL_WIDTH_UPPER_LEFT = 1;
  static const int SB_BEVEL_WIDTH_LOWER_RIGHT = 2;
  static const int SB_LEFT_PADDING = SB_PADDING + SB_BORDER_WIDTH;
  static const int SB_MARGIN_SPACE = SB_PADDING * 2;
  static const int SB_BUTTON_WIDTH = SB_WIDTH_NEXT - SB_MARGIN_SPACE - SB_BORDER_WIDTH;
  static const int SB_BUTTON_HEIGHT = SB_BUTTON_WIDTH;
  static const int SB_BUTTON_SINGLE_HEIGHT = SB_BUTTON_HEIGHT + SB_PADDING;
  static const int SB_BUTTON_BOTH_HEIGHT = SB_BUTTON_SINGLE_HEIGHT * 2;
  static const int SB_BUTTON_TOTAL_HEIGHT = SB_BUTTON_BOTH_HEIGHT + SB_PADDING;
  static const int SB_BUTTON_BEVEL_X = SB_LEFT_PADDING;
  static const int SB_BUTTON_FACE_X = SB_BUTTON_BEVEL_X + SB_BEVEL_WIDTH_UPPER_LEFT;
  static const int SB_THUMB_MIN_HEIGHT = SB_BUTTON_WIDTH - (SB_PADDING * 2);
  static const int SCROLLER_DIMPLE_WIDTH = 6;
  static const int SCROLLER_DIMPLE_HEIGHT = 6;
  static const int ARROW_WIDTH = 13;
  static const int ARROW_HEIGHT = 13;
  static const int n_stp_width = 8;
  static const int n_stp_height = 2;
  static const unsigned char n_stp_bits[] = {0x55, 0xaa};

  static const char* const SCROLLER_DIMPLE[] =
  {
    ".%###.",
    "%#%%%%",
    "#%%...",
    "#%..  ",
    "#%.   ",
    ".%.  ."
  };
  static const char* const SCROLLER_ARROW_UP[] =
  {
    ".............",
    ".............",
    "......%......",
    "......#......",
    ".....%#%.....",
    ".....###.....",
    "....%###%....",
    "....#####....",
    "...%#####%...",
    "...#######...",
    "..%#######%..",
    ".............",
    "............."
  };
  static const char* const SCROLLER_ARROW_DOWN[] =
  {
    ".............",
    ".............",
    "..%#######%..",
    "...#######...",
    "...%#####%...",
    "....#####....",
    "....%###%....",
    ".....###.....",
    ".....%#%.....",
    "......#......",
    "......%......",
    ".............",
    "............."
  };
  static const char* const HI_SCROLLER_ARROW_UP[] =
  {
    "             ",
    "             ",
    "      %      ",
    "      %      ",
    "     %%%     ",
    "     %%%     ",
    "    %%%%%    ",
    "    %%%%%    ",
    "   %%%%%%%   ",
    "   %%%%%%%   ",
    "  %%%%%%%%%  ",
    "             ",
    "             "
  };
  static const char* const HI_SCROLLER_ARROW_DOWN[] =
  {
    "             ",
    "             ",
    "  %%%%%%%%%  ",
    "   %%%%%%%   ",
    "   %%%%%%%   ",
    "    %%%%%    ",
    "    %%%%%    ",
    "     %%%     ",
    "     %%%     ",
    "      %      ",
    "      %      ",
    "             ",
    "             "
  };

  /**
   * N*XTSTEP like scrollbar - Originally written by Alfredo K. Kojima.
   */
  class next_scrollbar : public raxvt::scrollbar
  {
  public:
    explicit next_scrollbar(rxvt_term* terminal)
      : raxvt::scrollbar(terminal)
      , m_black_gc(0UL)
      , m_white_gc(0UL)
      , m_gray_gc(0UL)
      , m_dark_gc(0UL)
      , m_stipple_gc(0UL)
      , m_dimple(0UL)
      , m_up_arrow(0UL)
      , m_down_arrow(0UL)
      , m_up_arrow_hi(0UL)
      , m_down_arrow_hi(0UL)
      , m_last_has_sb(false)
    {
      const auto thickness = terminal->get_setting(Rs_scrollBar_thickness);

      m_width = SB_WIDTH_NEXT;

      if (thickness)
      {
        const auto value = std::atoi(thickness);

        if (value >= SB_WIDTH_MINIMUM)
        {
          m_width = std::min<int>(value, SB_WIDTH_MAXIMUM);
        }
      }
    }

    ~next_scrollbar()
    {
      if (m_black_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_black_gc);
      }
      if (m_white_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_white_gc);
      }
      if (m_gray_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_gray_gc);
      }
      if (m_dark_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_dark_gc);
      }
      if (m_stipple_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_stipple_gc);
      }
      if (m_dimple)
      {
        ::XFreePixmap(m_terminal->dpy, m_dimple);
      }
      if (m_up_arrow)
      {
        ::XFreePixmap(m_terminal->dpy, m_up_arrow);
      }
      if (m_down_arrow)
      {
        ::XFreePixmap(m_terminal->dpy, m_down_arrow);
      }
      if (m_up_arrow_hi)
      {
        ::XFreePixmap(m_terminal->dpy, m_up_arrow_hi);
      }
      if (m_down_arrow_hi)
      {
        ::XFreePixmap(m_terminal->dpy, m_down_arrow_hi);
      }
    }

    enum style style() const
    {
      return style::next;
    }

    int min_height() const
    {
      return SB_THUMB_MIN_HEIGHT;
    }

    bool is_up_button_available(int y) const
    {
      return y > m_end && y <= m_end + m_width + 1;
    }

    bool is_down_button_available(int y) const
    {
      return y > m_end + m_width + 1;
    }

  protected:
    void initialize()
    {
      ::XGCValues gcvalue;
      rxvt_color color;
      ::Pixmap stipple;
      unsigned long light;
      unsigned long dark;

      gcvalue.graphics_exposures = False;

      gcvalue.foreground = m_terminal->lookup_color(
        Color_Black,
        m_terminal->pix_colors_focused
      );
      m_black_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground | GCGraphicsExposures,
        &gcvalue
      );

      gcvalue.foreground = m_terminal->lookup_color(
        Color_White,
        m_terminal->pix_colors_focused
      );
      m_white_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground | GCGraphicsExposures,
        &gcvalue
      );

      light = m_terminal->lookup_color(
        Color_scroll,
        m_terminal->pix_colors_focused
      );
      gcvalue.foreground = light;
      m_gray_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground | GCGraphicsExposures,
        &gcvalue
      );

      dark = m_terminal->lookup_color(
        Color_Grey25,
        m_terminal->pix_colors_focused
      );
      gcvalue.foreground = dark;
      m_dark_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground | GCGraphicsExposures,
        &gcvalue
      );

      stipple = ::XCreateBitmapFromData(
        m_terminal->dpy,
        m_window,
        reinterpret_cast<const char*>(n_stp_bits),
        n_stp_width,
        n_stp_height
      );

      gcvalue.foreground = dark;
      gcvalue.background = light;
      gcvalue.fill_style = FillOpaqueStippled;
      gcvalue.stipple = stipple;

      m_stipple_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground
        | GCBackground
        | GCStipple
        | GCFillStyle
        | GCGraphicsExposures,
        &gcvalue
      );

      m_dimple = render_pixmap(
        SCROLLER_DIMPLE,
        SCROLLER_DIMPLE_WIDTH,
        SCROLLER_DIMPLE_HEIGHT
      );

      m_up_arrow = render_pixmap(
        SCROLLER_ARROW_UP,
        ARROW_WIDTH,
        ARROW_HEIGHT
      );
      m_down_arrow = render_pixmap(
        SCROLLER_ARROW_DOWN,
        ARROW_WIDTH,
        ARROW_HEIGHT
      );
      m_up_arrow_hi = render_pixmap(
        HI_SCROLLER_ARROW_UP,
        ARROW_WIDTH,
        ARROW_HEIGHT
      );
      m_down_arrow_hi = render_pixmap(
        HI_SCROLLER_ARROW_DOWN,
        ARROW_WIDTH,
        ARROW_HEIGHT
      );
    }

    bool render(bool update)
    {
      int height = m_end + SB_BUTTON_TOTAL_HEIGHT + SB_PADDING;
      ::Drawable src;
      bool has_sb = m_terminal->top_row;
      int stipple_height = height - SB_PADDING;

      if (has_sb)
      {
        stipple_height -= SB_BUTTON_TOTAL_HEIGHT;
      } else {
        stipple_height -= SB_PADDING;
      }

      if (has_sb != m_last_has_sb || !update)
      {
        m_last_has_sb = has_sb;
        ::XFillRectangle(
          m_terminal->dpy,
          m_window,
          m_gray_gc,
          0,
          0,
          SB_WIDTH_NEXT + 1,
          height
        );
        ::XDrawRectangle(
          m_terminal->dpy,
          m_window,
          m_black_gc,
          0,
          -SB_BORDER_WIDTH,
          SB_WIDTH_NEXT,
          height + SB_BORDER_WIDTH
        );
        ::XFillRectangle(
          m_terminal->dpy,
          m_window,
          m_stipple_gc,
          SB_LEFT_PADDING,
          SB_PADDING,
          SB_BUTTON_WIDTH,
          stipple_height
        );
      }

      if (m_terminal->top_row)
      {
        if (m_last_top < m_top || !update)
        {
          ::XFillRectangle(
            m_terminal->dpy,
            m_window,
            m_stipple_gc,
            SB_LEFT_PADDING,
            SB_PADDING + m_last_top,
            SB_BUTTON_WIDTH,
            m_top - m_last_top
          );
        }

        if (m_bottom < m_last_bottom || !update)
        {
          ::XFillRectangle(
            m_terminal->dpy,
            m_window,
            m_stipple_gc,
            SB_LEFT_PADDING,
            m_bottom + SB_PADDING,
            SB_BUTTON_WIDTH,
            m_last_bottom - m_bottom
          );
        }

        ::XFillRectangle(
          m_terminal->dpy,
          m_window,
          m_gray_gc,
          SB_LEFT_PADDING,
          m_top + SB_PADDING,
          SB_BUTTON_WIDTH,
          m_bottom - m_top
        );

        ::XCopyArea(
          m_terminal->dpy,
          m_dimple,
          m_window,
          m_white_gc,
          0,
          0,
          SCROLLER_DIMPLE_WIDTH,
          SCROLLER_DIMPLE_HEIGHT,
          (SB_WIDTH_NEXT - SCROLLER_DIMPLE_WIDTH) / 2,
          m_top + SB_BEVEL_WIDTH_UPPER_LEFT
            + (m_bottom - m_top - SCROLLER_DIMPLE_HEIGHT) / 2
        );

        draw_bevel(
          SB_BUTTON_BEVEL_X,
          m_top + SB_PADDING,
          SB_BUTTON_WIDTH,
          m_bottom - m_top
        );
        draw_bevel(
          SB_BUTTON_BEVEL_X,
          height - SB_BUTTON_BOTH_HEIGHT,
          SB_BUTTON_WIDTH,
          SB_BUTTON_HEIGHT
        );
        draw_bevel(
          SB_BUTTON_BEVEL_X,
          height - SB_BUTTON_SINGLE_HEIGHT,
          SB_BUTTON_WIDTH,
          SB_BUTTON_HEIGHT
        );

        src = m_state == state::up ? m_up_arrow_hi : m_up_arrow;
        ::XCopyArea(
          m_terminal->dpy,
          src,
          m_window,
          m_white_gc,
          0,
          0,
          ARROW_WIDTH,
          ARROW_HEIGHT,
          SB_BUTTON_FACE_X,
          height - SB_BUTTON_BOTH_HEIGHT + SB_BEVEL_WIDTH_UPPER_LEFT
        );

        src = m_state == state::down ? m_down_arrow_hi : m_down_arrow;
        ::XCopyArea(
          m_terminal->dpy,
          src,
          m_window,
          m_white_gc,
          0,
          0,
          ARROW_WIDTH,
          ARROW_HEIGHT,
          SB_BUTTON_FACE_X,
          height - SB_BUTTON_SINGLE_HEIGHT + SB_BEVEL_WIDTH_UPPER_LEFT
        );
      }

      return true;
    }

    void update_data()
    {
      m_beginning = 0;
      m_end = m_terminal->szHint.height - (SB_BUTTON_TOTAL_HEIGHT + SB_PADDING);
    }

  private:
    ::Pixmap render_pixmap(const char* const* data, int width, int height)
    {
      auto pixmap = ::XCreatePixmap(
        m_terminal->dpy,
        m_window,
        width,
        height,
        m_terminal->depth
      );

      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          const auto c = data[y][x];
          ::GC pointcolor;

          if (c == ' ' || c == 'w')
          {
            pointcolor = m_white_gc;
          }
          else if (c == '.' || c == 'l')
          {
            pointcolor = m_gray_gc;
          }
          else if (c == '%' || c == 'd')
          {
            pointcolor = m_dark_gc;
          } else {
            pointcolor = m_black_gc;
          }
          ::XDrawPoint(m_terminal->dpy, pixmap, pointcolor, x, y);
        }
      }

      return pixmap;
    }

    void draw_bevel(int x1, int y1, int w, int h)
    {
      int x2;
      int y2;
      auto d = m_window;
      auto dpy = m_terminal->dpy;

      // Right point.
      x2 = x1 + w - 1;
      y2 = y1 + h - 1;

      // White top and left.
      ::XDrawLine(dpy, d, m_white_gc, x1, y1, x2, y1);
      ::XDrawLine(dpy, d, m_white_gc, x1, y1, x1, y2);

      // Black bottom and right.
      ::XDrawLine(dpy, d, m_black_gc, x1, y2, x2, y2);
      ::XDrawLine(dpy, d, m_black_gc, x2, y1, x2, y2);

      // dark inside bottom and right.
      x1++, y1++, x2--, y2--;	// Move in one point.
      ::XDrawLine(dpy, d, m_dark_gc, x1, y2, x2, y2);
      ::XDrawLine(dpy, d, m_dark_gc, x2, y1, x2, y2);
    }

  private:
    ::GC m_black_gc;
    ::GC m_white_gc;
    ::GC m_gray_gc;
    ::GC m_dark_gc;
    ::GC m_stipple_gc;
    ::Pixmap m_dimple;
    ::Pixmap m_up_arrow;
    ::Pixmap m_down_arrow;
    ::Pixmap m_up_arrow_hi;
    ::Pixmap m_down_arrow_hi;
    bool m_last_has_sb;
  };
#endif /* NEXT_SCROLLBAR */
#if defined(RXVT_SCROLLBAR)
  class rxvt_scrollbar : public raxvt::scrollbar
  {
  public:
    explicit rxvt_scrollbar(rxvt_term* terminal)
      : raxvt::scrollbar(terminal)
      , m_scrollbar_gc(0UL)
      , m_top_shadow_gc(0UL)
      , m_bot_shadow_gc(0UL)
    {
      if (!terminal->get_option(Opt_scrollBar_floating))
      {
        m_shadow = SHADOW_WIDTH;
      }
    }

    ~rxvt_scrollbar()
    {
      if (m_top_shadow_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_top_shadow_gc);
      }
      if (m_bot_shadow_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_bot_shadow_gc);
      }
      if (m_scrollbar_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_scrollbar_gc);
      }
    }

    enum style style() const
    {
      return style::rxvt;
    }

    int color() const
    {
      return m_shadow ? Color_trough : Color_border;
    }

    bool is_up_button_available(int y) const
    {
      return y < m_beginning;
    }

    bool is_down_button_available(int y) const
    {
      return y > m_end;
    }

  protected:
    void initialize()
    {
      ::XGCValues gcvalue;

      gcvalue.foreground = m_terminal->lookup_color(
        Color_topShadow,
        m_terminal->pix_colors
      );
      m_top_shadow_gc = ::XCreateGC(
        m_terminal->dpy,
        m_terminal->vt,
        GCForeground,
        &gcvalue
      );
      gcvalue.foreground = m_terminal->lookup_color(
        Color_bottomShadow,
        m_terminal->pix_colors
      );
      m_bot_shadow_gc = ::XCreateGC(
        m_terminal->dpy,
        m_terminal->vt,
        GCForeground,
        &gcvalue
      );
      gcvalue.foreground = m_terminal->lookup_color(
        m_terminal->depth <= 2 ? Color_fg : Color_scroll,
        m_terminal->pix_colors
      );
      m_scrollbar_gc = ::XCreateGC(
        m_terminal->dpy,
        m_terminal->vt,
        GCForeground,
        &gcvalue
      );
    }

    bool render(bool update)
    {
      int sbwidth = m_width;

      if (update)
      {
        if (m_last_top < m_top)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            m_shadow,
            m_last_top,
            sbwidth,
            m_top - m_last_top,
            False
          );
        }
        if (m_bottom < m_last_bottom)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            m_shadow,
            m_bottom,
            sbwidth,
            m_last_bottom - m_bottom,
            False
          );
        }
      } else {
        ::XClearWindow(m_terminal->dpy, m_window);
      }

      // Scrollbar slider.
#ifdef SB_BORDER
      {
        int xofs;

        if (m_terminal->option(Opt_scrollBar_right))
        {
          xofs = 0;
        } else {
          xofs = m_shadow ? sbwidth : sbwidth - 1;
        }

        ::XDrawLine(
          m_terminal->dpy,
          m_window,
          m_bot_shadow_gc,
          xofs,
          0,
          xofs,
          end + sbwidth
        );
      }
#endif

      ::XFillRectangle(
        m_terminal->dpy,
        m_window,
        m_scrollbar_gc,
        m_shadow,
        m_top,
        sbwidth,
        m_bottom - m_top
      );

      // Trough shadow.
      if (m_shadow)
      {
        draw_shadow(
          0,
          0,
          sbwidth + 2 * m_shadow,
          m_end + (sbwidth + 1) + m_shadow
        );
      }

      // shadow for scrollbar slider.
      draw_shadow(m_shadow, m_top, sbwidth, m_bottom - m_top);

      // Redraw scrollbar arrows.
      draw_button(m_shadow, m_shadow, UP);
      draw_button(m_shadow, m_end + 1,  DN);

      return true;
    }

    void update_data()
    {
      m_beginning = (m_window + 1) + m_shadow;
      m_end = m_terminal->szHint.height - m_beginning;
    }

  private:
    void draw_shadow(int x, int y, int w, int h)
    {
      int shadow = (w == 0 || h == 0) ? 1 : SHADOW_WIDTH;
      auto d = m_window;
      auto dpy = m_terminal->dpy;

      w += x - 1;
      h += y - 1;

      for (; shadow-- > 0; ++x, ++y, --w, --h)
      {
        ::XDrawLine(dpy, d, m_top_shadow_gc, x, y, w, y);
        ::XDrawLine(dpy, d, m_top_shadow_gc, x, y, x, h);
        ::XDrawLine(dpy, d, m_bot_shadow_gc, w, h, w, y + 1);
        ::XDrawLine(dpy, d, m_bot_shadow_gc, w, h, x + 1, h);
      }
    }

    /**
     * Draw triangular button with a shadow of 2 pixels.
     */
    void draw_button(int x, int y, int dirn)
    {
      ::XPoint pt[3];
      ::GC top;
      ::GC bot;
      auto d = m_window;
      auto dpy = m_terminal->dpy;
      unsigned int sz = m_width;
      unsigned int sz2 = sz / 2;

      if ((dirn == UP && m_state == state::up)
          || (dirn == DN && m_state == state::down))
      {
        top = m_bot_shadow_gc;
        bot = m_top_shadow_gc;
      } else {
        top = m_top_shadow_gc;
        bot = m_bot_shadow_gc;
      }

      // Fill triangle.
      pt[0].x = x;
      pt[1].x = x + sz - 1;
      pt[2].x = x + sz2;

      if (dirn == UP)
      {
        pt[0].y = pt[1].y = y + sz - 1;
        pt[2].y = y;
      } else {
        pt[0].y = pt[1].y = y;
        pt[2].y = y + sz - 1;
      }

      ::XFillPolygon(
        dpy,
        d,
        m_scrollbar_gc,
        pt,
        3,
        Convex,
        CoordModeOrigin
      );

      // Draw base.
      ::XDrawLine(
        dpy,
        d,
        dirn == UP ? bot : top,
        pt[0].x,
        pt[0].y,
        pt[1].x,
        pt[1].y
      );

      // Draw shadow on left.
      pt[1].x = x + sz2 - 1;
      pt[1].y = y + (dirn == UP ? 0 : sz - 1);
      ::XDrawLine(
        dpy,
        d,
        top,
        pt[0].x,
        pt[0].y,
        pt[1].x,
        pt[1].y
      );

#if SHADOW_WIDTH > 1
      /* doubled */
      pt[0].x++;

      if (dirn == UP)
      {
        --pt[0].y;
        ++pt[1].y;
      } else {
        ++pt[0].y;
        --pt[1].y;
      }

      ::XDrawLine(
        dpy,
        d,
        top,
        pt[0].x,
        pt[0].y,
        pt[1].x,
        pt[1].y
      );
#endif

      // Draw shadow on right.
      pt[1].x = x + sz - 1;
      pt[1].y = y + (dirn == UP ? sz - 1 : 0);
      pt[2].y = y + (dirn == UP ? 0 : sz - 1);
      ::XDrawLine(
        dpy,
        d,
        bot,
        pt[2].x,
        pt[2].y,
        pt[1].x,
        pt[1].y
      );

#if SHADOW_WIDTH > 1
      // doubled.
      --pt[1].x;
      if (dirn == UP)
      {
        ++pt[2].y;
        --pt[1].y;
      } else {
        --pt[2].y;
        ++pt[1].y;
      }

      ::XDrawLine(
        dpy,
        d,
        bot,
        pt[2].x,
        pt[2].y,
        pt[1].x,
        pt[1].y
      );
#endif
    }

  private:
    ::GC m_scrollbar_gc;
    ::GC m_top_shadow_gc;
    ::GC m_bot_shadow_gc;
  };
#endif /* RXVT_SCROLLBAR */
#if defined(XTERM_SCROLLBAR)
  static const unsigned char x_stp_bits[] {0xaa, 0x55};
  static const int x_stp_width = 8;
  static const int x_stp_height = 2;

  class xterm_scrollbar : public raxvt::scrollbar
  {
  public:
    explicit xterm_scrollbar(rxvt_term* terminal)
      : raxvt::scrollbar(terminal)
      , m_xscrollbar_gc(0UL)
      , m_shadow_gc(0UL)
    {
      m_width = SB_WIDTH_XTERM;
    }

    ~xterm_scrollbar()
    {
      if (m_xscrollbar_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_xscrollbar_gc);
      }
      if (m_shadow_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_shadow_gc);
      }
    }

    enum style style() const
    {
      return style::xterm;
    }

  protected:
    void initialize()
    {
      ::XGCValues gcvalue;

      gcvalue.stipple = ::XCreateBitmapFromData(
        m_terminal->dpy,
        m_window,
        reinterpret_cast<const char*>(x_stp_bits),
        x_stp_width,
        x_stp_height
      );
      if (!gcvalue.stipple)
      {
        rxvt_fatal("can't create bitmap\n");
      }

      gcvalue.fill_style = FillOpaqueStippled;
      gcvalue.foreground = m_terminal->lookup_color(
        Color_scroll,
        m_terminal->pix_colors_focused
      );
      gcvalue.background = m_terminal->lookup_color(
        Color_bg,
        m_terminal->pix_colors_focused
      );

      m_xscrollbar_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground | GCBackground | GCFillStyle | GCStipple,
        &gcvalue
      );
      gcvalue.foreground = m_terminal->lookup_color(
        Color_border,
        m_terminal->pix_colors_focused
      );
      m_shadow_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground,
        &gcvalue
      );
    }

    bool render(bool update)
    {
      const int xsb = m_terminal->get_option(Opt_scrollBar_right) ? 1 : 0;
      const int sbwidth = m_width - 1;

      if (update)
      {
        if (m_last_top < m_top)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            xsb,
            m_last_top,
            sbwidth,
            m_top - m_last_top,
            False
          );
        }
        if (m_bottom < m_last_bottom)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            xsb,
            m_bottom,
            sbwidth,
            m_last_bottom - m_bottom,
            False
          );
        }
      } else {
        ::XClearWindow(m_terminal->dpy, m_window);
      }

      // Scrollbar slider.
      ::XFillRectangle(
        m_terminal->dpy,
        m_window,
        m_xscrollbar_gc,
        xsb + 1,
        m_top,
        sbwidth - 2,
        m_bottom - m_top
      );

      ::XDrawLine(
        m_terminal->dpy,
        m_window,
        m_shadow_gc,
        xsb ? 0 : sbwidth,
        m_beginning,
        xsb ? 0 : sbwidth,
        m_end
      );

      return true;
    }

    void update_data()
    {
      m_beginning = 0;
      m_end = m_terminal->szHint.height;
    }

  private:
    ::GC m_xscrollbar_gc;
    ::GC m_shadow_gc;
  };
#endif /* XTERM_SCROLLBAR */
#if defined(PLAIN_SCROLLBAR)
  class plain_scrollbar : public raxvt::scrollbar
  {
  public:
    explicit plain_scrollbar(rxvt_term* terminal)
      : raxvt::scrollbar(terminal)
      , m_pscrollbar_gc(0UL)
    {
      m_width = SB_WIDTH_PLAIN;
    }

    ~plain_scrollbar()
    {
      if (m_pscrollbar_gc)
      {
        ::XFreeGC(m_terminal->dpy, m_pscrollbar_gc);
      }
    }

    enum style style() const
    {
      return style::plain;
    }

  protected:
    void initialize()
    {
      ::XGCValues gcvalue;

      gcvalue.foreground = m_terminal->lookup_color(
        Color_scroll,
        m_terminal->pix_colors_focused
      );
      m_pscrollbar_gc = ::XCreateGC(
        m_terminal->dpy,
        m_window,
        GCForeground,
        &gcvalue
      );
    }

    bool render(bool update)
    {
      const int xsb = m_terminal->get_option(Opt_scrollBar_right) ? 1 : 0;
      const int sbwidth = m_width - 1;

      if (update)
      {
        if (m_last_top < m_top)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            0,
            m_last_top,
            sbwidth + 1,
            m_top - m_last_top,
            False
          );
        }
        if (m_bottom < m_last_bottom)
        {
          ::XClearArea(
            m_terminal->dpy,
            m_window,
            0,
            m_bottom,
            sbwidth + 1,
            m_last_bottom - m_bottom,
            False
          );
        }
      } else {
        ::XClearWindow(m_terminal->dpy, m_window);
      }

      // scrollbar slider.
      ::XFillRectangle(
        m_terminal->dpy,
        m_window,
        m_pscrollbar_gc,
        1 - xsb,
        m_top,
        sbwidth,
        m_bottom - m_top
      );

      return true;
    }

    void update_data()
    {
      m_beginning = 0;
      m_end = m_terminal->szHint.height;
    }

  private:
    ::GC m_pscrollbar_gc;
  };
#endif
}

namespace raxvt
{
  std::shared_ptr<scrollbar> scrollbar::make(rxvt_term* terminal)
  {
    const char* scrollstyle = terminal->get_setting(Rs_scrollstyle);
    enum style style;
    std::shared_ptr<scrollbar> instance;

#if defined(RXVT_SCROLLBAR)
    style = style::rxvt;
#elif defined(XTERM_SCROLLBAR)
    style = style::xterm;
#elif defined(NEXT_SCROLLBAR)
    style = style::next;
#elif defined(PLAIN_SCROLLBAR)
    style = style::plain;
#else
    style = style::rxvt;
#endif
    if (scrollstyle)
    {
#if defined(NEXT_SCROLLBAR)
      if (!std::strcmp(scrollstyle, "next"))
      {
        style = style::next;
      }
#endif
#if defined(XTERM_SCROLLBAR)
      if (!std::strcmp(scrollstyle, "xterm"))
      {
        style = style::xterm;
      }
#endif
#if defined(PLAIN_SCROLLBAR)
      if (!std::strcmp(scrollstyle, "plain"))
      {
        style = style::plain;
      }
#endif
#if defined(RXVT_SCROLLBAR)
      if (!std::strcmp(scrollstyle, "rxvt"))
      {
        style = style::rxvt;
      }
#endif
    }

    switch (style)
    {
#if defined(NEXT_SCROLLBAR)
      case style::next:
        instance = std::make_shared<next_scrollbar>(terminal);
        break;
#endif

#if defined(XTERM_SCROLLBAR)
      case style::xterm:
        instance = std::make_shared<xterm_scrollbar>(terminal);
        break;
#endif

#if defined(PLAIN_SCROLLBAR)
      case style::plain:
        instance = std::make_shared<plain_scrollbar>(terminal);
        break;
#endif

#if defined(RXVT_SCROLLBAR)
      case style::rxvt:
        instance = std::make_shared<rxvt_scrollbar>(terminal);
        break;
#endif
    }

    return instance;
  }

  scrollbar::scrollbar(rxvt_term* terminal)
    : m_terminal(terminal)
    , m_state(state::off)
    , m_beginning(0)
    , m_end(0)
    , m_top(0)
    , m_bottom(0)
    , m_width(SB_WIDTH_RXVT)
    , m_shadow(0)
    , m_last_bottom(0)
    , m_last_top(0)
    , m_last_state(state::off)
    , m_alignment(alignment::centre)
    , m_window(0UL)
    , m_leftptr_cursor(::XCreateFontCursor(terminal->dpy, XC_left_ptr))
  {
    const char* scrollalign = terminal->get_setting(Rs_scrollBar_align);

    if (scrollalign)
    {
      if (!std::strcmp(scrollalign, "top"))
      {
        m_alignment = alignment::top;
      }
      else if (!std::strcmp(scrollalign, "bottom"))
      {
        m_alignment = alignment::bottom;
      }
    }
  }

  scrollbar::~scrollbar() {}

  int
  scrollbar::color() const
  {
    return Color_border;
  }

  int
  scrollbar::min_height() const
  {
    return 10;
  }

  void
  scrollbar::map(bool map)
  {
    if (map)
    {
      m_state = state::idle;
      if (m_window)
      {
        ::XMapWindow(m_terminal->dpy, m_window);
      } else {
        resize();
      }
    } else {
      m_state = state::off;
      if (m_window)
      {
        ::XUnmapWindow(m_terminal->dpy, m_window);
      }
    }
  }

  void
  scrollbar::resize()
  {
    bool delayed_init = false;
    int window_sb_x = 0;

    if (m_terminal->get_option(Opt_scrollBar_right))
    {
      window_sb_x = m_terminal->szHint.width - total_width();
    }

    update_data();

    if (!m_window)
    {
      // Create the scrollbar window.
      m_window = ::XCreateSimpleWindow(
        m_terminal->dpy,
        m_terminal->parent,
        window_sb_x,
        0,
        total_width(),
        m_terminal->szHint.height,
        0,
        m_terminal->lookup_color(Color_fg, m_terminal->pix_colors),
        m_terminal->lookup_color(color(), m_terminal->pix_colors)
      );
      ::XDefineCursor(m_terminal->dpy, m_window, m_leftptr_cursor);
      ::XSelectInput(
        m_terminal->dpy,
        m_window,
        ExposureMask
        | ButtonPressMask
        | ButtonReleaseMask
        | Button1MotionMask
        | Button2MotionMask
        | Button3MotionMask
      );
      m_terminal->scrollbar_ev.start(m_terminal->display, m_window);
      initialize();
      delayed_init = true;
    } else {
      ::XMoveResizeWindow(
        m_terminal->dpy,
        m_window,
        window_sb_x,
        0,
        total_width(),
        m_terminal->szHint.height
      );
    }

    show(true);

    if (delayed_init)
    {
      ::XMapWindow(m_terminal->dpy, m_window);
    }
  }

  bool
  scrollbar::show(bool refresh)
  {
    bool result;

    if (m_state == state::off)
    {
      return false;
    }
    if (refresh)
    {
      const auto top = m_terminal->view_start - m_terminal->top_row;
      const auto bot = top + (m_terminal->nrow - 1);
      const auto len = std::max<int>(m_terminal->nrow - 1 - m_terminal->top_row, 1);
      const auto n = std::min<int>(min_height(), size());

      m_top = m_beginning + (top * (size() - n)) / len;
      m_bottom = m_top + ecb_div_ru((bot - top) * (size() - n), len) + n;
      // Test if change is necessary or not.
      if (m_top == m_last_top
          && m_bottom == m_last_bottom
          && (m_state == m_last_state
            || !(m_state == state::up || m_state == state::down)))
      {
        return false;
      }
    }

    result = render(refresh);
    m_last_top = m_top;
    m_last_bottom = m_bottom;
    m_last_state = m_state;

    return result;
  }

  bool
  scrollbar::is_up_button_available(int) const
  {
    return false;
  }

  bool
  scrollbar::is_down_button_available(int) const
  {
    return false;
  }

  void
  scrollbar::update_data() {}
}
