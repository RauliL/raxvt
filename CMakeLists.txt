CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(raxvt C CXX)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckStructHasMember)
INCLUDE(FindPerl)
INCLUDE(FindGlib)
INCLUDE(FindGDK-PixBuf)
INCLUDE(FindFreetype)
INCLUDE(FindStartupNotification)

OPTION(
  EIGHT_BIT_CONTROLS
  "Define if you want 8 bit control sequences"
  OFF
)
OPTION(
  ENABLE_COMBINING
  "Define if you want to automatically compose combining characters"
  ON
)
OPTION(
  ENABLE_FRILLS
  "Define if you want handling for rarely used but handy features"
  ON
)
OPTION(
  ENABLE_PERL
  "Define if you can embed a Perl interpreter"
  ON
)
OPTION(
  ENABLE_STYLES
  "Define if you want bold and italic support"
  ON
)
OPTION(
  ENABLE_TRANSPARENCY
  "Define if you want your background to use the parent window background"
  ON
)
OPTION(
  ENCODING_EU
  "Define if you want european extended codesets"
  ON
)
OPTION(
  ENCODING_JP
  "Define if you want japanese codesets"
  ON
)
OPTION(
  ENCODING_JP_EXT
  "Define if you want extended japanese codesets"
  ON
)
OPTION(
  ENCODING_KR
  "Define if you want korean codesets"
  ON
)
OPTION(
  ENCODING_VN
  "Define if you want vietnamese codesets"
  ON
)
OPTION(
  ENCODING_ZH
  "Define if you want chinese codesets"
  ON
)
OPTION(
  ENCODING_ZH_EXT
  "Define if you want extended chinese codesets"
  ON
)
OPTION(
  ISO_14755
  "Define if you want ISO 14755 extended support"
  ON
)
OPTION(
  LASTLOG_SUPPORT
  "Define if you want to have lastlog support when utmp/utmpx is enabled"
  ON
)
OPTION(
  MOUSE_SLIP_WHEELING
  "Define to have CTRL cause wheel events to accelerate scrolling. Release CTRL
  to halt scrolling"
  ON
)
OPTION(
  MOUSE_WHEEL
  "Define to use wheel events (button4 and button5) to scroll"
  ON
)
OPTION(
  NDEBUG
  "Disable assertions (good for debugging)"
  ON
)
OPTION(
  NEXT_SCROLLBAR
  "Support NeXT style scrollbars"
  ON
)
OPTION(
  NO_BACKSPACE_KEY
  "Define if you don't want support for the backspace key"
  OFF
)
OPTION(
  NO_DELETE_KEY
  "Define if you don't want support for the (non-keypad) delete key"
  OFF
)
OPTION(
  NO_RESOURCES
  "Define if you don't want any resources read"
  OFF
)
OPTION(
  NO_SCROLLBAR_BUTTON_CONTINUAL_SCROLLING
  "Define for continual scrolling when you keep the scrollbar button pressed"
  OFF
)
OPTION(
  NO_SECONDARY_SCREEN
  "Disable the secondary screen. Many programs use the secondary screen as
  their workspace"
  OFF
)
OPTION(
  OFF_FOCUS_FADING
  "Define if you want faded colors when focus is lost"
  ON
)
OPTION(
  PLAIN_SCROLLBAR
  "Support plain style scrollbars"
  ON
)
OPTION(
  POINTER_BLANK
  "Define if you want to hide the pointer while typing"
  ON
)
OPTION(
  RXVT_SCROLLBAR
  "Support Rxvt original style scrollbars"
  ON
)
OPTION(
  RXVT_TERMINFO
  "Set TERMINFO value to the value given by configure"
  OFF
)
OPTION(
  SELECTION_SCROLLING
  "Define to allow scrolling when the selection moves to the top or bottom of
  the screen"
  ON
)
OPTION(
  SMART_RESIZE
  "Define to use \"smart\" resize behavior"
  OFF
)
OPTION(
  STDC_HEADERS
  "Define to 1 if you have the ANSI C header files"
  ON
)
OPTION(
  TERMENV
  "Set the TERM to the value given by configure"
  OFF
)
OPTION(
  TEXT_BLINK
  "Define if you want blinking text support"
  ON
)
OPTION(
  TTY_GID_SUPPORT
  "TODO: Find out what this does"
  OFF
)
OPTION(
  UNICODE_3
  "Define if you want to represent unicode characters outside plane 0"
  OFF
)
OPTION(
  UNIX98_PTY
  "TODO: Find out what this does and remove it"
  ON
)
OPTION(
  USE_256_COLORS
  "Define if you want 256-color support"
  ON
)
OPTION(
  HAVE_PIXBUF
  "Define if you want to use gdk-pixbuf for image processing"
  ON
)
OPTION(
  HAVE_STARTUP_NOTIFICATION
  "Define if freedesktop startup notifications should be supported"
  ON
)
OPTION(
  HAVE_XMU
  "Define to enable XMU support"
  ON
)

