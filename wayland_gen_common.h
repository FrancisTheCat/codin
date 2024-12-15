#pragma once
#include "codin.h"

typedef struct {
  Builder    builder;
  Vector(Fd) fds;
  i32        current_id;
  Socket     socket;
} Wayland_Connection;

#define roundup_4(n) (((n) + 3) & -4)

// #define wayland_log_infof fmt_eprintfln
#define wayland_log_infof(format, ...)

