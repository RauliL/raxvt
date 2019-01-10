#ifndef RXVT_H_                /* include once only */
#define RXVT_H_

#include <cstdio>
#include <cctype>
#include <cerrno>
#include <cstdarg>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <sys/types.h>
#include <unistd.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

// we assume that Xlib.h defines XPointer, and it does since at least 1994...

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
}

typedef std::uint64_t rend_t;
typedef std::int32_t tlen_t;  // was int16_t, but this results in smaller code and memory use
typedef std::int32_t tlen_t_; // specifically for use in the line_t structure

#include "feature.h"

#if defined (ISO_14755) || defined (ENABLE_PERL)
# define ENABLE_OVERLAY 1
#endif

#if ENABLE_PERL
# define ENABLE_FRILLS    1
# define ENABLE_COMBINING 1
#endif

#if ENABLE_FRILLS
# define ENABLE_XEMBED        1
# define ENABLE_EWMH          1
# define ENABLE_XIM_ONTHESPOT 1
# define CURSOR_BLINK         1
# define OPTION_HC            1
# define BUILTIN_GLYPHS       1
#else
# define ENABLE_MINIMAL 1
#endif

#include <limits.h>

#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#if HAVE_PIXBUF
# include <gdk-pixbuf/gdk-pixbuf.h>
#endif

#if XRENDER && (HAVE_PIXBUF || ENABLE_TRANSPARENCY)
# define HAVE_IMG 1
#endif

#include "./ecb.h"
#include "./libptytty.h"

#include "encoding.h"
#include "rxvtutil.h"
#include "rxvtfont.h"
#include "rxvttoolkit.h"
#include "rxvtimg.h"
#include "ev_cpp.h"
#include "rxvtperl.h"
#include "raxvt/scrollbar.hpp"
#include "raxvt/selection.hpp"

/*
 *****************************************************************************
 * SYSTEM HACKS
 *****************************************************************************
 */

#include <termios.h>

#ifndef STDIN_FILENO
# define STDIN_FILENO   0
# define STDOUT_FILENO  1
# define STDERR_FILENO  2
#endif

/****************************************************************************/

// exception thrown on fatal (per-instance) errors
class rxvt_failure_exception { };

// exception thrown when the command parser runs out of input data
class out_of_input { };

/*
 *****************************************************************************
 * PROTOTYPES
 *****************************************************************************
 */
// main.C
extern bool rxvt_set_locale(const std::string&) noexcept;
extern void rxvt_push_locale(const std::string&) noexcept;
extern void rxvt_pop_locale() noexcept;
void rxvt_init ();

// misc.C
char *           rxvt_wcstombs                    (const wchar_t *str, int len = -1);
wchar_t *        rxvt_mbstowcs                    (const char *str, int len = -1);
char *           rxvt_wcstoutf8                   (const wchar_t *str, int len = -1);
wchar_t *        rxvt_utf8towcs                   (const char *str, int len = -1);

void             rxvt_vlog                        (const char *fmt, va_list arg_ptr);
void             rxvt_log                         (const char *fmt,...);
void             rxvt_warn                        (const char *fmt,...);
ecb_noreturn
void             rxvt_fatal                       (const char *fmt, ...);
ecb_noreturn
void             rxvt_exit_failure                ();

template<typename T>
static inline T*
rxvt_malloc(std::size_t size)
{
  void* p = std::malloc(size);

  if (!p)
  {
    rxvt_fatal("memory allocation failure. aborting\n");
  }

  return static_cast<T*>(p);
}

void *           rxvt_calloc                      (size_t number, size_t size);
void *           rxvt_realloc                     (void *ptr, size_t size);

KeySym rxvt_XKeycodeToKeysym (Display *dpy, KeyCode keycode, int index);

/////////////////////////////////////////////////////////////////////////////

// temporarily replace the process environment
extern char **environ;
extern char **rxvt_environ; // the original environ pointer

static inline void
set_environ (char **envv)
{
#if ENABLE_PERL
  assert (envv);
#else
  if (envv)
#endif
    environ = envv;
}

struct localise_env
{
  char **orig_env;

  localise_env (char **new_env)
  {
    orig_env = environ;
    environ = new_env;
  }

  ~localise_env ()
  {
    environ = orig_env;
  }
};

/*
 *****************************************************************************
 * STRUCTURES AND TYPEDEFS
 *****************************************************************************
 */

/*
 * the 'essential' information for reporting Mouse Events
 * pared down from XButtonEvent
 */
struct mouse_event
{
  int clicks;
  Time time;             /* milliseconds */
  unsigned int state;    /* key or button mask */
  unsigned int button;   /* detail */
};

#if ENABLE_XEMBED
// XEMBED messages
# define XEMBED_EMBEDDED_NOTIFY          0
# define XEMBED_WINDOW_ACTIVATE          1
# define XEMBED_WINDOW_DEACTIVATE        2
# define XEMBED_REQUEST_FOCUS            3
# define XEMBED_FOCUS_IN                 4
# define XEMBED_FOCUS_OUT                5
# define XEMBED_FOCUS_NEXT               6
# define XEMBED_FOCUS_PREV               7

# define XEMBED_MODALITY_ON              10
# define XEMBED_MODALITY_OFF             11
# define XEMBED_REGISTER_ACCELERATOR     12
# define XEMBED_UNREGISTER_ACCELERATOR   13
# define XEMBED_ACTIVATE_ACCELERATOR     14

// XEMBED detail code
# define XEMBED_FOCUS_CURRENT            0
# define XEMBED_FOCUS_FIRST              1
# define XEMBED_FOCUS_LAST               2

# define XEMBED_MAPPED			(1 << 0)
#endif

/*
 *****************************************************************************
 * NORMAL DEFINES
 *****************************************************************************
 */

/* COLORTERM, TERM environment variables */
#define COLORTERMENV    "rxvt"
#if HAVE_IMG
# define COLORTERMENVFULL COLORTERMENV "-xpm"
#else
# define COLORTERMENVFULL COLORTERMENV
#endif
#ifndef TERMENV
# define TERMENV        "rxvt-unicode-256color"
#endif

