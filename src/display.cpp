#include "../../config.h"

#include <algorithm>
#include <memory>
#include <unordered_map>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/utsname.h>

#include "rxvt.h"
#include "raxvt/display.hpp"

static const char *const xa_names[] =
{
  "TEXT",
  "COMPOUND_TEXT",
  "UTF8_STRING",
  "MULTIPLE",
  "TARGETS",
  "TIMESTAMP",
  "VT_SELECTION",
  "INCR",
  "WM_PROTOCOLS",
  "WM_DELETE_WINDOW",
  "CLIPBOARD",
  "AVERAGE_WIDTH",
  "WEIGHT_NAME",
  "SLANT",
  "CHARSET_REGISTRY",
  "CHARSET_ENCODING",
#if ENABLE_FRILLS
  "_MOTIF_WM_HINTS",
#endif
#if ENABLE_EWMH
  "_NET_WM_PID",
  "_NET_WM_NAME",
  "_NET_WM_ICON_NAME",
  "_NET_WM_PING",
  "_NET_WM_ICON",
#endif
#if USE_XIM
  "WM_LOCALE_NAME",
  "XIM_SERVERS",
#endif
#if HAVE_IMG || ENABLE_PERL
  "_XROOTPMAP_ID",
  "ESETROOT_PMAP_ID",
#endif
#if ENABLE_XEMBED
  "_XEMBED",
  "_XEMBED_INFO",
#endif
#if !ENABLE_MINIMAL
  "SCREEN_RESOURCES",
  "XDCCC_LINEAR_RGB_CORRECTION",
  "XDCCC_LINEAR_RGB_MATRICES",
  "WM_COLORMAP_WINDOWS",
  "WM_STATE",
  "cursor",
# if USE_XIM
  "TRANSPORT",
  "LOCALES",
  "_XIM_PROTOCOL",
  "_XIM_XCONNECT",
  "_XIM_MOREDATA",
# endif
#endif
};

namespace raxvt
{
  static std::unordered_map<std::string, std::unique_ptr<display>> cache;

  display*
  display::get(const std::string& id)
  {
    const auto entry = cache.find(id);
    display* disp;

    if (entry != std::end(cache))
    {
      disp = entry->second.get();
      // TODO: Somehow check whether the database files/resources changed
      // before affording re-loading/parsing.
      ::XrmDestroyDatabase(::XrmGetDatabase(disp->m_dpy));
#if XLIB_ILLEGAL_ACCESS
      // Work around a bug in XrmSetDatabase where it frees the database.
      disp->m_dpy->db = 0;
#endif
      ::XrmSetDatabase(disp->m_dpy, disp->get_resources(true));

      return disp;
    }

    disp = new display(id);

#if defined(LOCAL_X_IS_UNIX)
    if (id[0] == ':')
    {
      const std::string value = "unix/" + id;

      disp->m_dpy = ::XOpenDisplay(value.c_str());
    }
#endif

    if (!disp->m_dpy && !(disp->m_dpy = ::XOpenDisplay(id.c_str())))
    {
      delete disp;

      return nullptr;
    }

    disp->screen = DefaultScreen(disp->m_dpy);
    disp->root = DefaultRootWindow(disp->m_dpy);

    assert(ecb_array_length(xa_names) == NUM_XA);
    ::XInternAtoms(
      disp->m_dpy,
      const_cast<char**>(xa_names),
      NUM_XA,
      False,
      disp->xa
    );

    ::XrmSetDatabase(disp->m_dpy, disp->get_resources(false));

#if defined(POINTER_BLANK)
    ::XColor blackcolour;
    ::Font font = ::XLoadFont(disp->m_dpy, "fixed");

    blackcolour.red = 0;
    blackcolour.green = 0;
    blackcolour.blue = 0;
    disp->m_blank_cursor = ::XCreateGlyphCursor(
      disp->m_dpy,
      font,
      font,
      ' ',
      ' ',
      &blackcolour,
      &blackcolour
    );
    ::XUnloadFont(disp->m_dpy, font);
#endif

#if defined(XRENDER)
    int major;
    int minor;

    if (::XRenderQueryVersion(disp->m_dpy, &major, &minor))
    {
      if (major > 0 || (major == 0 && minor >= 11))
      {
        disp->m_flags |= has_render;

        if (auto filters = ::XRenderQueryFilters(disp->m_dpy, disp->root))
        {
          for (int i = 0; i < filters->nfilter; ++i)
          {
            if (!std::strcmp(filters->filter[i], FilterConvolution))
            {
              disp->m_flags |= has_render_conv;
            }
          }

          ::XFree(filters);
        }
      }
    }
#endif

    int fd = ::XConnectionNumber(disp->m_dpy);

    // try to detect whether we have a local connection.
    // assume unix domain socket == local, everything else not
    // TODO: might want to check for inet/127.0.0.1
    disp->m_local = false;
    sockaddr_un sa;
    socklen_t sl = sizeof(sa);

    if (!::getsockname(fd, reinterpret_cast<sockaddr*>(&sa), &sl))
    {
      disp->m_local = sa.sun_family == AF_UNIX;
    }

    disp->m_flush_event.start();
    disp->m_x_event.start(fd, ev::READ);
    ::fcntl(fd, F_SETFD, FD_CLOEXEC);

    ::XSelectInput(disp->m_dpy, disp->root, PropertyChangeMask);

    disp->flush();

    cache.emplace(id, disp);

    return disp;
  }

