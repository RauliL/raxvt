SET(
  SOURCES_COMMON
  command.cpp
  encoding.cpp
  ev_cpp.cpp
  fdpass_wrapper.cpp
  init.cpp
  keyboard.cpp
  main.cpp
  misc.cpp
  ptytty_wrapper.cpp
  rxvtfont.cpp
  rxvtimg.cpp
  rxvttoolkit.cpp
  rxvtutil.cpp
  screen.cpp
  scrollbar-next.cpp
  scrollbar-plain.cpp
  scrollbar-rxvt.cpp
  scrollbar-xterm.cpp
  scrollbar.cpp
  settings.cpp
)

SET(
  SOURCES_COMMON_DAEMON
  rxvtdaemon.cpp
)

ADD_EXECUTABLE(
  raxvt
  ${SOURCES_COMMON}
  rxvt.cpp
)

ADD_EXECUTABLE(
  raxvtd
  ${SOURCES_COMMON}
  ${SOURCES_COMMON_DAEMON}
  rxvtd.cpp
)

ADD_EXECUTABLE(
  raxvtc
  ${SOURCES_COMMON}
  ${SOURCES_COMMON_DAEMON}
  rxvtc.cpp
)

TARGET_LINK_LIBRARIES(
  raxvt
  "${X11_LIBRARIES}"
  "${GDKPIXBUF_LIBRARIES}"
  "${XFT_LIBRARIES}"
  "${FONTCONFIG_LIBRARIES}"
  "${XRENDER_LIBRARIES}"
  "${STARTUP_NOTIFICATION_LIBRARIES}"
)

TARGET_LINK_LIBRARIES(
  raxvtd
  "${X11_LIBRARIES}"
  "${XFT_LIBRARIES}"
  "${FONTCONFIG_LIBRARIES}"
  "${XRENDER_LIBRARIES}"
  "${STARTUP_NOTIFICATION_LIBRARIES}"
)

TARGET_LINK_LIBRARIES(
  raxvtc
  "${X11_LIBRARIES}"
  "${XFT_LIBRARIES}"
  "${FONTCONFIG_LIBRARIES}"
  "${XRENDER_LIBRARIES}"
  "${STARTUP_NOTIFICATION_LIBRARIES}"
)
