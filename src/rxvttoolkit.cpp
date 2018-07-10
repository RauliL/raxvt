/*----------------------------------------------------------------------*
 * File:	rxvttoolkit.C
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

#include "../config.h"
#include <rxvt.h>
#include <rxvttoolkit.h>

#include "raxvt/display.hpp"

#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <X11/extensions/Xrender.h>

/////////////////////////////////////////////////////////////////////////////

refcounted::refcounted (const char *id)
{
  this->id = strdup (id);
}

refcounted::~refcounted ()
{
  free (id);
}

/////////////////////////////////////////////////////////////////////////////

#if USE_XIM

static void
#if XIMCB_PROTO_BROKEN
im_destroy_cb (XIC unused1, XPointer client_data, XPointer unused3)
#else
im_destroy_cb (XIM unused1, XPointer client_data, XPointer unused3)
#endif
{
  rxvt_xim *xim = (rxvt_xim *)client_data;
  raxvt::display* display = xim->display;

  xim->xim = 0;

  display->remove_xim(xim);
  display->im_change_cb();
}

bool
rxvt_xim::ref_init ()
{
  display = GET_R->display; //HACK: TODO

  xim = XOpenIM(display->dpy(), 0, 0, 0);

  if (!xim)
    return false;

  XIMCallback ximcallback;
  ximcallback.client_data = (XPointer)this;
  ximcallback.callback = im_destroy_cb;

  XSetIMValues (xim, XNDestroyCallback, &ximcallback, (char *)0);

  return true;
}

rxvt_xim::~rxvt_xim ()
{
  if (xim)
    XCloseIM (xim);
}

#endif

/////////////////////////////////////////////////////////////////////////////

rxvt_drawable::~rxvt_drawable ()
{
  if (xftdrawable)
    XftDrawDestroy (xftdrawable);
}

rxvt_drawable::operator XftDraw *()
{
  if (!xftdrawable)
    xftdrawable = XftDrawCreate (screen->dpy, drawable, screen->visual, screen->cmap);

  return xftdrawable;
}

/////////////////////////////////////////////////////////////////////////////

// not strictly necessary as it is only used with superclass of zero_initialised
rxvt_screen::rxvt_screen ()
: scratch_area (0)
{
}

rxvt_drawable& rxvt_screen::scratch_drawable(int w, int h)
{
  if (!scratch_area || w > scratch_w || h > scratch_h)
  {
    if (scratch_area)
    {
      XFreePixmap(dpy, scratch_area->drawable);
      delete scratch_area;
    }

    Pixmap pm = XCreatePixmap(
      dpy,
      RootWindowOfScreen(ScreenOfDisplay(dpy, display->screen())),
      scratch_w = w,
      scratch_h = h,
      depth
    );

    scratch_area = new rxvt_drawable (this, pm);
  }

  return *scratch_area;
}

void
rxvt_screen::set(raxvt::display* disp)
{
  display = disp;
  dpy = disp->dpy();

  Screen *screen = ScreenOfDisplay (dpy, disp->screen());

  depth   = DefaultDepthOfScreen    (screen);
  visual  = DefaultVisualOfScreen   (screen);
  cmap    = DefaultColormapOfScreen (screen);
}

#if ENABLE_FRILLS

void
rxvt_screen::select_visual(int id)
{
  XVisualInfo vinfo;
  int n;

  vinfo.visualid = id;

  if (auto vi = XGetVisualInfo(dpy, VisualIDMask, &vinfo, &n))
  {
    depth = vi->depth;
    visual = vi->visual;

    XFree(vi);

    cmap = XCreateColormap(dpy, display->root(), visual, AllocNone);
  } else {
    rxvt_warn(
      "no visual found for requested id 0x%02x, using default visual.\n",
      id
    );
  }
}

void
rxvt_screen::select_depth(int bitdepth)
{
  XVisualInfo vinfo;

  if (XMatchVisualInfo(dpy, display->screen(), bitdepth, TrueColor, &vinfo))
  {
    select_visual(vinfo.visualid);
  } else {
    rxvt_warn(
      "no visual found for requested depth %d, using default visual.\n",
      bitdepth
    );
  }
}

#endif

void
rxvt_screen::clear()
{
  if (scratch_area)
  {
    XFreePixmap(dpy, scratch_area->drawable);
    delete scratch_area;
  }

  if (cmap != DefaultColormapOfScreen(ScreenOfDisplay(dpy, display->screen())))
  {
    XFreeColormap(dpy, cmap);
  }
}

/////////////////////////////////////////////////////////////////////////////

#if defined(USE_XIM)
void
im_watcher::start(raxvt::display* display)
{
  display->reg(this);
}

void
im_watcher::stop(raxvt::display* display)
{
  display->unreg(this);
}
#endif

void
xevent_watcher::start(raxvt::display* display, Window window)
{
  this->window = window;
  display->reg(this);
}

void
xevent_watcher::stop(raxvt::display* display)
{
  display->unreg(this);
}

/////////////////////////////////////////////////////////////////////////////
//

static unsigned int
insert_component (unsigned int value, unsigned int mask, unsigned int shift)
{
  return (value * (mask + 1) >> 16) << shift;
}

bool
rxvt_color::alloc (rxvt_screen *screen, const rgba &color)
{
  //TODO: only supports 24 bit
  unsigned int alpha = color.a >= 0xff00 ? 0xffff : color.a;

  XRenderPictFormat *format;

  // not needed by XftColorAlloc, but by the other paths (ours
  // and fallback), so just set all components here.
  c.color.red   = color.r;
  c.color.green = color.g;
  c.color.blue  = color.b;
  c.color.alpha = alpha;

  // FUCKING Xft gets it wrong, of course, so work around it.
  // Transparency users should eat shit and die, and then
  // XRenderQueryPictIndexValues themselves plenty.
  if ((screen->visual->c_class == TrueColor)
      && (format = XRenderFindVisualFormat (screen->dpy, screen->visual)))
    {
      // the fun lies in doing everything manually...

      // Xft wants premultiplied alpha, but abuses the alpha channel
      // as blend factor, and doesn't allow us to set the alpha channel
      c.color.red   = c.color.red   * alpha / 0xffff;
      c.color.green = c.color.green * alpha / 0xffff;
      c.color.blue  = c.color.blue  * alpha / 0xffff;

      c.pixel = insert_component (c.color.red  , format->direct.redMask  , format->direct.red  )
              | insert_component (c.color.green, format->direct.greenMask, format->direct.green)
              | insert_component (c.color.blue , format->direct.blueMask , format->direct.blue )
              | insert_component (alpha        , format->direct.alphaMask, format->direct.alpha);

      return true;
    }
  else
    {
      XRenderColor d;

      d.red   = color.r;
      d.green = color.g;
      d.blue  = color.b;
      d.alpha = alpha;

      // XftColorAlloc always returns 100% transparent pixels(!)
      if (XftColorAllocValue (screen->dpy, screen->visual, screen->cmap, &d, &c))
        return true;
    }

  c.pixel = (color.r * 2 + color.g * 3 + color.b) >= 0x8000 * 6
          ? WhitePixelOfScreen (DefaultScreenOfDisplay (screen->dpy))
          : BlackPixelOfScreen (DefaultScreenOfDisplay (screen->dpy));

  return false;
}

bool
rxvt_color::set (rxvt_screen *screen, const char *name)
{
  rgba c;
  char eos;
  int skip = 0;

  c.a = rgba::MAX_CC;

  // parse the nonstandard "[alphapercent]" prefix
  if (1 <= sscanf (name, "[%hd]%n", &c.a, &skip))
    {
      c.a = lerp<int, int, int> (0, rgba::MAX_CC, c.a);
      name += skip;
    }

  // parse the non-standard "rgba:rrrr/gggg/bbbb/aaaa" format
  if (std::strlen(name) != 4+5*4 || 4 != std::sscanf(name, "rgba:%4hx/%4hx/%4hx/%4hx%c", &c.r, &c.g, &c.b, &c.a, &eos))
    {
      XColor xc;

      if (XParseColor (screen->dpy, screen->cmap, name, &xc))
        {
          c.r = xc.red;
          c.g = xc.green;
          c.b = xc.blue;
        }
      else
        {
          c.r = 0xffff;
          c.g = 0x6969;
          c.b = 0xb4b4;

          rxvt_warn ("unable to parse color '%s', using pink instead.\n", name);
        }
    }

  return set (screen, c);
}

bool
rxvt_color::set (rxvt_screen *screen, const rgba &color)
{
  bool got = alloc (screen, color);

#if !ENABLE_MINIMAL
  int cmap_size = screen->visual->map_entries;

  if (!got
      && screen->visual->c_class == PseudoColor
      && cmap_size < 4096)
    {
      XColor *colors = new XColor [screen->visual->map_entries];

      for (int i = 0; i < cmap_size; i++)
        colors [i].pixel = i;

      // many kilobytes transfer per colour, but pseudocolor isn't worth
      // many extra optimisations.
      XQueryColors (screen->dpy, screen->cmap, colors, cmap_size);

      while (cmap_size)
        {
          int diff = 0x7fffffffL;
          XColor *best = colors;

          for (int i = 0; i < cmap_size; i++)
            {
              // simple weighted rgb distance sucks, but keeps it simple
              int d = abs (color.r - colors [i].red  ) * 2
                    + abs (color.g - colors [i].green) * 3
                    + abs (color.b - colors [i].blue );

              if (d < diff)
                {
                  diff = d;
                  best = colors + i;
                }
            }

          //rxvt_warn ("could not allocate %04x %04x %04x, getting %04x %04x %04x instead (%d,%d)\n",
          //    color.r, color.g, color.b, best->red, best->green, best->blue, diff, best - colors);

          got = alloc (screen, rgba (best->red, best->green, best->blue));

          if (got)
            break;

          *best = colors [--cmap_size];
        }

      delete [] colors;
    }
#endif

  return got;
}

void
rxvt_color::get (rgba &color) const
{

  color.r = c.color.red;
  color.g = c.color.green;
  color.b = c.color.blue;
  color.a = c.color.alpha;

  if (IN_RANGE_INC (color.a, 0x0001, 0xfffe))
    {
      color.r = color.r * 0xffff / color.a;
      color.g = color.g * 0xffff / color.a;
      color.b = color.b * 0xffff / color.a;
    }
}

void
rxvt_color::get (XColor &color) const
{
  rgba c;
  get (c);

  color.red   = c.r;
  color.green = c.g;
  color.blue  = c.b;
  color.pixel = (Pixel)*this;
}

void
rxvt_color::free (rxvt_screen *screen)
{
  if (screen->visual->c_class == TrueColor)
    return; // nothing to do

  XftColorFree (screen->dpy, screen->visual, screen->cmap, &c);
}

void
rxvt_color::fade (rxvt_screen *screen, int percent, rxvt_color &result, const rgba &to)
{
  rgba c;
  get (c);

  result.set (
    screen,
    rgba (
      lerp (c.r, to.r, percent),
      lerp (c.g, to.g, percent),
      lerp (c.b, to.b, percent),
      lerp (c.a, to.a, percent)
    )
  );
}

rxvt_selection::rxvt_selection(raxvt::display* disp,
                               int selnum,
                               Time tm,
                               Window win,
                               Atom prop,
                               rxvt_term* term)
: display (disp), request_time (tm), request_win (win), request_prop (prop), term (term)
{
  assert (selnum >= Sel_Primary && selnum <= Sel_Clipboard);

  timer_ev.set<rxvt_selection, &rxvt_selection::timer_cb> (this);
  timer_ev.repeat = 10.;
  x_ev.callback = std::bind(&rxvt_selection::x_cb, this, std::placeholders::_1);

  incr_buf = 0;
  incr_buf_size = incr_buf_fill = 0;
  selection_wait = Sel_normal;
  selection_type = selnum;
  cb_sv = 0;
}

void
rxvt_selection::stop ()
{
  free (incr_buf);
  incr_buf = 0;
  timer_ev.stop ();
  x_ev.stop (display);
}

rxvt_selection::~rxvt_selection ()
{
  stop ();
}

void
rxvt_selection::run ()
{
  int selnum = selection_type;

#if ENABLE_FRILLS
  if (selnum == Sel_Primary && display->selection_owner())
  {
    /* internal selection */
    auto str = rxvt_wcstombs(
      display->selection_owner()->selection.text,
      display->selection_owner()->selection.len
    );

    finish(str, std::strlen(str));
    std::free(str);
    return;
  }