  void
  display::remove(const std::string& id)
  {
    cache.erase(id);
  }

  display::display(const std::string& id)
    : screen(0)
    , selection_owner(nullptr)
    , clipboard_owner(nullptr)
    , m_id(id)
    , m_flags(0)
    , m_local(false)
    , m_dpy(nullptr)
  {
    m_x_event.set<display, &display::x_cb>(this);
    m_flush_event.set<display, &display::flush_cb>(this);
  }

  display::~display()
  {
    if (!m_dpy)
    {
      return;
    }

#if defined(POINTER_BLANK)
    ::XFreeCursor(m_dpy, m_blank_cursor);
#endif
    m_x_event.stop();
    m_flush_event.stop();
#if defined(USE_XIM)
    xims.clear();
#endif
    ::XrmDestroyDatabase(::XrmGetDatabase(m_dpy));
    ::XCloseDisplay(m_dpy);
  }

  ::XrmDatabase
  display::get_resources(bool refresh)
  {
    const char* homedir = std::getenv("HOME");
    char fname[1024];

    /*
     * get resources using the X library function
     */
    char* displayResource;
    char* xe;
    ::XrmDatabase rdb1;
    ::XrmDatabase database = 0;

#if !XLIB_ILLEGAL_ACCESS
    /* work around a bug in XrmSetDatabase where it frees the db, see ref_next */
    database = ::XrmGetStringDatabase("");
#endif

    // for ordering, see for example http://www.faqs.org/faqs/Xt-FAQ/ Subject: 20
    // as opposed to "standard practise", we always read in ~/.Xdefaults

    // 6. System wide per application default file.

    /* Add in $XAPPLRESDIR/Rxvt only; not bothering with XUSERFILESEARCHPATH */
    if ((xe = std::getenv("XAPPLRESDIR")))
    {
      std::snprintf(fname, sizeof(fname), "%s/%s", xe, RESCLASS);

      if ((rdb1 = ::XrmGetFileDatabase(fname)))
      {
        ::XrmMergeDatabases(rdb1, &database);
      }
    }

    // 5. User's per application default file.
    // none

    // 4. User's defaults file.
    if (homedir)
    {
      std::snprintf(fname, sizeof(fname), "%s/.Xdefaults", homedir);

      if ((rdb1 = ::XrmGetFileDatabase(fname)))
      {
        ::XrmMergeDatabases(rdb1, &database);
      }
    }

    /* Get any Xserver defaults */
    if (refresh)
    {
      // fucking xlib keeps a copy of the rm string
      ::Atom actual_type;
      int actual_format;
      unsigned long nitems;
      unsigned long nremaining;
      char* val = nullptr;

#if XLIB_ILLEGAL_ACCESS
      if (m_dpy->xdefaults)
      {
        ::XFree(m_dpy->xdefaults);
      }
#endif

      const auto result = ::XGetWindowProperty(
        m_dpy,
        RootWindow(m_dpy, 0),
        XA_RESOURCE_MANAGER,
        0L,
        100000000L,
        False,
        XA_STRING,
        &actual_type,
        &actual_format,
        &nitems,
        &nremaining,
        reinterpret_cast<unsigned char**>(&val)
      );

      if (result == Success && actual_type == XA_STRING && actual_format == 8)
      {
        displayResource = val;
      } else {
        displayResource = nullptr;

        if (val)
        {
          ::XFree(val);
        }
      }

#if XLIB_ILLEGAL_ACCESS
      m_dpy->xdefaults = displayResource;
#endif
    } else {
      displayResource = ::XResourceManagerString(m_dpy);
    }

    if (displayResource)
    {
      if ((rdb1 = ::XrmGetStringDatabase(displayResource)))
      {
        ::XrmMergeDatabases(rdb1, &database);
      }
    }

#if !XLIB_ILLEGAL_ACCESS
    if (refresh && displayResource)
    {
      ::XFree(displayResource);
    }
#endif

    /* Get screen specific resources */
    displayResource = ::XScreenResourceString(ScreenOfDisplay(m_dpy, screen));

    if (displayResource)
    {
      if ((rdb1 = ::XrmGetStringDatabase(displayResource)))
      {
        /* Merge with screen-independent resources */
        ::XrmMergeDatabases(rdb1, &database);
      }

      ::XFree(displayResource);
    }

    // 3. User's per host defaults file
    /* Add in XENVIRONMENT file */
    if ((xe = std::getenv("XENVIRONMENT")) && (rdb1 = ::XrmGetFileDatabase(xe)))
    {
      ::XrmMergeDatabases(rdb1, &database);
    }
    else if (homedir)
    {
      struct utsname un;

      if (!::uname(&un))
      {
        std::snprintf(fname, sizeof(fname), "%s/.Xdefaults-%s", homedir, un.nodename);

        if ((rdb1 = ::XrmGetFileDatabase(fname)))
        {
          ::XrmMergeDatabases(rdb1, &database);
        }
      }
    }

    return database;
  }

