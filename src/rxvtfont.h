#ifndef DEFAULTFONT_H_
#define DEFAULTFONT_H_

#include <cstdint>
#include <memory>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "encoding.h"
#include "rxvtutil.h"
#include "rxvttoolkit.h"

struct rxvt_term;

struct rxvt_fontprop
{
  enum {
    unset  = -1,
    medium = 100, bold = 200,
    roman  = 0, italic = 100,
  };
  int width, height, ascent;
  int weight, slant;
};

struct rxvt_font
{
  // managed by the fontset
  rxvt_term *term;
  void set_term (rxvt_term *term) { this->term = term; }

  char *name;
  codeset cs;
  bool loaded; // whether we tried loading it before (not whether it's loaded)

  // managed by the font object
  int ascent, descent,
      width, height;

  void set_name (char *name_);

  rxvt_font ();
  virtual ~rxvt_font () { free (name); };

  virtual void clear () { };

  void clear_rect (rxvt_drawable &d, int x, int y, int w, int h, int color) const;

  virtual rxvt_fontprop properties () = 0;

  virtual bool load (const rxvt_fontprop &morph, bool force_prop) = 0;
  virtual bool has_char (uint32_t unicode, const rxvt_fontprop *prop, bool &careful) const = 0;

  virtual void draw (rxvt_drawable &d,
                     int x, int y,
                     const char32_t* text, int len,
                     int fg, int bg) = 0;
};

struct rxvt_fallback_font;

class rxvt_fontset
{
public:
  char *fontdesc;

  // must be power-of-two - 1, also has to match RS_fontMask in rxvt.h
  enum { fontCount =   7 }; // 36 extra colors bits, 4 fewer fontcount bits

  // index of first font in set
  enum { firstFont = 2 };

  rxvt_fontset(rxvt_term *term);
  ~rxvt_fontset();

  bool populate (const char *desc);
  void set_prop (const rxvt_fontprop &prop, bool force_prop) { this->prop = prop; this->force_prop = force_prop; }
  int find_font_idx (uint32_t unicode);
  int find_font (const char *name) const;
  bool realize_font (int i);

  const std::shared_ptr<rxvt_font>& operator[](int id) const
  {
    return fonts[id >> 1];
  }

  inline int
  find_font(unicode_t unicode)
  {
    return std::min<int>((fontCount << 1) | 1, find_font_idx(unicode));
  }

  rxvt_fontset(const rxvt_fontset&) = delete;
  void operator=(const rxvt_fontset&) = delete;

private:
  rxvt_term *term;
  rxvt_fontprop prop;
  bool force_prop;
  std::vector<std::shared_ptr<rxvt_font>> fonts;
  const rxvt_fallback_font *fallback;

  // this once was a "typedef xxx pagemap[256]
  // but c++ arrays are not normal types, and cannot be
  // put into containers, new doesn't work for them etc. etc.
  // so we wrap out array into an object that acts like one. doh.
  // example: C++ has no separate new and new [] forms,
  // and if pagemap is char[256], new incorrectly assumes we want to
  // allocate an array of chars instead of a single pagemap.
  struct pagemap
  {
    unsigned char cppsucks[256];
    unsigned char &operator [](int i) { return cppsucks [i]; };
  };
  std::vector<pagemap *> fmap;

  void clear ();
  std::shared_ptr<rxvt_font> new_font(const char *name, codeset cs);
  void prepare_font(const std::shared_ptr<rxvt_font>& font, codeset cs);
  void add_fonts (const char *desc);
  void push_font(const std::shared_ptr<rxvt_font>& font);
};

#endif /* _DEFAULTFONT_H_ */