// Hidden color cube for indexed 24-bit colors. There are fewer blue levels
// because normal human eye is less sensitive to the blue component than to
// the red or green. (https://en.m.wikipedia.org/wiki/Color_depth#8-bit_color)
// 7x7x5=245 < 254 unused color indices
# define Red_levels      7
# define Green_levels    7
# define Blue_levels     5

#define RGB24_CUBE_SIZE (Red_levels * Green_levels * Blue_levels)

#if defined (NO_MOUSE_REPORT) && !defined (NO_MOUSE_REPORT_SCROLLBAR)
# define NO_MOUSE_REPORT_SCROLLBAR 1
#endif

#define scrollBar_esc           30

#if !defined (RXVT_SCROLLBAR) && !defined (NEXT_SCROLLBAR)
# define NO_SCROLLBAR_BUTTON_CONTINUAL_SCROLLING 1
#endif

enum {
  NO_REFRESH       = 0,  /* Window not visible at all!        */
  FAST_REFRESH     = 1,  /* Fully exposed window              */
  SLOW_REFRESH     = 2,  /* Partially exposed window          */
};

#ifdef NO_SECONDARY_SCREEN
# define NSCREENS               0
#else
# define NSCREENS               1
#endif

/* flags for rxvt_term::scr_gotorc () */
enum {
  C_RELATIVE = 1,       /* col movement is relative */
  R_RELATIVE = 2,       /* row movement is relative */
  RELATIVE   = C_RELATIVE | R_RELATIVE,
};

/* modes for rxvt_term::scr_insdel_chars (), rxvt_term::scr_insdel_lines () */
enum {
  INSERT = -1,				/* don't change these values */
  DELETE = +1,
  ERASE  = +2,
};

/* modes for rxvt_term::scr_page () - scroll page. used by scrollbar window */
enum page_dirn {
  DN     = -1,
  NO_DIR =  0,
  UP     =  1,
};

/* arguments for rxvt_term::scr_change_screen () */
enum {
  PRIMARY = 0,
  SECONDARY,
};

// define various rendition bits and masks. the rendition word
// is 32 bits in size, and we should use it as efficiently as possible

#define RS_None                 0

// GET_BGATTR depends on RS_fgShift > RS_bgShift
#define RS_colorMask (static_cast<rend_t>((1UL << Color_Bits) - 1UL))
#define RS_bgShift 0
#define RS_fgShift (RS_bgShift + Color_Bits)
#define RS_bgMask (static_cast<rend_t>(RS_colorMask << RS_bgShift))
#define RS_fgMask (static_cast<rend_t>(RS_colorMask << RS_fgShift))

// must have space for rxvt_fontset::fontCount * 2 + 2 values
#define RS_fontShift            (RS_fgShift + Color_Bits)
/** Be careful when drawing these. */
#define RS_Careful (static_cast<rend_t>(1UL << RS_fontShift))
#define RS_fontCount (static_cast<rend_t>(rxvt_fontset::fontCount))
/** Includes RS_Careful. */
#define RS_fontMask (static_cast<rend_t>((RS_fontCount << (RS_fontShift + 1)) | RS_Careful))

// toggle this to force redraw, must be != RS_Careful and otherwise "pretty neutral"
#define RS_redraw (static_cast<rend_t>(2UL << RS_fontShift))

#define RS_selShift (RS_fontShift + 4)
#define RS_Sel (static_cast<rend_t>(1UL << 22))

// 4 custom bits for extensions
#define RS_customCount          16UL
#define RS_customShift (RS_selShift + 1)
#define RS_customMask (static_cast<rend_t>((RS_customCount - 1UL) << RS_customShift))

// font styles
#define RS_Bold (static_cast<rend_t>(1UL << RS_styleShift))
#define RS_Italic	(static_cast<rend_t>(2UL << RS_styleShift))

#define RS_styleCount 4
#define RS_styleShift (RS_customShift + RS_styleCount)
#define RS_styleMask (static_cast<rend_t>(RS_Bold | RS_Italic))

// fake styles
#define RS_Blink (static_cast<rend_t>(1UL << (RS_styleShift + 2)))
#define RS_RVid (static_cast<rend_t>(1UL << (RS_styleShift + 3)))    // reverse video
#define RS_Uline (static_cast<rend_t>(1UL << (RS_styleShift + 4)))    // underline

#define RS_baseattrMask (static_cast<rend_t>(RS_Italic | RS_Bold | RS_Blink | RS_RVid | RS_Uline))
#define RS_attrMask (static_cast<rend_t>(RS_baseattrMask | RS_fontMask))

#define DEFAULT_RSTYLE  (RS_None | (Color_fg    << RS_fgShift) | (Color_bg     << RS_bgShift))
#define OVERLAY_RSTYLE  (RS_None | (Color_Black << RS_fgShift) | (Color_Yellow << RS_bgShift))

enum {
  C0_NUL = 0x00,
          C0_SOH, C0_STX, C0_ETX, C0_EOT, C0_ENQ, C0_ACK, C0_BEL,
  C0_BS , C0_HT , C0_LF , C0_VT , C0_FF , C0_CR , C0_SO , C0_SI ,
  C0_DLE, C0_DC1, C0_DC2, D0_DC3, C0_DC4, C0_NAK, C0_SYN, C0_ETB,
  C0_CAN, C0_EM , C0_SUB, C0_ESC, C0_IS4, C0_IS3, C0_IS2, C0_IS1,
};
#define CHAR_ST                 0x9c    /* 0234 */

/*
 * XTerm Operating System Commands: ESC ] Ps;Pt (ST|BEL)
 * colour extensions by Christian W. Zuckschwerdt <zany@triq.net>
 */
enum {
  XTerm_name             =  0,
  XTerm_iconName         =  1,
  XTerm_title            =  2,
  XTerm_property         =  3,      // change X property
  XTerm_Color            =  4,      // change colors
  XTerm_Color00          = 10,      // change fg color
  XTerm_Color01          = 11,      // change bg color
  XTerm_Color_cursor     = 12,      // change actual 'Cursor' color
  XTerm_Color_pointer_fg = 13,      // change actual 'Pointer' fg color
  XTerm_Color_pointer_bg = 14,      // change actual 'Pointer' bg color
  XTerm_Color05          = 15,      // not implemented (tektronix fg)
  XTerm_Color06          = 16,      // not implemented (tektronix bg)
  XTerm_Color_HC         = 17,      // change actual 'Highlight' bg color
  XTerm_Color_HTC        = 19,      // change actual 'Highlight' fg color
  XTerm_logfile          = 46,      // not implemented
  XTerm_font             = 50,

