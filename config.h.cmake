#cmakedefine XFT 1
#cmakedefine UTMP_SUPPORT 1
#cmakedefine WTMP_SUPPORT 1

// Define if you want to use gdk-pixbuf for image processing.
#cmakedefine HAVE_PIXBUF 1
#cmakedefine HAVE_STARTUP_NOTIFICATION 1

// Header existance information.
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_LASTLOG_H 1
#cmakedefine HAVE_LIBUTIL_H
#cmakedefine HAVE_MEMORY_H 1
#cmakedefine HAVE_POLL_H 1
#cmakedefine HAVE_PORT_H 1
#cmakedefine HAVE_PTY_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_STRING_H 1
#cmakedefine HAVE_STROPTS_H 1
#cmakedefine HAVE_SYS_BYTEORDER_H 1
#cmakedefine HAVE_SYS_EPOLL_H 1
#cmakedefine HAVE_SYS_EVENTFD_H 1
#cmakedefine HAVE_SYS_EVENT_H 1
#cmakedefine HAVE_SYS_INOTIFY_H 1
#cmakedefine HAVE_SYS_IOCTL_H 1
#cmakedefine HAVE_SYS_SELECT_H 1
#cmakedefine HAVE_SYS_SIGNALFD_H 1
#cmakedefine HAVE_SYS_SOCKIO_H 1
#cmakedefine HAVE_SYS_STAT_H 1
#cmakedefine HAVE_SYS_STRREDIR_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine HAVE_UTIL_H 1
#cmakedefine HAVE_UTMPX_H 1
#cmakedefine HAVE_UTMP_H 1
#cmakedefine HAVE_WCHAR_H 1
#cmakedefine HAVE_X11_XFT_XFT_H 1

// Function existance information.
#cmakedefine HAVE_CLOCK_GETTIME 1
#cmakedefine HAVE_CLOCK_SYSCALL 1
#cmakedefine HAVE_EPOLL_CTL 1
#cmakedefine HAVE_EVENTFD 1
#cmakedefine HAVE_FCPATTERNGET 1
#cmakedefine HAVE_FLOOR 1
#cmakedefine HAVE_GETPT 1
#cmakedefine HAVE_INOTIFY_INIT 1
#cmakedefine HAVE_ISASTREAM 1
#cmakedefine HAVE_KQUEUE 1
#cmakedefine HAVE_NANOSLEEP 1
#cmakedefine HAVE_NL_LANGINFO 1
#cmakedefine HAVE_OPENPTY 1
#cmakedefine HAVE_POLL 1
#cmakedefine HAVE_PORT_CREATE 1
#cmakedefine HAVE_POSIX_OPENPT 1
#cmakedefine HAVE_REVOKE 1
#cmakedefine HAVE_SELECT 1
#cmakedefine HAVE_SETEUID 1
#cmakedefine HAVE_SETRESUID 1
#cmakedefine HAVE_SETREUID 1
#cmakedefine HAVE_SETUID 1
#cmakedefine HAVE_SIGNALFD 1
#cmakedefine HAVE_UNSETENV 1
#cmakedefine HAVE_UPDLASTLOGX 1
#cmakedefine HAVE_UPDWTMP 1
#cmakedefine HAVE_UPDWTMPX 1
#cmakedefine HAVE_XFTDRAWSTRING 132

// C structure existance information
#cmakedefine HAVE_STRUCT_LASTLOG 1
#cmakedefine HAVE_STRUCT_LASTLOGX 1
#cmakedefine HAVE_STRUCT_UTMP 1
#cmakedefine HAVE_STRUCT_UTMPX 1
#cmakedefine HAVE_UTMPX_HOST 1
#cmakedefine HAVE_UTMP_HOST 1
#cmakedefine HAVE_UTMP_PID 1

// Required by libptytty
#cmakedefine UNIX98_PTY 1

// Resource class. TODO: Remove this eventually.
#define RESCLASS "raxvt"

// Default resource name. TODO: Remove this eventually.
#define RESNAME "raxvt"

// Binary base name. TODO: Remove this eventually.
#define RXVTNAME "raxvt"
