#include <config.h>
#include "rxvt.h"

#define PTYTTY_REENTRANT 0

#define PTYTTY_FATAL rxvt_fatal
#define PTYTTY_WARN  rxvt_warn

#include "./ptytty_logging.cpp"
#include "./ptytty_proxy.cpp"
#include "./ptytty.cpp"