  XTerm_konsole30        = 30,      // reserved for konsole
  XTerm_konsole31        = 31,      // reserved for konsole
  XTerm_emacs51          = 51,      // reserved for emacs shell
  /*
   * rxvt extensions of XTerm OSCs: ESC ] Ps;Pt (ST|BEL)
   */

  // deprecated
  Rxvt_restoreFG         = 39,
  Rxvt_restoreBG         = 49,

  Rxvt_dumpscreen        = 55,      // dump scrollback and all of screen

  URxvt_locale           = 701,     // change locale
  URxvt_version          = 702,     // request version

  URxvt_Color_IT         = 704,     // change actual 'Italic' colour
  URxvt_Color_BD         = 706,     // change actual 'Bold' color
  URxvt_Color_UL         = 707,     // change actual 'Underline' color
  URxvt_Color_border     = 708,

  URxvt_font             = 710,
  URxvt_boldFont         = 711,
  URxvt_italicFont       = 712,
  URxvt_boldItalicFont   = 713,

  URxvt_view_up          = 720,
  URxvt_view_down        = 721,

  URxvt_perl             = 777,     // for use by perl extensions, starts with "extension-name;"
};

/* Words starting with `Color_' are colours.  Others are counts */
/*
 * The PixColor and rendition colour usage should probably be decoupled
 * on the unnecessary items, e.g. Color_pointer, but won't bother
 * until we need to.  Also, be aware of usage in pixcolor_set
 */

enum colour_list {
  Color_none = -2,
  Color_transparent = -1,
  Color_fg = 0,
  Color_bg,
  minCOLOR,                   /* 2 */
  Color_Black = minCOLOR,
  Color_Red3,
  Color_Green3,
  Color_Yellow3,
  Color_Blue3,
  Color_Magenta3,
  Color_Cyan3,
  maxCOLOR,                   /* minCOLOR + 7 */
#ifndef NO_BRIGHTCOLOR
  Color_AntiqueWhite = maxCOLOR,
  minBrightCOLOR,             /* maxCOLOR + 1 */
  Color_Grey25 = minBrightCOLOR,
  Color_Red,
  Color_Green,
  Color_Yellow,
  Color_Blue,
  Color_Magenta,
  Color_Cyan,
  maxBrightCOLOR,             /* minBrightCOLOR + 7 */
  Color_White = maxBrightCOLOR,
#else
  Color_White = maxCOLOR,
#endif
  minTermCOLOR = Color_White + 1,
  maxTermCOLOR = Color_White + 240,
  minTermCOLOR24,
  maxTermCOLOR24 = minTermCOLOR24 +
                   RGB24_CUBE_SIZE - 1,
#ifndef NO_CURSORCOLOR
  Color_cursor,
  Color_cursor2,
#endif
  Color_pointer_fg,
  Color_pointer_bg,
  Color_border,
#ifndef NO_BOLD_UNDERLINE_REVERSE
  Color_BD,
  Color_IT,
  Color_UL,
  Color_RV,
#endif
#if ENABLE_FRILLS
  Color_underline,
#endif
#ifdef OPTION_HC
  Color_HC,
  Color_HTC,
#endif
  Color_scroll,
#ifdef RXVT_SCROLLBAR
  Color_trough,
#endif
#if OFF_FOCUS_FADING
  Color_fade,
#endif
  NRS_COLORS,                 /* */
#ifdef RXVT_SCROLLBAR
  Color_topShadow = NRS_COLORS,
  Color_bottomShadow,
  TOTAL_COLORS
#else
  TOTAL_COLORS = NRS_COLORS
#endif
};

#define Color_Bits 25

#if maxTermCOLOR24 >= (1 << Color_Bits)
# error color index overflow
#endif

/*
 * Resource list
 */
enum {
# define def(name) Rs_ ## name,
# define reserve(name,count) Rs_ ## name ## _ = Rs_ ## name + (count) - 1,
# include "rsinc.h"
# undef def
# undef reserve
  NUM_RESOURCES
};

/* DEC private modes */
#define PrivMode_132            (1UL<<0)
#define PrivMode_132OK          (1UL<<1)
#define PrivMode_rVideo         (1UL<<2)
#define PrivMode_relOrigin      (1UL<<3)
#define PrivMode_Screen         (1UL<<4)
#define PrivMode_Autowrap       (1UL<<5)
#define PrivMode_aplCUR         (1UL<<6)
#define PrivMode_aplKP          (1UL<<7)
#define PrivMode_HaveBackSpace  (1UL<<8)
#define PrivMode_BackSpace      (1UL<<9)
#define PrivMode_ShiftKeys      (1UL<<10)
#define PrivMode_VisibleCursor  (1UL<<11)
#define PrivMode_MouseX10       (1UL<<12)
#define PrivMode_MouseX11       (1UL<<13)
#define PrivMode_scrollBar      (1UL<<14)
#define PrivMode_TtyOutputInh   (1UL<<15)
#define PrivMode_Keypress       (1UL<<16)
#define PrivMode_smoothScroll   (1UL<<17)
#define PrivMode_vt52           (1UL<<18)
#define PrivMode_LFNL		(1UL<<19)
#define PrivMode_MouseBtnEvent  (1UL<<20)
#define PrivMode_MouseAnyEvent  (1UL<<21)
#define PrivMode_BracketPaste   (1UL<<22)
#define PrivMode_ExtModeMouse   (1UL<<23) // xterm pseudo-utf-8 hack
#define PrivMode_ExtMouseRight  (1UL<<24) // xterm pseudo-utf-8, but works in non-utf-8-locales
#define PrivMode_BlinkingCursor (1UL<<25)
#define PrivMode_FocusEvent     (1UL<<26)