CHECK_INCLUDE_FILES(fontconfig/fongconfig.h HAVE_FONTCONFIG_FONTCONFIG_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(lastlog.h HAVE_LASTLOG_H)
CHECK_INCLUDE_FILES(libutil.h HAVE_LIBUTIL_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(poll.h HAVE_POLL_H)
CHECK_INCLUDE_FILES(port.h HAVE_PORT_H)
CHECK_INCLUDE_FILES(pty.h HAVE_PTY_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(stropts.h HAVE_STROPTS_H)
CHECK_INCLUDE_FILES(sys/byteorder.h HAVE_SYS_BYTEORDER_H)
CHECK_INCLUDE_FILES(sys/epoll.h HAVE_SYS_EPOLL_H)
CHECK_INCLUDE_FILES(sys/eventfd.h HAVE_SYS_EVENTFD_H)
CHECK_INCLUDE_FILES(sys/event.h HAVE_SYS_EVENT_H)
CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(sys/select.h HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILES(sys/signalfd.h HAVE_SYS_SIGNALFD_H)
CHECK_INCLUDE_FILES(sys/sockio.h HAVE_SYS_SOCKIO_H)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/strredir.h HAVE_SYS_STRREDIR_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(util.h HAVE_UTIL_H)
CHECK_INCLUDE_FILES(utmpx.h HAVE_UTMPX_H)
CHECK_INCLUDE_FILES(utmp.h HAVE_UTMP_H)
CHECK_INCLUDE_FILES(wchar.h HAVE_WCHAR_H)
CHECK_INCLUDE_FILES(X11/Xft/Xft.h HAVE_X11_XFT_XFT_H)

CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
CHECK_FUNCTION_EXISTS(epoll_ctl HAVE_EPOLL_CTL)
CHECK_FUNCTION_EXISTS(eventfd HAVE_EVENTFD)
CHECK_FUNCTION_EXISTS(FcPatternGet HAVE_FCPATTERNGET)
CHECK_FUNCTION_EXISTS(floor HAVE_FLOOR)
CHECK_FUNCTION_EXISTS(getpt HAVE_GETPT)
CHECK_FUNCTION_EXISTS(inotify_init HAVE_INOTIFY_INIT)
CHECK_FUNCTION_EXISTS(isastream HAVE_ISASTREAM)
CHECK_FUNCTION_EXISTS(kqueue HAVE_KQUEUE)
CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)
CHECK_FUNCTION_EXISTS(nl_langinfo HAVE_NL_LANGINFO)
CHECK_FUNCTION_EXISTS(openpty HAVE_OPENPTY)
CHECK_FUNCTION_EXISTS(poll HAVE_POLL)
CHECK_FUNCTION_EXISTS(port_create HAVE_PORT_CREATE)
CHECK_FUNCTION_EXISTS(posix_openpt HAVE_POSIX_OPENPT)
CHECK_FUNCTION_EXISTS(revoke HAVE_REVOKE)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(seteuid HAVE_SETEUID)
CHECK_FUNCTION_EXISTS(setresuid HAVE_SETRESUID)
CHECK_FUNCTION_EXISTS(setreuid HAVE_SETREUID)
CHECK_FUNCTION_EXISTS(setuid HAVE_SETUID)
CHECK_FUNCTION_EXISTS(signalfd HAVE_SIGNALFD)
CHECK_FUNCTION_EXISTS(unsetenv HAVE_UNSETENV)
CHECK_FUNCTION_EXISTS(updlastlogx HAVE_UPDLASTLOGX)
CHECK_FUNCTION_EXISTS(updwtmp HAVE_UPDWTMP)
CHECK_FUNCTION_EXISTS(updwtmpx HAVE_UPDWTMPX)
CHECK_FUNCTION_EXISTS(XftDrawString32 HAVE_XFTDRAWSTRING32)
CHECK_FUNCTION_EXISTS(xsetlocale HAVE_XSETLOCALE)
CHECK_FUNCTION_EXISTS(_getpty HAVE__GETPTY)

CHECK_LIBRARY_EXISTS(rt aio_return "/lib" HAVE_LIBRT)

CHECK_SYMBOL_EXISTS(lastlog utmp.h HAVE_STRUCT_LASTLOG)
CHECK_SYMBOL_EXISTS(lastlogx utmpx.h HAVE_STRUCT_LASTLOGX)
CHECK_SYMBOL_EXISTS(utmp utmp.h HAVE_STRUCT_UTMP)
CHECK_SYMBOL_EXISTS(utmpx utmpx.h HAVE_STRUCT_UTMPX)
CHECK_SYMBOL_EXISTS(fpdass sys/socket.h HAVE_UNIX_FDPASS)

CHECK_STRUCT_HAS_MEMBER(utmpx ut_host utmpx.h HAVE_UTMPX_HOST)
CHECK_STRUCT_HAS_MEMBER(utmp ut_host utmp.h HAVE_UTMP_HOST)
CHECK_STRUCT_HAS_MEMBER(utmp pid utmp.h HAVE_UTMP_PID)

# TODO: Look for Perl if the option has been set.

IF(NOT PERL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find Perl.")
ENDIF()

CONFIGURE_FILE(
  "${CMAKE_SOURCE_DIR}/config.h.cmake"
  "${CMAKE_SOURCE_DIR}/src/config.h"
)

INCLUDE_DIRECTORIES(
  "${PERL_INCLUDE_PATH}"
  "${Glib_INCLUDE_DIRS}"
  "${GDK-PixBuf_INCLUDE_DIRS}"
  "${Freetype_INCLUDE_DIRS}"
  "${StartupNotification_INCLUDE_DIRS}"
  "${CMAKE_SOURCE_DIR}/libptytty/src"
  "${CMAKE_SOURCE_DIR}/src"
)

SET(
  SOURCES_COMMON
  ./src/command.C
  ./src/encoding.C
  ./src/ev_cpp.C
  ./src/fdpass_wrapper.C
  ./src/init.C
  ./src/keyboard.C
  ./src/main.C
  ./src/misc.C
  ./src/ptytty_wrapper.C
  ./src/rxvtfont.C
  ./src/rxvtimg.C
  #./src/rxvtperl.C
  ./src/rxvttoolkit.C
  ./src/rxvtutil.C
  ./src/screen.C
  ./src/scrollbar-next.C
  ./src/scrollbar-plain.C
  ./src/scrollbar-rxvt.C
  ./src/scrollbar-xterm.C
  ./src/scrollbar.C
  ./src/xdefaults.C
)

SET(
  SOURCES_COMMON_DAEMON
  ./src/rxvtdaemon.C
)

ADD_EXECUTABLE(
  raxvt
  ${SOURCES_COMMON}
)

TARGET_LINK_LIBRARIES(
  raxvt
  "${PERL_LIBRARY}"
  "${Glib_LIBRARIES}"
  "${GDK-PixBuf_LIBRARIES}"
  "${Freetype_LIBRARIES}"
)

ADD_EXECUTABLE(
  raxvtd
  ${SOURCES_COMMON}
  ${SOURCES_COMMON_DAEMON}
)

ADD_EXECUTABLE(
  raxvtc
  ${SOURCES_COMMON_DAEMON}
  ./src/fdpass_wrapper.C
)
