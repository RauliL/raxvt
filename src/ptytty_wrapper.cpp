#include <config.h>
#include "rxvt.h"

#define PTYTTY_REENTRANT 0

#define PTYTTY_FATAL rxvt_fatal
#define PTYTTY_WARN  rxvt_warn

#include "../deps/libptytty/src/logging.C"
#include "../deps/libptytty/src/proxy.C"
#include "../deps/libptytty/src/ptytty.C"