#define PrivMode_mouse_report   (PrivMode_MouseX10|PrivMode_MouseX11|PrivMode_MouseBtnEvent|PrivMode_MouseAnyEvent)

#ifdef ALLOW_132_MODE
# define PrivMode_Default (PrivMode_Autowrap|PrivMode_ShiftKeys|PrivMode_VisibleCursor|PrivMode_132OK)
#else
# define PrivMode_Default (PrivMode_Autowrap|PrivMode_ShiftKeys|PrivMode_VisibleCursor)
#endif

// do not change these constants lightly, there are many interdependencies
#define IMBUFSIZ               128     // input modifier buffer sizes
#define KBUFSZ                 512     // size of keyboard mapping buffer
#define CBUFSIZ                32768   // size of command buffer (longest command sequence possible)
#define CBUFCNT                8       // never call pty_fill/cmd_parse more than this often in a row
#define UBUFSIZ                2048    // character buffer

#if ENABLE_FRILLS
# include <X11/Xmd.h>
typedef struct _mwmhints
{
  CARD32 flags;
  CARD32 functions;
  CARD32 decorations;
  INT32  input_mode;
  CARD32 status;
} MWMHints;
#endif

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)
/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)
/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)
/* bit definitions for MwmHints.inputMode */
#define MWM_INPUT_MODELESS                  0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL              2
#define MWM_INPUT_FULL_APPLICATION_MODAL    3
#define PROP_MWM_HINTS_ELEMENTS             5

/*
 *****************************************************************************
 * MACRO DEFINES
 *****************************************************************************
 */

// speed hack, copy some member variable into a local variable of the same name
#define dLocal(type,name)       type const name = this->name

// for speed reasons, we assume that all codepoints 32 to 126 are
// single-width.
#define WCWIDTH(c)		(IN_RANGE_INC (c, 0x20, 0x7e) ? 1 : wcwidth (c))

/* convert pixel dimensions to row/column values.  Everything as int32_t */
#define Pixel2Col(x)            Pixel2Width((int32_t)(x))
#define Pixel2Row(y)            Pixel2Height((int32_t)(y))
#define Pixel2Width(x)          ((int32_t)(x) / (int32_t)fwidth)
#define Pixel2Height(y)         ((int32_t)(y) / (int32_t)fheight)
#define Col2Pixel(col)          ((int32_t)Width2Pixel(col))
#define Row2Pixel(row)          ((int32_t)Height2Pixel(row))
#define Width2Pixel(n)          ((int32_t)(n) * (int32_t)fwidth)
#define Height2Pixel(n)         ((int32_t)(n) * (int32_t)fheight)

#define LINENO_of(t,n) MOD ((t)->term_start + int(n), (t)->total_rows)
#define ROW_of(t,n) (t)->row_buf [LINENO_of ((t), n)]

#define LINENO(n) LINENO_of (this, n)
#define ROW(n) ROW_of (this, n)

/* how to build & extract colors and attributes */
#define GET_BASEFG(x) ((((rend_t) (x)) & RS_fgMask) >> RS_fgShift)
#define GET_BASEBG(x) ((((rend_t) (x)) & RS_bgMask) >> RS_bgShift)

#define GET_FONT(x) ((((rend_t) (x)) & RS_fontMask) >> RS_fontShift)
#define SET_FONT(x,fid) ((((rend_t) (x)) & ~((rend_t) RS_fontMask)) | (((rend_t) (fid)) << RS_fontShift))

#define GET_STYLE(x) ((((rend_t) (x)) & RS_styleMask) >> RS_styleShift)
#define SET_STYLE(x,style) ((((rend_t) (x)) & ~((rend_t) RS_styleMask)) | (((rend_t) (style)) << RS_styleShift))

#define GET_ATTR(x) ((((rend_t) (x)) & RS_attrMask))
#define SET_FGCOLOR(x,fg) ((((rend_t) (x)) & ~((rend_t) RS_fgMask))   | (((rend_t) (fg)) << RS_fgShift))
#define SET_BGCOLOR(x,bg) ((((rend_t) (x)) & ~((rend_t) RS_bgMask))   | (((rend_t) (bg)) << RS_bgShift))
#define SET_ATTR(x,a) ((((rend_t) (x)) & ~((rend_t) RS_attrMask)) | ((rend_t) (a)))

#define RS_SAME(a,b) (!((((rend_t) (a)) ^ ((rend_t) (b))) & ~((rend_t) RS_Careful)))

#define PIXCOLOR_NAME(idx)      get_setting(Rs_color + (idx))
#define ISSET_PIXCOLOR(idx)     (!!get_setting(Rs_color + (idx)))

#if ENABLE_STYLES
# define FONTSET_of(t,style) (t)->fontset[GET_STYLE (style)]
#else
# define FONTSET_of(t,style) (t)->fontset[0]
#endif

#define FONTSET(style) FONTSET_of (this, style)

using log_callback = std::function<void(const char*)>;
using getfd_callback = std::function<int(int)>;

/****************************************************************************/

#define LINE_LONGER     0x0001 // line is continued on the next row
#define LINE_FILTERED   0x0002 // line has been filtered
#define LINE_COMPRESSED 0x0004 // line has been compressed (NYI)
#define LINE_FILTER     0x0008 // line needs to be filtered before display (NYI)
#define LINE_BIDI       0x0010 // line needs bidi (NYI)

struct line_t
{
  /** The actual text. */
  char32_t* t;
   rend_t *r; // rendition, uses RS_ flags
   tlen_t_ l; // length of each text line
   std::uint32_t f; // flags

   bool valid ()
   {
     return l >= 0;
   }

   void alloc ()
   {
     l = 0;
   }

   bool is_longer ()
   {
     return f & LINE_LONGER;
   }

   void is_longer (int set)
   {
     if (set)
       f |= LINE_LONGER;
     else
       f &= ~LINE_LONGER;
   }

   void clear ()
   {
     t = 0;
     r = 0;
     l = 0;
     f = 0;
   }

   void touch () // call whenever a line is changed/touched/updated
   {
#if ENABLE_PERL
     f &= ~LINE_FILTERED;
#endif
   }