#endif

#if X_HAVE_UTF8_STRING
  selection_type = Sel_UTF8String;
  if (request (display->xa()[XA_UTF8_STRING], selnum))
    return;
#else
  selection_type = Sel_CompoundText;
  if (request (display->xa()[XA_COMPOUND_TEXT], selnum))
    return;
#endif

  // fallback to CUT_BUFFER0 if the requested property has no owner
  handle_selection(display->root(), XA_CUT_BUFFER0, false);
}

void
rxvt_selection::finish (char *data, unsigned int len)
{
  if (!cb_sv)
    {
      if (data)
        term->paste (data, len);

      term->selection_req = 0;
      delete this;
    }
#if ENABLE_PERL
  else
    {
      stop (); // we do not really trust perl callbacks
      rxvt_perl.selection_finish (this, data, len);
    }
#endif
}

bool
rxvt_selection::request (Atom target, int selnum)
{
  Atom sel;

  selection_type |= selnum;

  if (selnum == Sel_Primary)
    sel = XA_PRIMARY;
  else if (selnum == Sel_Secondary)
    sel = XA_SECONDARY;
  else
    sel = display->xa()[XA_CLIPBOARD];

  if (XGetSelectionOwner(display->dpy(), sel) != None)
    {
      XConvertSelection(
        display->dpy(),
        sel,
        target,
        request_prop,
        request_win,
        request_time
      );
      x_ev.start (display, request_win);
      timer_ev.again ();
      return true;
    }

  return false;
}

