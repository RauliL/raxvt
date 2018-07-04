/*----------------------------------------------------------------------*
 * File:	rxvttoolkit.h - provide toolkit-functionality for rxvt.
 *----------------------------------------------------------------------*
 *
 * All portions of code are copyright by their respective author/s.
 * Copyright (c) 2003-2011 Marc Lehmann <schmorp@schmorp.de>
 * Copyright (c) 2011      Emanuele Giaquinta <e.giaquinta@glauco.it>
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

#ifndef RXVT_TOOLKIT_H
#define RXVT_TOOLKIT_H

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "ev_cpp.h"

#include "rxvtutil.h"

// see rxvttoolkit.C:xa_names, which must be kept in sync
enum {
  XA_TEXT,
  XA_COMPOUND_TEXT,
  XA_UTF8_STRING,
  XA_MULTIPLE,
  XA_TARGETS,
  XA_TIMESTAMP,
  XA_VT_SELECTION,
  XA_INCR,
  XA_WM_PROTOCOLS,
  XA_WM_DELETE_WINDOW,
  XA_CLIPBOARD,
  XA_AVERAGE_WIDTH,
  XA_WEIGHT_NAME,
  XA_SLANT,
  XA_CHARSET_REGISTRY,
  XA_CHARSET_ENCODING,
#if ENABLE_FRILLS
  XA_MOTIF_WM_HINTS,
#endif
#if ENABLE_EWMH
  XA_NET_WM_PID,
  XA_NET_WM_NAME,
  XA_NET_WM_ICON_NAME,
  XA_NET_WM_PING,
  XA_NET_WM_ICON,
#endif
#if USE_XIM
  XA_WM_LOCALE_NAME,
  XA_XIM_SERVERS,
#endif
#if HAVE_IMG || ENABLE_PERL
  XA_XROOTPMAP_ID,
  XA_ESETROOT_PMAP_ID,
#endif
#if ENABLE_XEMBED
  XA_XEMBED,
  XA_XEMBED_INFO,
#endif
#if !ENABLE_MINIMAL
  // these are usually allocated by other subsystems, but we do it
  // here to avoid a server roundtrip.
  XA_SCREEN_RESOURCES,
  XA_XDCCC_LINEAR_RGB_CORRECTION,
  XA_XDCCC_LINEAR_RGB_MATRICES,
  XA_WM_COLORMAP_WINDOWS,
  XA_WM_STATE,
  XA_cursor,
# if USE_XIM
  // various selection targets used by XIM
  XA_TRANSPORT,
  XA_LOCALES,
  XA__XIM_PROTOCOL,
  XA__XIM_XCONNECT,
  XA__XIM_MOREDATA,
# endif
#endif
  NUM_XA
};

struct rxvt_term;

namespace raxvt
{
  struct display;
}

struct im_watcher;
struct xevent_watcher;

struct rxvt_watcher
{
  int active; /* 0 == inactive, else index into respective vector */

  bool is_active () { return active; }

  rxvt_watcher () : active (0) { }
};

struct refcounted
{
  int referenced;
  char *id;

  refcounted (const char *id);
  bool ref_init () { return false; }
  void ref_next () { }
  ~refcounted ();
};

template<class T>
struct refcache : vector<T*>
{
public:
  ~refcache()
  {
    clear();
  }

  T* get(const char* id)
  {
    for (T** i = this->begin(); i < this->end(); ++i)
    {
      if (!std::strcmp(id, (*i)->id))
      {
        ++(*i)->referenced;
        (*i)->ref_next();

        return *i;
      }
    }

    T* obj = new T(id);

    if (obj && obj->ref_init())
    {
      obj->referenced = 1;
      this->push_back(obj);

      return obj;
    }

    delete obj;

    return nullptr;
  }

  void put(T* obj)
  {
    if (!obj)
    {
      return;
    }
    if (!--obj->referenced)
    {
      this->erase(find(this->begin(), this->end(), obj));
      delete obj;
    }
  }

  void clear()
  {
    while (this->size())
    {
      put(*this->begin());
    }
  }
};

/////////////////////////////////////////////////////////////////////////////

struct rxvt_screen;

struct rxvt_drawable
{
  rxvt_screen *screen;
  Drawable drawable;
  operator Drawable() { return drawable; }