   void touch (int col)
   {
     l = std::max(l, col);
     touch ();
   }
};

/****************************************************************************/

// primitive wrapper around mbstate_t to ensure initialisation
struct mbstate
{
  mbstate_t mbs;

  operator mbstate_t*() { return &mbs; }
  void reset () { std::memset(&mbs, 0, sizeof(mbs)); }
  mbstate () { reset (); }
};

/****************************************************************************/

#define UNICODE_MASK 0x1fffffUL

#define COMPOSE_LO 0x40000000UL
#define COMPOSE_HI 0x400fffffUL
#define IS_COMPOSE(n) ((int32_t)(n) >= COMPOSE_LO)

#if ENABLE_COMBINING
// compose chars are used to represent composite characters
// that are not representable in unicode, as well as characters
// not fitting in the BMP.
struct compose_char
{
  unicode_t c1, c2; // any chars != NOCHAR are valid

  compose_char (unicode_t c1, unicode_t c2)
  : c1(c1), c2(c2)
  { }
};

class rxvt_composite_vec
{
  std::vector<compose_char> v;
public:
  char32_t compose (unicode_t c1, unicode_t c2 = NOCHAR);
  int expand (unicode_t c, wchar_t *r);
  compose_char *operator [](char32_t c)
  {
    return c >= COMPOSE_LO && c < COMPOSE_LO + v.size ()
           ? &v[c - COMPOSE_LO]
           : 0;
  }
};

extern class rxvt_composite_vec rxvt_composite;
#endif

/****************************************************************************/

#ifdef KEYSYM_RESOURCE
class keyboard_manager;
#endif

typedef struct rxvt_term *rxvt_t;

extern rxvt_t rxvt_current_term;

#define SET_R(r) rxvt_current_term = const_cast<rxvt_term *>(r)
#define GET_R rxvt_current_term

/* ------------------------------------------------------------------------- */
class overlay_base
{
public:
  int x, y, w, h; // overlay dimensions
  char32_t** text;
  rend_t **rend;

  overlay_base()
    : x(0)
    , y(0)
    , w(0)
    , h(0)
    , text(nullptr)
    , rend(nullptr) {}

  overlay_base(const overlay_base&) = delete;
  void operator=(const overlay_base&) = delete;
};

/*
 * screen accounting:
 * screen_t elements
 *   row:       Cursor row position                   : 0 <= row < nrow
 *   col:       Cursor column position                : 0 <= col < ncol
 *   tscroll:   Scrolling region top row inclusive    : 0 <= row < nrow
 *   bscroll:   Scrolling region bottom row inclusive : 0 <= row < nrow
 *
 * selection_t elements
 *   clicks:    1, 2 or 3 clicks - 4 indicates a special condition of 1 where
 *              nothing is selected
 *   beg:       row/column of beginning of selection  : never past mark
 *   mark:      row/column of initial click           : never past end
 *   end:       row/column of one character past end of selection
 * * Note: top_row <= beg.row <= mark.row <= end.row < nrow
 * * Note: col == -1 ==> we're left of screen
 *
 */
class screen_t
{
public:
  raxvt::coordinates cur;          /* cursor position on the screen             */
  int             tscroll;      /* top of settable scroll region             */
  int             bscroll;      /* bottom of settable scroll region          */
  unsigned int    charset;      /* character set number [0..3]               */
  unsigned int    flags;        /* see below                                 */
  raxvt::coordinates s_cur;        /* saved cursor position                     */
  unsigned int    s_charset;    /* saved character set number [0..3]         */
  char            s_charset_char;
  rend_t          s_rstyle;     /* saved rendition style                     */

  screen_t();

  screen_t(const screen_t&) = delete;
  void operator=(const screen_t&) = delete;
};

/* ------------------------------------------------------------------------- */

/* screen_t flags */
#define Screen_Relative          (1<<0)  /* relative origin mode flag         */
#define Screen_VisibleCursor     (1<<1)  /* cursor visible?                   */
#define Screen_Autowrap          (1<<2)  /* auto-wrap flag                    */
#define Screen_Insert            (1<<3)  /* insert mode (vs. overstrike)      */
#define Screen_WrapNext          (1<<4)  /* need to wrap for next char?       */
#define Screen_DefaultFlags      (Screen_VisibleCursor | Screen_Autowrap)

/* rxvt_vars.options */
enum {
# define def(name)   Opt_ ## name,
# define nodef(name) Opt_prev_ ## name, Opt_ ## name = 0, Opt_next_ ## name = Opt_prev_ ## name - 1,
  Opt_0,
# include "optinc.h"
# undef nodef
# undef def
  Opt_count
};

struct rxvt_term : rxvt_screen
{
  int            vt_width;      /* actual window width             [pixels] */
  int            vt_height;     /* actual window height            [pixels] */
  int            width;         /* window width                    [pixels] */
  int            height;        /* window height                   [pixels] */
  int            fwidth;        /* font width                      [pixels] */
  int            fheight;       /* font height                     [pixels] */
  int            fbase;         /* font ascent (baseline)          [pixels] */
  int            ncol;          /* window columns              [characters] */
  int            nrow;          /* window rows                 [characters] */
  int            focus;         /* window has focus                         */
  int            mapped;        /* window state mapped?                     */
  int            int_bwidth;    /* internal border width                    */
  int            ext_bwidth;    /* external border width                    */
  int            lineSpace;     /* number of extra pixels between rows      */
  int            letterSpace;   /* number of extra pixels between columns   */
  int            saveLines;     /* number of lines that fit in scrollback   */
  int            total_rows;    /* total number of rows in this terminal    */
  int            term_start;    /* term lines start here                    */
  int            view_start;    /* scrollback view starts here              */
  int            top_row;       /* topmost row index of scrollback          */
  Window         parent;        /* parent identifier                        */
  Window         vt;            /* vt100 window                             */
  GC             gc;            /* GC for drawing                           */
  rxvt_drawable *drawable;
  rxvt_fontset  *fontset[4];

  std::shared_ptr<raxvt::scrollbar> scrollbar;
  XSizeHints      szHint;
  rxvt_color     *pix_colors;
  Cursor          TermWin_cursor;       /* cursor for vt window */