void
rxvt_selection::handle_selection (Window win, Atom prop, bool delete_prop)
{
  auto dpy = display->dpy();
  char *data = 0;
  unsigned int data_len = 0;
  unsigned long bytes_after;
  XTextProperty ct;

  // check for failed XConvertSelection
  if (prop == None)
  {
    bool error = true;
    int selnum = selection_type & Sel_whereMask;

    if (selection_type & Sel_CompoundText)
    {
      selection_type = 0;
      error = !request(XA_STRING, selnum);
    }

    if (selection_type & Sel_UTF8String)
    {
      selection_type = Sel_CompoundText;
      error = !request(display->xa()[XA_COMPOUND_TEXT], selnum);
    }

    if (error)
    {
      ct.value = 0;
      goto bailout;
    }

    return;
  }

  // length == (2^31 - 1) / 4, as gdk
  if (XGetWindowProperty (dpy, win, prop,
                          0, 0x1fffffff,
                          delete_prop, AnyPropertyType,
                          &ct.encoding, &ct.format,
                          &ct.nitems, &bytes_after,
                          &ct.value) != Success)
    {
      ct.value = 0;
      goto bailout;
    }

  if (ct.encoding == None)
    goto bailout;

  if (ct.value == 0)
    goto bailout;

  if (ct.encoding == display->xa()[XA_INCR])
  {
    // INCR selection, start handshake
    if (!delete_prop)
    {
      XDeleteProperty(dpy, win, prop);
    }

    selection_wait = Sel_incr;
    timer_ev.again();

    goto bailout;
  }

  if (ct.nitems == 0)
  {
    if (selection_wait == Sel_incr)
    {
      XFree(ct.value);

      // finally complete, now paste the whole thing
      selection_wait = Sel_normal;
      ct.value = (unsigned char *)incr_buf;
      ct.nitems = incr_buf_fill;
      incr_buf = 0;
      timer_ev.stop();
    } else {
      // avoid recursion
      if (win != display->root() || prop != XA_CUT_BUFFER0)
      {
        XFree(ct.value);

         // fallback to CUT_BUFFER0 if the requested property
         // has an owner but is empty
        handle_selection(display->root(), XA_CUT_BUFFER0, False);
        return;
      }

      goto bailout;
    }
  }
  else if (selection_wait == Sel_incr)
  {
    timer_ev.again ();

    while (incr_buf_fill + ct.nitems > incr_buf_size)
    {
      incr_buf_size = incr_buf_size ? incr_buf_size * 2 : 128*1024;
      incr_buf = (char *)rxvt_realloc (incr_buf, incr_buf_size);
    }

    std::memcpy(incr_buf + incr_buf_fill, ct.value, ct.nitems);
    incr_buf_fill += ct.nitems;

    goto bailout;
  }

  char **cl;
  int cr;

  // we honour the first item only

#if !ENABLE_MINIMAL
  // xlib is horribly broken with respect to UTF8_STRING, and nobody cares to fix it
  // so recode it manually
  if (ct.encoding == display->xa()[XA_UTF8_STRING])
  {
    auto w = rxvt_utf8towcs((const char *)ct.value, ct.nitems);

    data = rxvt_wcstombs(w);
    std::free(w);
  }
  else
#endif
  if (XmbTextPropertyToTextList (dpy, &ct, &cl, &cr) >= 0 && cl)
  {
    data = strdup(cl[0]);
    XFreeStringList(cl);
  } else {
    // paste raw
    data = strdup ((const char *)ct.value);
  }

  data_len = std::strlen(data);

bailout:
  XFree (ct.value);

  if (selection_wait == Sel_normal)
    {
      finish (data, data_len);
      std::free(data);
    }
}

void
rxvt_selection::timer_cb (ev::timer &w, int revents)
{
  if (selection_wait == Sel_incr)
    rxvt_warn ("data loss: timeout on INCR selection paste, ignoring.\n");

  finish ();
}

void
rxvt_selection::x_cb (XEvent &xev)
{
  switch (xev.type)
    {
      case PropertyNotify:
        if (selection_wait == Sel_incr
            && xev.xproperty.atom == request_prop
            && xev.xproperty.state == PropertyNewValue)
          handle_selection (xev.xproperty.window, xev.xproperty.atom, true);
        break;

      case SelectionNotify:
        if (selection_wait == Sel_normal
            && xev.xselection.time == request_time)
          {
            timer_ev.stop ();
            handle_selection (xev.xselection.requestor, xev.xselection.property, true);
          }
        break;
    }
}