  XftDraw *xftdrawable;
  operator XftDraw *();

  rxvt_drawable (rxvt_screen *screen, Drawable drawable)
  : screen(screen),
    drawable(drawable),
    xftdrawable(nullptr) {}

  ~rxvt_drawable ();
};

/////////////////////////////////////////////////////////////////////////////

#if USE_XIM
struct rxvt_xim : refcounted
{
  void destroy ();
  raxvt::display* display;

//public
  XIM xim;

  rxvt_xim (const char *id) : refcounted (id) { }
  bool ref_init ();
  ~rxvt_xim ();
};
#endif

struct rxvt_screen
{
  raxvt::display* display;
  Display *dpy;
  int depth;
  Visual *visual;
  Colormap cmap;

  // scratch pixmap
  rxvt_drawable *scratch_area;
  int scratch_w, scratch_h;

  rxvt_drawable &scratch_drawable (int w, int h);

  rxvt_screen ();

  void set(raxvt::display* disp);
  void select_visual (int id);
  void select_depth (int bitdepth); // select visual by depth
  void clear ();
};

#if USE_XIM
struct im_watcher : rxvt_watcher
{
  std::function<void(void)> callback;

  void start(raxvt::display* display);
  void stop(raxvt::display* display);
};
#endif

struct xevent_watcher : rxvt_watcher
{
  std::function<void(XEvent&)> callback;
  Window window;

  void start(raxvt::display* display, Window window);
  void stop(raxvt::display* display);
};

/////////////////////////////////////////////////////////////////////////////

typedef unsigned long Pixel;

struct rgba
{
  uint16_t r, g, b, a;

  enum { MIN_CC = 0x0000, MAX_CC  = 0xffff };

  rgba ()
  { }

  rgba (uint16_t r, uint16_t g, uint16_t b, uint16_t a = MAX_CC)
  : r(r), g(g), b(b), a(a)
  { }
};

struct rxvt_color
{
  XftColor c;

  operator Pixel () const { return c.pixel; }

  bool operator == (const rxvt_color &b) const { return Pixel (*this) == Pixel (b); }
  bool operator != (const rxvt_color &b) const { return Pixel (*this) != Pixel (b); }

  bool is_opaque () const
  {
    return c.color.alpha == rgba::MAX_CC;
  }

  bool alloc (rxvt_screen *screen, const rgba &color);
  void free (rxvt_screen *screen);

  operator rgba () const
  {
    rgba c;
    get (c);
    return c;
  }
  void get (rgba &color) const;
  void get (XColor &color) const;

  bool set (rxvt_screen *screen, const char *name);
  bool set (rxvt_screen *screen, const rgba &color);

  void fade (rxvt_screen *screen, int percent, rxvt_color &result, const rgba &to = rgba (0, 0, 0));
};

#define Sel_normal              0x01    /* normal selection */
#define Sel_incr                0x02    /* incremental selection */
#define Sel_Primary             0x01
#define Sel_Secondary           0x02
#define Sel_Clipboard           0x03
#define Sel_whereMask           0x0f
#define Sel_CompoundText        0x10    /* last request was COMPOUND_TEXT */
#define Sel_UTF8String          0x20    /* last request was UTF8_STRING */

struct rxvt_selection
{
  rxvt_selection(raxvt::display* disp,
                 int selnum,
                 Time tm,
                 Window win,
                 Atom prop,
                 rxvt_term* term);
  void run ();
  ~rxvt_selection ();

  rxvt_term *term; // terminal to paste to, may be 0
  void *cb_sv;     // managed by perl

  raxvt::display* display;
  Time request_time;
  Window request_win;
  Atom request_prop;

private:
  unsigned char selection_wait;
  unsigned char selection_type;

  char *incr_buf;
  size_t incr_buf_size, incr_buf_fill;

  void timer_cb (ev::timer &w, int revents); ev::timer timer_ev;
  void x_cb (XEvent &xev); xevent_watcher x_ev;

  void finish (char *data = 0, unsigned int len = 0);
  void stop ();
  bool request (Atom target, int selnum);
  void handle_selection (Window win, Atom prop, bool delete_prop);
};

#endif