  line_t         *row_buf;      // all lines, scrollback + terminal, circular
  line_t         *drawn_buf;    // text on screen
  line_t         *swap_buf;     // lines for swap buffer
  char           *tabs;         /* per location: 1 == tab-stop               */
  screen_t        screen;
  screen_t        swap;
  raxvt::selection selection;
  rxvt_color      pix_colors_focused[TOTAL_COLORS];
#ifdef OFF_FOCUS_FADING
  rxvt_color      pix_colors_unfocused[TOTAL_COLORS];
#endif

  // special markers with magic addresses
  static const char resval_undef [];    // options specifically unset
  static const char resval_on [];       // boolean options switched on
  static const char resval_off [];      // or off

  log_callback   *log_hook;             // log error messages through this hook, if != 0
  getfd_callback *getfd_hook;           // convert remote to local fd, if != 0
#if ENABLE_PERL
  rxvt_perl_term  perl;
#endif
  struct mbstate  mbstate;              // current input multibyte state

  unsigned char   want_refresh:1,
                  current_screen:1,	/* primary or secondary              */
                  num_scr_allow:1,
                  bypass_keystate:1,
#if ENABLE_FRILLS
                  urgency_hint:1,
#endif
#if CURSOR_BLINK
                  hidden_cursor:1,
#endif
#if TEXT_BLINK
                  hidden_text:1,
#endif
#if POINTER_BLANK
                  hidden_pointer:1,
#endif
                  enc_utf8:1,		/* whether locale uses utf-8 */
                  seen_input:1,         /* whether we have seen some program output yet */
                  seen_resize:1,	/* whether we had a resize event */
                  init_done:1,
                  parsed_geometry:1;

  unsigned char   refresh_type,
#ifdef META8_OPTION
                  meta_char;            /* Alt-key prefix */
#endif
/* ---------- */
  bool            rvideo_state, rvideo_mode;
#ifndef NO_BELL
  bool            rvideo_bell;
#endif
  int             num_scr;              /* screen: number of lines scrolled */
  int             prev_ncol,            /* screen: previous number of columns */
                  prev_nrow;            /* screen: previous number of rows */
/* ---------- */
  rend_t          rstyle;
/* ---------- */
#ifdef SELECTION_SCROLLING
  int             scroll_selection_lines;
  int             selection_save_x,
                  selection_save_y,
                  selection_save_state;
#endif
/* ---------- */
  int             csrO,       /* Hops - csr offset in thumb/slider to      */
                              /*   give proper Scroll behaviour            */
#if defined(MOUSE_WHEEL) && defined(MOUSE_SLIP_WHEELING)
                  mouse_slip_wheel_speed,
#endif
                  refresh_count,
                  window_vt_x,
                  window_vt_y,
                  mouse_row,
                  mouse_col,
# ifdef POINTER_BLANK
                  pointerBlankDelay,
# endif
                  multiClickTime,
                  cursor_type,
                  allowedxerror;
/* ---------- */
  unsigned int    ModLevel3Mask,
                  ModMetaMask,
                  ModNumLockMask;
  unsigned long   priv_modes,
                  SavedModes;
/* ---------- */
  Atom            *xa;
/* ---------- */
  Time            selection_time,
                  clipboard_time;
  rxvt_selection *selection_req;
  pid_t           cmd_pid;    /* process id of child */
/* ---------- */
  struct mouse_event MEvent;
  XComposeStatus  compose;
  static struct termios def_tio;
  raxvt::coordinates oldcursor;

#ifdef HAVE_IMG
  enum {
    BG_IS_TRANSPARENT    = 1 << 1,
    BG_NEEDS_REFRESH     = 1 << 2,
  };

  std::uint8_t bg_flags;

  rxvt_img *bg_img;
#endif

#if ENABLE_OVERLAY
  overlay_base ov;

  void scr_swap_overlay ();
  void scr_overlay_new (int x, int y, int w, int h);
  void scr_overlay_off ();
  void scr_overlay_set (int x, int y,
                        char32_t text,
                        rend_t rend = OVERLAY_RSTYLE);
  void scr_overlay_set (int x, int y, const char *s);
  void scr_overlay_set (int x, int y, const wchar_t *s);
#endif

  std::vector<void*> allocated;           // free these memory blocks with free()

  int            parent_x, parent_y; // parent window position relative to root, only updated on demand

  char            charsets[4];
  char           *v_buffer;           /* pointer to physical buffer */
  unsigned int    v_buflen;           /* size of area to write */
  std::vector<std::string> argv;
  std::vector<std::string> envv;
  char** env;
  std::size_t env_size;

#ifdef KEYSYM_RESOURCE
  keyboard_manager *keyboard;
#endif
#ifndef NO_RESOURCES
  XrmDatabase option_db;
#endif

  /* command input buffering */
  char           *cmdbuf_ptr, *cmdbuf_endp;
  char            cmdbuf_base[CBUFSIZ];

  ptytty         *pty;

  // chunk contains all line_t's as well as rend_t and char32_t buffers
  // for drawn_buf, swap_buf and row_buf, in this order
  void           *chunk;
  size_t          chunk_size;

  std::uint32_t        rgb24_color[RGB24_CUBE_SIZE];   // the 24-bit color value
  std::uint16_t        rgb24_seqno[RGB24_CUBE_SIZE];   // which one is older?
  std::uint16_t        rgb24_sequence;

  /** Vector containing all running terminals. */
  static std::vector<rxvt_term*> termlist;

#if ENABLE_FRILLS || ISO_14755
  // ISO 14755 entry support
  unicode_t iso14755buf;
  void commit_iso14755 ();
# if ISO_14755
  void iso14755_51 (unicode_t ch, rend_t r = DEFAULT_RSTYLE, int x = 0, int y = -1, int y2 = -1);
  void iso14755_54 (int x, int y);
# endif
#endif

  long vt_emask, vt_emask_perl, vt_emask_xim, vt_emask_mouse;

