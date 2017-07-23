# - Try to find Startup Notification
# Once done, this will define
#
#  StartupNotification_FOUND - system has Startup Notification
#  StartupNotification_INCLUDE_DIRS - the Startup Notification include
#                                     directories
#  StartupNotification_LIBRARIES - Link these to use Startup Notification

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(
  StartupNotification_PKGCONF
  libstartup-notification-1.0
)

# Include dir
find_path(StartupNotification_INCLUDE_DIR
  NAMES libsn/sn-common.h
  HINTS ${StartupNotification_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES startup-notification-1.0
)

# Finally the library itself
#find_library(StartupNotification_LIBRARY
#  NAMES freetype
#  HINTS ${Freetype_PKGCONF_LIBRARY_DIRS}
#)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(StartupNotification_PROCESS_INCLUDES Freetype_INCLUDE_DIR)
#set(Freetype_PROCESS_LIBS Freetype_LIBRARY)
libfind_process(StartupNotification)
