#pragma once
#include "codin.h"

typedef struct {
  Builder     builder;
  Vector(Fd)  fds_in;
  Vector(Fd)  fds_out;
  Vector(u32) free_ids;
  Byte_Slice  buffer;
  u32         start, end;
  i32         current_id;
  Socket      socket;
} Wayland_Connection;

#define roundup_4(n) (((n) + 3) & -4)

// #define wayland_log_infof fmt_eprintfln
#define wayland_log_infof(format, ...)

u32 _wayland_connection_get_next_id(Wayland_Connection *conn);