  void vt_select_input () const
  {
    XSelectInput (dpy, vt, vt_emask | vt_emask_perl | vt_emask_xim | vt_emask_mouse);
  }

#if ENABLE_PERL
  void rootwin_cb (XEvent &xev);
  xevent_watcher rootwin_ev;
#endif

  void x_cb (XEvent &xev);
  xevent_watcher termwin_ev;
  xevent_watcher vt_ev;
  xevent_watcher scrollbar_ev;

  void child_cb (ev::child &w, int revents); ev::child child_ev;
  void destroy_cb (ev::idle &w, int revents); ev::idle destroy_ev;
  void refresh_check ();
  void flush ();
  void flush_cb (ev::timer &w, int revents); ev::timer flush_ev;
  void cmdbuf_reify ();
  void cmdbuf_append (const char *str, size_t count);
  bool pty_fill ();
  void pty_cb (ev::io &w, int revents); ev::io pty_ev;

#ifdef CURSOR_BLINK
  void cursor_blink_reset ();
  void cursor_blink_cb (ev::timer &w, int revents); ev::timer cursor_blink_ev;
#endif
#ifdef TEXT_BLINK
  void text_blink_cb (ev::timer &w, int revents); ev::timer text_blink_ev;
#endif
#ifndef NO_BELL
  void bell_cb (ev::timer &w, int revents); ev::timer bell_ev;
#endif

#ifndef NO_SCROLLBAR_BUTTON_CONTINUAL_SCROLLING
  void cont_scroll_cb (ev::timer &w, int revents); ev::timer cont_scroll_ev;
#endif
#ifdef SELECTION_SCROLLING
  void sel_scroll_cb (ev::timer &w, int revents); ev::timer sel_scroll_ev;
#endif
#if defined(MOUSE_WHEEL) && defined(MOUSE_SLIP_WHEELING)
  void slip_wheel_cb (ev::timer &w, int revents); ev::timer slip_wheel_ev;
#endif

#ifdef POINTER_BLANK
  void pointer_cb (ev::timer &w, int revents); ev::timer pointer_ev;
  void pointer_blank ();
#endif
  void pointer_unblank ();

  void tt_printf (const char *fmt,...);
  void tt_write_ (const char *data, unsigned int len);
  void tt_write (const char *data, unsigned int len);
  void tt_write_user_input (const char *data, unsigned int len);
  void pty_write ();

  /**
   * Makes this terminal the "currently active" instance.
   */
  void make_current() const
  {
    SET_R(this);
    set_environ(env);
    rxvt_set_locale(m_locale);
  }

#if USE_XIM
  std::shared_ptr<rxvt_xim> input_method;
  XIC      Input_Context;
  XIMStyle input_style;
  XPoint   spot; // most recently sent spot position

  void im_destroy ();
  void im_cb (); im_watcher im_ev;
  void im_set_size (XRectangle &size);
  void im_set_position (XPoint &pos);
  void im_set_color (unsigned long &fg, unsigned long &bg);
  void im_set_preedit_area (XRectangle &preedit_rect, XRectangle &status_rect, const XRectangle &needed_rect);

  bool im_is_running ();
  void im_send_spot ();
  bool im_get_ic (const char *modifiers);
  void im_set_position ();
#endif

  // command.C
  void key_press (XKeyEvent &ev);
  void key_release (XKeyEvent &ev);

  wchar_t next_char ();
  wchar_t cmd_getc ();
  std::uint32_t next_octet ();
  std::uint32_t cmd_get8 ();

  void cmd_parse ();
  void mouse_report (XButtonEvent &ev);
  void button_press (XButtonEvent &ev);
  void button_release (XButtonEvent &ev);
  void focus_in ();
  void focus_out ();
#if ENABLE_FRILLS
  void set_urgency (bool enable);
#else
  void set_urgency (bool enable) { }
#endif
  void update_fade_color (unsigned int idx, bool first_time = false);
#ifdef PRINTPIPE
  FILE *popen_printer ();
  int pclose_printer (FILE *stream);
#endif
  void process_print_pipe ();
  void process_nonprinting (unicode_t ch);
  void process_escape_vt52 (unicode_t ch);
  void process_escape_seq ();
  void process_csi_seq ();
  void process_window_ops (const int *args, unsigned int nargs);
  char *get_to_st (unicode_t &ends_how);
  void process_dcs_seq ();
  void process_osc_seq ();
  void process_color_seq (int report, int color, const char *str, char resp);
  void process_xterm_seq (int op, char *str, char resp);
  int privcases (int mode, unsigned long bit);
  void process_terminal_mode (int mode, int priv, unsigned int nargs, const int *arg);
  void process_sgr_mode (unsigned int nargs, const int *arg);
  void set_cursor_style (int style);
  // init.C
  void init(
    const std::vector<std::string>& argv,
    const std::vector<std::string>& envv
  );
  void init (int argc, const char *const *argv, const char *const *envv);
  void init2(const std::vector<std::string>& argv);
  void init_vars ();
  std::vector<std::string> init_resources(const std::vector<std::string>& argv);
  void init_env ();

  inline const std::string& locale() const
  {
    return m_locale;
  }

  void set_locale(const std::string& locale);

  void init_xlocale ();
  void init_command(const std::vector<std::string>& argv);
  void run_command (const std::vector<std::string>& argv);
  int run_child (const std::vector<std::string>& argv);
  void color_aliases (int idx);
  void create_windows(const std::vector<std::string>& argv);
  void get_colors ();
  void get_ourmods ();
  void set_icon (const char *file);
  // main.C
  void tt_winch ();
  rxvt_term ();
  ~rxvt_term ();
  void destroy ();
  void emergency_cleanup ();
  void recolor_cursor ();
  void resize_all_windows (unsigned int newwidth, unsigned int newheight, int ignoreparent);
  void window_calc (unsigned int newwidth, unsigned int newheight);
  bool set_fonts ();
  void set_string_property (Atom prop, const char *str, int len = -1);
  void set_mbstring_property (Atom prop, const char *str, int len = -1);
  void set_utf8_property (Atom prop, const char *str, int len = -1);
  void set_title (const char *str);
  void set_icon_name (const char *str);
  void set_window_color (int idx, const char *color);
  char *get_colorfgbg ();
  bool set_color (rxvt_color &color, const char *name);
  void alias_color (int dst, int src);
  void set_widthheight (unsigned int newwidth, unsigned int newheight);
  void get_window_origin (int &x, int &y);