  void
  display::flush()
  {
    m_flush_event.start();
  }

#if defined(USE_XIM)
  void
  display::im_change_cb()
  {
    for (const auto& i : m_im_watchers)
    {
      i->call();
    }
  }

  void
  display::im_change_check()
  {
    // try to only call im_change_cb when a new input method
    // registers, as xlib crashes due to a race otherwise.
    ::Atom actual_type;
    ::Atom* atoms;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    const auto result = ::XGetWindowProperty(
      m_dpy,
      root,
      xa[XA_XIM_SERVERS],
      0L,
      1000000L,
      False,
      XA_ATOM,
      &actual_type,
      &actual_format,
      &nitems,
      &bytes_after,
      reinterpret_cast<unsigned char**>(&atoms)
    );

    if (result != Success)
    {
      return;
    }

    if (actual_type == XA_ATOM && actual_format == 32)
    {
      for (int i = 0; i < nitems; ++i)
      {
        if (::XGetSelectionOwner(m_dpy, atoms[i]))
        {
          im_change_cb();
          break;
        }
      }
    }

    ::XFree(atoms);
  }
#endif

  void
  display::x_cb(ev::io& w, int revents)
  {
    m_flush_event.start();
  }

  void
  display::flush_cb(ev::prepare& w, int revents)
  {
    while (::XEventsQueued(m_dpy, QueuedAfterFlush))
    {
      do
      {
        ::XEvent xev;

        ::XNextEvent(m_dpy, &xev);

#if defined(USE_XIM)
        if (!::XFilterEvent(&xev, None))
        {
          if (xev.type == PropertyNotify
              && xev.xany.window == root
              && xev.xproperty.atom == xa[XA_XIM_SERVERS])
          {
            im_change_check();
          }
#endif
          if (xev.type == MappingNotify)
          {
            ::XRefreshKeyboardMapping(&xev.xmapping);
          }

          for (int i = m_xevent_watchers.size(); i--;)
          {
            if (!m_xevent_watchers[i])
            {
              auto w = m_xevent_watchers.back();

              m_xevent_watchers.pop_back();
              if (!m_xevent_watchers.empty())
              {
                m_xevent_watchers[i] = w;
                if (w)
                {
                  w->active = i + 1;
                }
              }
            }
            else if (m_xevent_watchers[i]->window == xev.xany.window)
            {
              m_xevent_watchers[i]->call(xev);
            }
          }
#if defined(USE_XIM)
        }
#endif
      }
      while (::XEventsQueued(m_dpy, QueuedAlready));
    }

    w.stop();
  }

