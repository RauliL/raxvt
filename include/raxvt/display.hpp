#ifndef RAXVT_DISPLAY_HPP_GUARD
#define RAXVT_DISPLAY_HPP_GUARD

#include <bitset>
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
      has_render = 0,
      has_render_conv = 1
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
      return m_flags[f];
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

    inline int screen() const
    {
      return m_screen;
    }

    inline Window root() const
    {
      return m_root;
    }

    inline ::Atom* xa()
    {
      return m_xa;
    }

    inline const ::Atom* xa() const
    {
      return m_xa;
    }

    inline const rxvt_term* selection_owner() const
    {
      return m_selection_owner;
    }

    inline void reset_selection_owner()
    {
      m_selection_owner = nullptr;
    }

    inline const rxvt_term* clipboard_owner() const
    {
      return m_clipboard_owner;
    }

    inline void reset_clipboard_owner()
    {
      m_clipboard_owner = nullptr;
    }

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

    std::shared_ptr<rxvt_xim> get_xim(
      const std::string& locale,
      const std::string& modifiers
    );
    void put_xim(const std::shared_ptr<rxvt_xim>& xim);
    void remove_xim(const std::string& id);
#endif

  private:
    explicit display(const std::string& id);
    display(const display&) = delete;
    display& operator=(const display&) = delete;

    ::XrmDatabase get_resources(bool refresh);

  private:
    const std::string m_id;
    std::bitset<2> m_flags;
    bool m_local;
    ::Display* m_dpy;
    std::vector<xevent_watcher*> m_xevent_watchers;
    std::vector<im_watcher*> m_im_watchers;
    ev::prepare m_flush_event;
    ev::io m_x_event;
#if defined(POINTER_BLANK)
    ::Cursor m_blank_cursor;
#endif
#if defined(USE_XIM)
    std::unordered_map<std::string, std::shared_ptr<rxvt_xim>> m_xims;
#endif
    int m_screen;
    ::Window m_root;
    rxvt_term* m_selection_owner;
    rxvt_term* m_clipboard_owner;
    ::Atom m_xa[NUM_XA];
  };
}

#endif /* !RAXVT_DISPLAY_HPP_GUARD */
