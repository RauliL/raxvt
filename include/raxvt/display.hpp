#ifndef RAXVT_DISPLAY_HPP_GUARD
#define RAXVT_DISPLAY_HPP_GUARD

#include <string>
#include <vector>

#include <X11/Xresource.h>

namespace raxvt
{
  class display
  {
  public:
    enum flag
    {
      has_render = 1 << 0,
      has_render_conv = 1 << 1
    };

    static display* get(const std::string& id);
    static void remove(const std::string& id);

    ~display();

    inline const std::string& id() const
    {
      return m_id;
    }

    inline bool has_flag(flag f) const
    {
      return (m_flags & f) != 0;
    }

    inline bool local() const
    {
      return m_local;
    }

    inline ::Display* dpy() const
    {
      return m_dpy;
    }

#if defined(POINTER_BLANK)
    inline const ::Cursor blank_cursor() const
    {
      return m_blank_cursor;
    }
#endif

    void flush_cb(ev::prepare& w, int revents);
    void x_cb(ev::io& w, int revents);

#if defined(USE_XIM)
    void im_change_cb();
    void im_change_check();
#endif

    bool ref_init();
    void ref_next();

    void flush();

    ::Atom atom(const std::string& name);
    ::Pixmap get_pixmap_property(::Atom property);
    void set_selection_owner(rxvt_term* owner, bool clipboard);

    void reg(xevent_watcher* w);
    void unreg(xevent_watcher* w);

#if defined(USE_XIM)
    void reg(im_watcher* w);
    void unreg(im_watcher* w);

    rxvt_xim* get_xim(const char* locale, const char* modifiers);
    void put_xim(rxvt_xim* xim);
#endif

#if defined(USE_XIM)
    refcache<rxvt_xim> xims;
#endif
    int screen;
    ::Window root;
    rxvt_term* selection_owner;
    rxvt_term* clipboard_owner;
    ::Atom xa[NUM_XA];

  private:
    explicit display(const std::string& id);
    display(const display&) = delete;
    display& operator=(const display&) = delete;

    ::XrmDatabase get_resources(bool refresh);

  private:
    const std::string m_id;
    std::uint8_t m_flags;
    bool m_local;
    ::Display* m_dpy;
    std::vector<xevent_watcher*> m_xevent_watchers;
    std::vector<im_watcher*> m_im_watchers;
    ev::prepare m_flush_event;
    ev::io m_x_event;
#if defined(POINTER_BLANK)
    ::Cursor m_blank_cursor;
#endif
  };
}

#endif /* !RAXVT_DISPLAY_HPP_GUARD */