  void
  display::reg(xevent_watcher* w)
  {
    if (!w->active)
    {
      m_xevent_watchers.push_back(w);
      w->active = m_xevent_watchers.size();
    }
  }

  void
  display::unreg(xevent_watcher* w)
  {
    if (w->active)
    {
      m_xevent_watchers[w->active - 1] = nullptr;
      w->active = 0;
    }
  }

  void
  display::set_selection_owner(rxvt_term* owner, bool clipboard)
  {
    rxvt_term*& cur_owner = !clipboard ? selection_owner : clipboard_owner;

    if (cur_owner && cur_owner != owner)
    {
      rxvt_term* term = cur_owner;

      term->selection_clear(clipboard);
      term->flush();
    }

    cur_owner = owner;
  }

#if defined(USE_XIM)
  void
  display::reg(im_watcher* w)
  {
    m_im_watchers.push_back(w);
  }

  void
  display::unreg(im_watcher* w)
  {
    const auto position = std::find(
      m_im_watchers.begin(),
      m_im_watchers.end(),
      w
    );

    if (position != std::end(m_im_watchers))
    {
      m_im_watchers.erase(position);
    }
  }

  rxvt_xim*
  display::get_xim(const char* locale, const char* modifiers)
  {
    char* id;
    const std::size_t l = std::strlen(locale);
    const std::size_t m = std::strlen(modifiers);

    if (!(id = rxvt_temp_buf<char> (l + m + 2)))
    {
      return nullptr;
    }

    std::memcpy(id, locale, l);
    id[l] = '\n';

    std::memcpy(id + l + 1, modifiers, m);
    id[l + m + 1] = 0;

    return xims.get(id);
  }

  void
  display::put_xim(rxvt_xim* xim)
  {
# if defined(XLIB_IS_RACEFREE)
    xims.put(xim);
# endif
  }
#endif

  ::Atom
  display::atom(const std::string& name)
  {
    return ::XInternAtom(m_dpy, name.c_str(), False);
  }

  Pixmap
  display::get_pixmap_property(Atom property)
  {
    Pixmap pixmap = None;
    int aformat;
    unsigned long nitems, bytes_after;
    Atom atype;
    unsigned char *prop;
    const auto result = ::XGetWindowProperty(
      m_dpy,
      root,
      property,
      0L,
      1L,
      False,
      XA_PIXMAP,
      &atype,
      &aformat,
      &nitems,
      &bytes_after,
      &prop
    );

    if (result == Success)
    {
      if (atype == XA_PIXMAP)
      {
        pixmap = *reinterpret_cast<Pixmap*>(prop);
      }
      ::XFree(prop);
    }

    return pixmap;
  }
}