  // screen.C

  int fgcolor_of (rend_t r) const
  {
    int base = GET_BASEFG (r);
#ifndef NO_BRIGHTCOLOR
    if (r & RS_Bold
# if ENABLE_STYLES
        && get_option(Opt_intensityStyles)
# endif
        && IN_RANGE_EXC (base, minCOLOR, minBrightCOLOR))
      base += minBrightCOLOR - minCOLOR;
#endif
    return base;
  }

  int bgcolor_of (rend_t r) const
  {
    int base = GET_BASEBG (r);
#ifndef NO_BRIGHTCOLOR
    if (r & RS_Blink
# if ENABLE_STYLES
        && get_option(Opt_intensityStyles)
# endif
        && IN_RANGE_EXC (base, minCOLOR, minBrightCOLOR))
      base += minBrightCOLOR - minCOLOR;
#endif
    return base;
  }

  // modifies first argument(!)
  void tt_paste (char *data, unsigned int len);
  void paste (char *data, unsigned int len);
  void scr_alloc ();
  void scr_blank_line (line_t &l, unsigned int col, unsigned int width, rend_t efs) const;
  void scr_blank_screen_mem (line_t &l, rend_t efs) const;
  void scr_kill_char (line_t &l, int col) const;
  void scr_set_char_rend (line_t &l, int col, rend_t rend);
  int scr_scroll_text (int row1, int row2, int count);
  void copy_line (line_t &dst, line_t &src);
  void scr_reset ();
  void scr_release ();
  void scr_clear (bool really = false);
  void scr_refresh ();
  bool scr_refresh_rend (rend_t mask, rend_t value);
  void scr_erase_screen (int mode);
#if ENABLE_FRILLS
  void scr_erase_savelines ();
  void scr_backindex ();
  void scr_forwardindex ();
#endif
  void scr_touch (bool refresh);
  void scr_expose (int x, int y, int width, int height, bool refresh);
  void scr_recolor (bool refresh = true);
  void scr_remap_chars ();
  void scr_remap_chars (line_t &l);

  enum cursor_mode { SAVE, RESTORE };

  void scr_poweron ();
  void scr_soft_reset ();
  void scr_cursor (cursor_mode mode);
  void scr_do_wrap ();
  void scr_swap_screen ();
  void scr_change_screen (int scrn);
  void scr_color (unsigned int color, int fgbg);
  void scr_color_24(unsigned int color, int fgbg);
  void scr_color_rgb(unsigned int r, unsigned int g, unsigned int b, int fgbg);
  rxvt_color& lookup_color(unsigned int color, rxvt_color* table);
  void scr_rendition (int set, rend_t style);
  void scr_add_lines (const wchar_t *str, int len, int minlines = 0);
  void scr_backspace ();
  void scr_tab (int count, bool ht = false);
  void scr_gotorc (int row, int col, int relative);
  void scr_index (enum page_dirn direction);
  void scr_erase_line (int mode);
  void scr_E ();
  void scr_insdel_lines (int count, int insdel);
  void scr_insdel_chars (int count, int insdel);
  void scr_scroll_region (int top, int bot);
  void scr_cursor_visible (int mode);
  void scr_autowrap (int mode);
  void scr_relative_origin (int mode);
  void scr_insert_mode (int mode);
  void scr_set_tab (int mode);
  void scr_rvideo_mode (bool on);
  void scr_report_position ();
  void scr_charset_choose (int set);
  void scr_charset_set (int set, unsigned int ch);
  void scr_move_to (int y, int len);
  bool scr_page (int nlines);
  bool scr_page (enum page_dirn direction, int nlines)
  {
    return scr_page (direction * nlines);
  }
  bool scr_changeview (int new_view_start);
  void scr_bell ();
  void scr_printscreen (int fullhist);
  void scr_xor_rect (int beg_row, int beg_col, int end_row, int end_col, rend_t rstyle1, rend_t rstyle2);
  void scr_xor_span (int beg_row, int beg_col, int end_row, int end_col, rend_t rstyle);
  void scr_reverse_selection ();
  void scr_dump (int fd);

  void selection_check (int check_more);
  void selection_changed (); /* sets want_refresh, corrects coordinates */
  void selection_request (Time tm, int selnum = Sel_Primary);
  void selection_clear (bool clipboard = false);
  void selection_make (Time tm);
  bool selection_grab (Time tm, bool clipboard = false);
  void selection_start_colrow (int col, int row);
  void selection_delimit_word (enum page_dirn dirn, const raxvt::coordinates* mark, raxvt::coordinates* ret);
  void selection_extend_colrow (int32_t col, int32_t row, int button3, int buttonpress, int clickchange);
  void selection_remove_trailing_spaces ();
  void selection_send (const XSelectionRequestEvent &rq);
  void selection_click (int clicks, int x, int y);
  void selection_extend (int x, int y, int flag);
  void selection_rotate (int x, int y);

  // settings.cpp
  const char* get_setting(int) const;
  void set_setting(int, const char*);
  void set_setting(int, const std::string&);
  bool get_option(int) const;
  void set_option(int, bool);
  void load_settings();

  // xdefaults.C
  void rxvt_usage (int type);
  std::vector<std::string> get_options(const std::vector<std::string>& argv);
  int parse_keysym (const char *str, unsigned int &state);
  int bind_action (const char *str, const char *arg);
  const char *x_resource (const char *name);
  void enumerate_resources (void (*cb)(rxvt_term *, const char *, const char *), const char *name_p = 0, const char *class_p = 0);
  void enumerate_keysym_resources (void (*cb)(rxvt_term *, const char *, const char *))
  {
    enumerate_resources (cb, "keysym", "Keysym");
  }
  void extract_keysym_resources ();

  void* operator new(std::size_t size);
  void operator delete(void* ptr);

private:
  std::unordered_map<int, std::string> m_settings;
  std::unordered_map<int, bool> m_options;
  std::string m_locale;
};

#endif /* _RXVT_H_ */

