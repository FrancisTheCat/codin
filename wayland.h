#include "codin.h"
#include "xkb.h"

#include "wayland-gen/wayland.h"
#include "wayland-gen/xdg-shell.h"
#include "wayland-gen/cursor-shape.h"

#include "ui.h"

#include "spall.h"

SpallBuffer spall_buffer;
SpallProfile spall_ctx;

// #define spall_buffer_begin(...)
// #define spall_buffer_end(...)

f64 get_time_in_micros() {
  return (f64)(time_now().nsec) / Microsecond;
}

#define WAYLAND_HEADER_SIZE 8
#define COLOR_CHANNELS      4

#define CMSG_DATA(cmsg) ((void *)((struct cmsghdr *)cmsg + 1))

#define CMSG_NXTHDR(mhdr, cmsg) __cmsg_nxthdr(mhdr, cmsg)

#define CMSG_FIRSTHDR(mhdr)                                                    \
  ((usize)(mhdr)->controllen >= size_of(struct cmsghdr)                        \
       ? (struct cmsghdr *)(mhdr)->control                                     \
       : (struct cmsghdr *)0)

#define CMSG_ALIGN(len)                                                        \
  (((len) + size_of(usize) - 1) & (usize) ~(size_of(usize) - 1))

#define CMSG_SPACE(len) (CMSG_ALIGN(len) + CMSG_ALIGN(size_of(struct cmsghdr)))

#define CMSG_LEN(len) (CMSG_ALIGN(size_of(struct cmsghdr)) + (len))

#define __CMSG_PADDING(len)                                                    \
  ((size_of(usize) - ((len) & (size_of(usize) - 1))) & (size_of(usize) - 1))

struct cmsghdr {
  u64 len;    /* Data byte count, including header (type is socklen_t in POSIX) */
  i32 level;  /* Originating protocol */
  i32 type;   /* Protocol-specific type */
};

struct iovec {
  rawptr base;
  usize  len;
};

struct msghdr {
  rawptr        name;       /* Optional address */
  u32           namelen;    /* Size of address */
  struct iovec *iov;        /* Scatter/gather array */
  usize         iovlen;     /* # elements in msg_iov */
  void         *control;    /* Ancillary data, see below */
  usize         controllen; /* Ancillary data buffer len */
  i32           flags;      /* Flags (unused) */
};

struct cmsghdr * __cmsg_nxthdr(struct msghdr *__mhdr, struct cmsghdr *__cmsg) {

  unsigned char * __msg_control_ptr = (unsigned char *) __mhdr->control;
  unsigned char * __cmsg_ptr = (unsigned char *) __cmsg;

  usize __size_needed = sizeof (struct cmsghdr)
                         + __CMSG_PADDING (__cmsg->len);

  /* The current header is malformed, too small to be a full header.  */
  if ((usize) __cmsg->len < sizeof (struct cmsghdr))
    return (struct cmsghdr *) 0;

  /* There isn't enough space between __cmsg and the end of the buffer to
  hold the current cmsg *and* the next one.  */
  if (((usize)
         (__msg_control_ptr + __mhdr->controllen - __cmsg_ptr)
       < __size_needed)
      || ((usize)
            (__msg_control_ptr + __mhdr->controllen - __cmsg_ptr
             - __size_needed)
          < __cmsg->len))

    return (struct cmsghdr *) 0;

  /* Now, we trust cmsg_len and can use it to find the next header.  */
  __cmsg = (struct cmsghdr *) ((unsigned char *) __cmsg
			       + CMSG_ALIGN (__cmsg->len));
  return __cmsg;
}

internal void wayland_connection_flush(Wayland_Connection *conn) {
  if (conn->builder.len) {
    Byte_Slice control_buf;
    slice_init(&control_buf, CMSG_SPACE(conn->fds.len * size_of(int)), context.temp_allocator);
    struct iovec iov = {
      .base = conn->builder.data,
      .len  = (usize)conn->builder.len,
    };
    struct msghdr msg = {
      .name       = nil,
      .namelen    = 0,
      .iov        = &iov,
      .iovlen     = 1,
      .control    = control_buf.data,
      .controllen = (usize)control_buf.len,
    };

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->level = SOL_SOCKET;
    cmsg->type  = 1;
    cmsg->len   = CMSG_LEN(conn->fds.len * size_of(int));
    mem_copy(CMSG_DATA(cmsg), conn->fds.data, conn->fds.len * size_of(int));

    syscall(SYS_sendmsg, conn->socket, &msg, MSG_NOSIGNAL);

    vector_clear(&conn->builder);
    vector_clear(&conn->fds);
  }
}

internal void wayland_connection_destroy(Wayland_Connection const *conn) {
  vector_delete(conn->builder);
  vector_delete(conn->fds);
}

internal Wayland_Connection wayland_display_connect(Allocator allocator) {
  struct sockaddr_un {
    u16 family;
    char path[108];
  };

  Wayland_Connection conn = {0};
  conn.current_id = 1;
  vector_init(&conn.builder, 0, 8, allocator);
  vector_init(&conn.fds, 0, 8, allocator);

  String xdg_runtime_dir = get_env(LIT("XDG_RUNTIME_DIR"));
  String wayland_display = get_env(LIT("WAYLAND_DISPLAY"));

  String socket_path = fmt_tprintf(LIT("%S/%S"), xdg_runtime_dir, wayland_display);
  struct sockaddr_un addr = {0};
  addr.family = 1;
  string_copy((String) {.data = addr.path, .len = size_of(addr.path) - 1}, socket_path);

  conn.socket = syscall(SYS_socket, 1, SOCK_STREAM, 0);
  assert(conn.socket != -1);

  isize connect_status = syscall(SYS_connect, conn.socket, &addr, size_of(addr));
  assert(connect_status != -1)

  return conn;
}

typedef enum {
  Surface_State_None = 0,
  Surface_State_Acked_Configure,
  Surface_State_Attached,
} Surface_State;

typedef enum {
  Buffer_State_Released = 0,
  Buffer_State_Busy,
} Buffer_State;

typedef struct {
  u32           wl_registry;
  u32           wl_seat;
  u32           wl_keyboard;
  u32           wl_pointer;
  u32           wl_shm;
  u32           wl_shm_pool;
  u32           wl_buffer;
  u32           xdg_wm_base;
  u32           xdg_surface;
  u32           wl_compositor;
  u32           wl_surface;
  u32           xdg_toplevel;
  u32           wp_cursor_shape_manager;
  u32           wp_cursor_shape_device;
  u32           stride;
  i32           w;
  i32           h;
  u32           shm_pool_size;
  i32           shm_fd;
  u8           *shm_pool_data;
  f32           mouse_x;
  f32           mouse_y;
  b8            mouse_buttons[2];
  Surface_State surface_state;
  Buffer_State  buffer_state;
  b8            should_close;
  b8            should_resize;
  Vector(Fd)    fds_in;
  Keymap        keymap;
  b8            keys[Key_MAX_VALUE];
} Wayland_State;

internal b8 create_shared_memory_file(uintptr size, Wayland_State *state) {
  #define MFD_CLOEXEC       0x0001U
  #define MFD_ALLOW_SEALING	0x0002U
  #define MFD_HUGETLB       0x0004U
  /* not executable and sealed to prevent changing to executable. */
  #define MFD_NOEXEC_SEAL    0x0008U
  /* executable */
  #define MFD_EXEC		0x0010U
  Fd fd = syscall(SYS_memfd_create, "/wayland_framebuffer_memfd", MFD_ALLOW_SEALING);
  if (fd == -1) {
    return false;
  }
  
  isize t = syscall(SYS_ftruncate, fd, size);
  if (t != 0) {
    return false;
  }

  #define F_LINUX_SPECIFIC_BASE	1024
  #define F_ADD_SEALS	(F_LINUX_SPECIFIC_BASE + 9)
  #define F_GET_SEALS	(F_LINUX_SPECIFIC_BASE + 10)

  /*
   * Types of seals
   */
  #define F_SEAL_SEAL	0x0001	/* prevent further seals from being set */
  #define F_SEAL_SHRINK	0x0002	/* prevent file from shrinking */
  #define F_SEAL_GROW	0x0004	/* prevent file from growing */
  #define F_SEAL_WRITE	0x0008	/* prevent writes */
  #define F_SEAL_FUTURE_WRITE	0x0010  /* prevent future writes while mapped */
  #define F_SEAL_EXEC	0x0020  /* prevent chmod modifying exec bits */

  t = syscall(SYS_fcntl, F_ADD_SEALS, F_SEAL_SHRINK);
  
  state->shm_pool_data =
    (u8 *)syscall(SYS_mmap, nil, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (state->shm_pool_data == (rawptr)-1) {
    return false;
  }
  state->shm_fd = fd;

  return true;
}

internal isize wayland_handle_message(Wayland_Connection *conn, Wayland_State *state, Byte_Slice data) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  if (data.len < 8) {
    fmt_panicf(LIT("Invalid len: %d"), data.len);
  }

  Byte_Slice data_copy = data;

  isize data_len = data.len;

  Reader reader = buffer_reader(&data);

  u32 object_id;
  read_any(&reader, &object_id);

  u16 opcode;
  read_any(&reader, &opcode);

  u16 announced_size;
  read_any(&reader, &announced_size);

  if (object_id == state->wl_registry && opcode == Wayland_Wl_Registry_Event_Global) {
    u32 name, version;
    String interface;

    wayland_parse_event_wl_registry_global(data_copy, &name, &interface, &version, context.temp_allocator);

    if (string_equal(interface, LIT("wl_shm"))) {
      state->wl_shm = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("xdg_wm_base"))) {
      state->xdg_wm_base = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("wl_compositor"))) {
      state->wl_compositor = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("wl_seat"))) {
      state->wl_seat = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("wp_cursor_shape_manager_v1"))) {
      state->wp_cursor_shape_manager = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
    }

  } else if (object_id == state->xdg_wm_base && opcode == Wayland_Xdg_Wm_Base_Event_Ping) {
    u32 serial;
    wayland_parse_event_xdg_wm_base_ping(data_copy, &serial);
    wayland_xdg_wm_base_pong(conn, state->xdg_wm_base, serial);

  } else if (object_id == state->xdg_surface && opcode == Wayland_Xdg_Surface_Event_Configure) {
    u32 serial;
    wayland_parse_event_xdg_surface_configure(data_copy, &serial);
    wayland_xdg_surface_ack_configure(conn, state->xdg_surface, serial);
    state->surface_state = Surface_State_Acked_Configure;

  } else if (object_id == state->xdg_toplevel) {
    switch ((Wayland_Xdg_Toplevel_Event)opcode) {
    case Wayland_Xdg_Toplevel_Event_Configure_Bounds:
      i32 width;
      i32 height;
      wayland_parse_event_xdg_toplevel_configure_bounds(data_copy, &width, &height);
      break;

    case Wayland_Xdg_Toplevel_Event_Configure:
      Byte_Slice states;
      wayland_parse_event_xdg_toplevel_configure(data_copy, &state->w, &state->h, &states, context.temp_allocator);

      state->w = max(state->w, 1);
      state->h = max(state->h, 1);

      state->should_resize = true;
      break;

    case Wayland_Xdg_Toplevel_Event_Close:
      state->should_close = true;
      break;

    default:
      break;
    }

  } else if (object_id == state->wl_buffer && opcode == Wayland_Wl_Buffer_Event_Release) {
    wayland_parse_event_wl_buffer_release(data_copy);
    state->buffer_state = Buffer_State_Released;

  } else if (object_id == state->wl_keyboard) {
    switch ((Wayland_Wl_Keyboard_Event)opcode) {
    case Wayland_Wl_Keyboard_Event_Keymap:
      u32 format, size;
      Fd fd;
      rawptr data;
      Byte_Slice kmdata;

      read_any(&reader, &format);
      read_any(&reader, &size);

      if (!state->fds_in.len) {
        log_fatal(LIT("FUCK (we should have recieved an fd for this event, but we didnt, this is probably our fault)"));
        trap();
      }

      fd = state->fds_in.data[0];
      vector_remove_ordered(&state->fds_in, 0);

      wayland_log_infof(LIT("Keymap %d %B %d"), size, format, fd);

      data = (rawptr)syscall(SYS_mmap, nil, size, PROT_READ, MAP_PRIVATE, fd, 0);

      kmdata = (Byte_Slice) {
        .data = (byte *)data,
        .len  = size,
      };
      write_entire_file_path(LIT("keymap.txt"), kmdata);

      parse_key_codes(transmute(String, kmdata), &state->keymap, context.allocator);
      break;

    case Wayland_Wl_Keyboard_Event_Key:
      u32 serial;
      u32 time;
      u32 key;
      Wayland_Wl_Keyboard_Key_State key_state;

      wayland_parse_event_wl_keyboard_key(data_copy, &serial, &time, &key, &key_state);

      if (key + 8 < state->keymap.len) {
        if (state->keymap.data[key + 8]) {
          state->keys[state->keymap.data[key + 8]] = key_state == Wayland_Wl_Keyboard_Key_State_Pressed;
        }

        if (state->keymap.data[key + 8] == Key_Escape && key_state == Wayland_Wl_Keyboard_Key_State_Pressed) {
          state->should_close = true;
        }

        fmt_println(get_key_name(state->keymap.data[key + 8]));
      }
      break;

    case Wayland_Wl_Keyboard_Event_Enter:
      u32 surface;
      Byte_Slice keys;

      wayland_parse_event_wl_keyboard_enter(data_copy, &serial, &surface, &keys, context.temp_allocator);

      break;
      
    default:
      break;
    }

  } else if (object_id == state->wl_pointer) {
    u32 serial, time, button;
    Wayland_Wl_Pointer_Axis axis;
    Wayland_Wl_Pointer_Button_State button_state;
    f32 x, y, value;
    u32 surface;

    switch ((Wayland_Wl_Pointer_Event)opcode) {
    case Wayland_Wl_Pointer_Event_Motion:
      wayland_parse_event_wl_pointer_motion(data_copy, &time, &x, &y);

      state->mouse_x = x;
      state->mouse_y = y;

      break;
    case Wayland_Wl_Pointer_Event_Button:
      wayland_parse_event_wl_pointer_button(data_copy, &serial, &time, &button, &button_state);

      #define BTN_MOUSE   0x110
      #define BTN_LEFT    0x110
      #define BTN_RIGHT   0x111
      #define BTN_MIDDLE  0x112
      #define BTN_SIDE    0x113
      #define BTN_EXTRA   0x114
      #define BTN_FORWARD 0x115
      #define BTN_BACK    0x116
      #define BTN_TASK    0x117

      switch (button) {
      case BTN_LEFT:
        state->mouse_buttons[0] = button_state == Wayland_Wl_Pointer_Button_State_Pressed;
        break;
      case BTN_RIGHT:
        state->mouse_buttons[1] = button_state == Wayland_Wl_Pointer_Button_State_Pressed;
        break;
      }

      break;
    case Wayland_Wl_Pointer_Event_Axis:
      wayland_parse_event_wl_pointer_axis(data_copy, &time, &axis, &value);

      break;

    case Wayland_Wl_Pointer_Event_Enter:
      wayland_parse_event_wl_pointer_enter(data_copy, &serial, &surface, &x, &y);

      if (state->wp_cursor_shape_manager) {
        if (!state->wp_cursor_shape_device) {
          state->wp_cursor_shape_device = wayland_wp_cursor_shape_manager_v1_get_pointer(conn, state->wp_cursor_shape_manager, state->wl_pointer);
        }

        wayland_wp_cursor_shape_device_v1_set_shape(conn, state->wp_cursor_shape_device, serial, Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default);
      }
      break;

    default:
      break;
    }
  } else if (object_id == state->wl_seat) {
    if (opcode == Wayland_Wl_Seat_Event_Capabilities) {
      Wayland_Wl_Seat_Capability capabilities;
      wayland_parse_event_wl_seat_capabilities(data_copy, &capabilities);
    }

  } else if (object_id == 1 && opcode == Wayland_Wl_Display_Event_Error) {
    u32 target_object_id, code;
    String message;
    wayland_parse_event_wl_display_error(data_copy, &target_object_id, &code, &message, context.temp_allocator);
    log_fatalf(LIT("Fatal error: object = %d, code = %d, error = '%S'"), target_object_id, code, message);
    trap();
  }

  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
  return announced_size;
}

internal void wayland_draw_rect_outlines(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 color) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  u32 *pixels = (u32 *)state->shm_pool_data;

  if (in_range(x, 0, state->w)) {
    for_range(_y, max(y, 0), min(y + h, state->h)) {
      pixels[x + _y * state->w] = color;
    }
  }

  if (in_range(x + w - 1, 0, state->w) && w) {
    for_range(_y, max(y, 0), min(y + h, state->h)) {
      pixels[x + w - 1 + _y * state->w] = color;
    }
  }

  if (in_range(y, 0, state->h)) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      pixels[_x + y * state->w] = color;
    }
  }
  if (in_range(y + h - 1, 0, state->h) && h) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      pixels[_x + (y + h - 1) * state->w] = color;
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void alpha_blend_rgb8(u32 *_dst, u32 _src, u8 _alpha) {
  if (!_alpha) {
    return;
  }
  if (_alpha == 0xFF) {
    *_dst = _src;
    return;
  }
  Array(u8, 4)  src = transmute(type_of(src), _src);
  Array(u8, 4) *dst = transmute(type_of(dst), _dst);
  for_range(i, 0, 4) {
    dst->data[i] = (((u16)dst->data[i] * (u16)(255 - (u16)_alpha)) >> 8) +
                   (((u16)src.data[i]  * (u16)(      (u16)_alpha)) >> 8);
  }
}

internal void alpha_blend_rgba8(u32 *dst, u32 src) {
  alpha_blend_rgb8(dst, src, src >> 24);
}

internal void wayland_draw_rect_outlines_alpha(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 color) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  if ((color & 0xFF) == 0xFF) {
    wayland_draw_rect_outlines(state, x, y, w, h, color);
    return;
  }
  u32 *pixels = (u32 *)state->shm_pool_data;

  if (in_range(x, 0, state->w)) {
    for_range(_y, max(y, 0), min(y + h, state->h)) {
      alpha_blend_rgba8(&pixels[x + _y * state->w], color);
    }
  }

  if (in_range(x + w - 1, 0, state->w) && w) {
    for_range(_y, max(y, 0), min(y + h, state->h)) {
      alpha_blend_rgba8(&pixels[x + w - 1 + _y * state->w], color);
    }
  }

  if (in_range(y, 0, state->h)) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      alpha_blend_rgba8(&pixels[_x + y * state->w], color);
    }
  }
  if (in_range(y + h - 1, 0, state->h) && h) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      alpha_blend_rgba8(&pixels[_x + (y + h - 1) * state->w], color);
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_rect(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 color) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  u32 *pixels = (u32 *)state->shm_pool_data;
  for_range(_y, max(y, 0), min(y + h, state->h)) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      pixels[_x + _y * state->w] = color;
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_rect_alpha(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 color) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  if ((color >> 24) == 0xFF) {
    wayland_draw_rect(state, x, y, w, h, color);
    return;
  }
  u32 *pixels = (u32 *)state->shm_pool_data;
  for_range(_y, max(y, 0), min(y + h, state->h)) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      alpha_blend_rgba8(&pixels[_x + _y * state->w], color);
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_rect_gradient_v(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 _start, u32 _end) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  u32 *pixels = (u32 *)state->shm_pool_data;

  Array(u8, 4) start = transmute(type_of(start), _start);
  Array(u8, 4) end   = transmute(type_of(end),   _end  );
  Array(u8, 4) color = {0};
  
  for_range(_y, max(y, 0), min(y + h, state->h)) {
    u8 t = (255 * (_y - y)) / h;
    for_range(i, 0, 4) {
      color.data[i] = (((u16)start.data[i] * (u16)(255 - t)) >> 8) +
                      (((u16)end.  data[i] * (u16)(      t)) >> 8);
    }
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      alpha_blend_rgba8(&pixels[_x + _y * state->w], transmute(u32, color));
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_rect_gradient_h(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, u32 _start, u32 _end) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  u32 *pixels = (u32 *)state->shm_pool_data;

  Array(u8, 4) start = transmute(type_of(start), _start);
  Array(u8, 4) end   = transmute(type_of(end),   _end  );
  Array(u8, 4) color = {0};

  // TODO(Franz): Update this value based on profiling
  if (state->h < 64) {
    for_range(_x, max(x, 0), min(x + w, state->w)) {
      u8 t = (255 * (_x - x)) / w;
      for_range(i, 0, 4) {
        color.data[i] = (((u16)start.data[i] * (u16)(255 - t)) >> 8) +
                        (((u16)end.  data[i] * (u16)(      t)) >> 8);
      }
      for_range(_y, max(y, 0), min(y + h, state->h)) {
        alpha_blend_rgba8(&pixels[_x + _y * state->w], transmute(u32, color));
      }
    }
  } else {
    for_range(_y, max(y, 0), min(y + h, state->h)) {
      for_range(_x, max(x, 0), min(x + w, state->w)) {
        u8 t = (255 * (_x - x)) / w;
        for_range(i, 0, 4) {
          color.data[i] = (((u16)start.data[i] * (u16)(255 - t)) >> 8) +
                          (((u16)end.  data[i] * (u16)(      t)) >> 8);
        }
        alpha_blend_rgba8(&pixels[_x + _y * state->w], transmute(u32, color));
      }
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal isize wayland_draw_text(Wayland_State *state, BMF_Font const *font, String str, u32 color, isize *x, isize *y) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  isize start_x = *x;

  u32 *pixels = (u32 *)state->shm_pool_data;
  BMF_Baked_Quad_I q;
  u16 color_alpha = color >> 24;

  slice_iter(str, c, _i, {
    if (*c == '\n') {
      *x = start_x;
    }
    b8 ok = bmf_get_baked_quad_i(font, *c, x, y, &q);
    if (!ok) {
      bmf_get_baked_quad_i(font, '.', x, y, &q);
    }

    if (q.x0 >= state->w) {
      break;
    }
    if (q.y0 >= state->h) {
      break;
    }

    for_range(ry, 0, q.y1 - q.y0) {
      isize _y = ry + q.y0;
      isize _v = ry + q.v0;

      if (_y >= state->h) {
        break;
      }

      for_range(rx, 0, q.x1 - q.x0) {
        isize _x = rx + q.x0;
        isize _u = rx + q.u0;

        if (_x >= state->w) {
          break;
        }

        u16 alpha = font->atlas.data[_u + _v * font->atlas_width];
        alpha_blend_rgb8(&pixels[_x + _y * state->w], color, (alpha * color_alpha) >> 8);
      }
    }
  })
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_box_shadow(Wayland_State *state, Rectangle rect) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  #define UI_SHADOW_RADIUS       16
  #define UI_SHADOW_INV_STRENGTH 2
  #define UI_SHADOW_COLOR        0

  // input: squared distance to completely shadowed region
  #define UI_SHADOW_FUNC(x2) (x2 / UI_SHADOW_INV_STRENGTH)

  u32 *pixels = (u32 *)state->shm_pool_data;
  for_range(_y, 0, UI_SHADOW_RADIUS) {
    if (_y + rect.y1 >= state->h) {
      break;
    }
    u8 t = (255 * (UI_SHADOW_RADIUS - _y)) / UI_SHADOW_RADIUS;
    t = ((u16)t * (u16)t) >> 8;
    for_range(_x, rect.x0 + UI_SHADOW_RADIUS, rect.x1) {
      if (_x >= state->w) {
        break;
      }
      alpha_blend_rgb8(&pixels[_x + state->w * (_y + rect.y1)], UI_SHADOW_COLOR, UI_SHADOW_FUNC(t));
    }
  }
  for_range(_x, 0, UI_SHADOW_RADIUS) {
    if (rect.x1 + _x >= state->w) {
      break;
    }
    u8 t = (255 * (UI_SHADOW_RADIUS - _x)) / UI_SHADOW_RADIUS;
    t = ((u16)t * (u16)t) >> 8;
    for_range(_y, rect.y0 + UI_SHADOW_RADIUS, rect.y1) {
      if (_y >= state->h) {
        break;
      }
      alpha_blend_rgb8(&pixels[rect.x1 + _x + state->w * _y], UI_SHADOW_COLOR, UI_SHADOW_FUNC(t));
    }
  }
  for_range(_y, 0, UI_SHADOW_RADIUS) {
    if (_y + rect.y1 >= state->h) {
      break;
    }
    for_range(_x, 0, UI_SHADOW_RADIUS) {
      if (rect.x1 + _x >= state->w) {
        break;
      }
      u8 tx = (255 * (UI_SHADOW_RADIUS - _x)) / UI_SHADOW_RADIUS;
      u8 ty = (255 * (UI_SHADOW_RADIUS - _y)) / UI_SHADOW_RADIUS;

      tx = ((u16)tx * (u16)tx) >> 8;
      ty = ((u16)ty * (u16)ty) >> 8;

      u8 t = ((u16)tx * (u16)ty) >> 8;

      alpha_blend_rgb8(&pixels[rect.x1 + _x + state->w * (_y + rect.y1)], UI_SHADOW_COLOR, UI_SHADOW_FUNC(t));
    }
  }
  for_range(_y, 0, UI_SHADOW_RADIUS) {
    if (_y + rect.y1 >= state->h) {
      break;
    }
    for_range(_x, 0, UI_SHADOW_RADIUS) {
      if (rect.x0 + _x >= state->w) {
        break;
      }
      u8 tx = (255 * _x) / UI_SHADOW_RADIUS;
      u8 ty = (255 * (UI_SHADOW_RADIUS - _y)) / UI_SHADOW_RADIUS;

      tx = ((u16)tx * (u16)tx) >> 8;
      ty = ((u16)ty * (u16)ty) >> 8;

      u8 t = ((u16)tx * (u16)ty) >> 8;

      alpha_blend_rgb8(&pixels[rect.x0 + _x + state->w * (_y + rect.y1)], UI_SHADOW_COLOR, UI_SHADOW_FUNC(t));
    }
  }
  for_range(_y, 0, UI_SHADOW_RADIUS) {
    if (_y + rect.y0 >= state->h) {
      break;
    }
    for_range(_x, 0, UI_SHADOW_RADIUS) {
      if (rect.x1 + _x >= state->w) {
        break;
      }
      u8 tx = (255 * (UI_SHADOW_RADIUS - _x)) / UI_SHADOW_RADIUS;
      u8 ty = (255 * _y) / UI_SHADOW_RADIUS;

      tx = ((u16)tx * (u16)tx) >> 8;
      ty = ((u16)ty * (u16)ty) >> 8;

      u8 t = ((u16)tx * (u16)ty) >> 8;

      alpha_blend_rgb8(&pixels[rect.x1 + _x + state->w * (_y + rect.y0)], UI_SHADOW_COLOR, UI_SHADOW_FUNC(t));
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_image(Wayland_State *state, i32 x, i32 y, Image const *image) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  assert(image->pixel_type == PT_u8);
  assert(image->components == 4);
  u32 *pixels = (u32 *)state->shm_pool_data;
  for_range(_y, 0, image->height) {
    if (_y + y >= state->h) {
      break;
    }
    for_range(_x, 0, image->width) {
      if (_x + x >= state->w) {
        break;
      }
      u32 pixel = ((u32 *)image->pixels.data)[_x + _y * image->width];
      alpha_blend_rgba8(&pixels[_x + x + (_y + y) * state->w], pixel);
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void wayland_draw_image_scaled(Wayland_State *state, i32 x, i32 y, i32 w, i32 h, Image const *image) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  assert(image->pixel_type == PT_u8);
  assert(image->components == 4);
  u32 *pixels = (u32 *)state->shm_pool_data;
  for_range(_y, 0, h) {
    isize iy = (_y * image->height) / h;
    if (_y + y >= state->h) {
      break;
    }
    for_range(_x, 0, w) {
      isize ix = (_x * image->width) / w;
      if (_x + x >= state->w) {
        break;
      }
      u32 pixel = ((u32 *)image->pixels.data)[ix + iy * image->width];
      alpha_blend_rgba8(&pixels[_x + x + (_y + y) * state->w], pixel);
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void ui_state_render(UI_Context *ctx, Wayland_State *wl_state) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  // slice_iter(ctx->commands, cmd, _i, {
  //   u32 hash = hash_ui_command(0, *cmd);
  //   // ui_state->command_hashes
  // });
  Rectangle rect;
  isize x, y;
  slice_iter(ctx->commands, cmd, _i, {
    switch(cmd->type) {
    case UI_Command_Type_None:
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("UI_Command_Type_None") ,get_time_in_micros());
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
      break;
    case UI_Command_Type_Gradient:
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("UI_Command_Type_Gradient"), get_time_in_micros());
      rect = cmd->variant.box.rect;
      wayland_draw_rect_gradient_v(
        wl_state,
        rect.x0 + 1,
        rect.y0 + 1,
        rect.x1 - rect.x0 - 2,
        rect.y1 - rect.y0 - 2,
        cmd->variant.box.color,
        cmd->variant.box.color2
      );
      wayland_draw_rect_outlines_alpha(
        wl_state,
        rect.x0,
        rect.y0,
        rect.x1 - rect.x0,
        rect.y1 - rect.y0,
        cmd->variant.box.outline
      );
      wayland_draw_box_shadow(wl_state, rect);
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
      break;
    case UI_Command_Type_Box:
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("UI_Command_Type_Box"), get_time_in_micros());
      rect = cmd->variant.box.rect;
      wayland_draw_rect_alpha(
        wl_state,
        rect.x0 + 1,
        rect.y0 + 1,
        rect.x1 - rect.x0 - 2,
        rect.y1 - rect.y0 - 2,
        cmd->variant.box.color
      );
      wayland_draw_rect_outlines_alpha(
        wl_state,
        rect.x0,
        rect.y0,
        rect.x1 - rect.x0,
        rect.y1 - rect.y0,
        cmd->variant.box.outline
      );
      wayland_draw_box_shadow(wl_state, rect);
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
      break;
    case UI_Command_Type_Text:
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("UI_Command_Type_Text"), get_time_in_micros());
      x = cmd->variant.text.bounds.x0 + ctx->font.decender;
      y = cmd->variant.text.bounds.y1 - ctx->font.decender;
      wayland_draw_text(wl_state, &ctx->font, cmd->variant.text.text, cmd->variant.text.color, &x, &y);
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
      break;
    case UI_Command_Type_Image:
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("UI_Command_Type_Image"), get_time_in_micros());
      rect = cmd->variant.image.rect;
      wayland_draw_image(
        wl_state,
        rect.x0,
        rect.y0,
        &ctx->images.data[cmd->variant.image.image.index]
      );
      wayland_draw_rect_outlines_alpha(
        wl_state,
        rect.x0,
        rect.y0,
        rect.x1 - rect.x0,
        rect.y1 - rect.y0,
        cmd->variant.image.outline
      );
      wayland_draw_box_shadow(wl_state, rect);
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
      break;
    }
  });

  vector_clear(&ctx->commands);
  ctx->x = 25;
  ctx->y = 25;
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

UI_Context ui_context;

String fps_string = {0};

internal void wayland_render(Wayland_Connection *conn, Wayland_State *state, Directory const *directory) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  u32 *pixels = (u32 *)state->shm_pool_data;

  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("Clear Background"), get_time_in_micros());
  u32 color = 0xFF1E2128;
  for_range(i, 0, state->w * state->h) {
    pixels[i] = color;
  }
  
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

  String str = LIT("The quick brown fox jumps over the lazy dog");
  ui_context.mouse.x = state->mouse_x;
  ui_context.mouse.y = state->mouse_y;

  ui_context.width  = state->w;
  ui_context.height = state->h;

  ui_context.mouse.buttons[0] = state->mouse_buttons[0];
  ui_context.mouse.buttons[1] = state->mouse_buttons[1];

  ui_context.horizontal = true;

  ui_label(&ui_context, fps_string);

  local_persist struct Time last_time = {0};
  struct Time curr_time = time_now();
  isize diff = curr_time.nsec - last_time.nsec;

  ui_label(&ui_context, fmt_tprintf(LIT("%fms"), (f32)((f64)diff / Millisecond)));
  last_time = curr_time;

  ui_label(&ui_context, str);

  ui_context.horizontal = false;
  ui_context.x  = 25;
  ui_context.y += ui_context.spacing + ui_context.font.single_h + ui_context.font.decender;

  // ui_image(&ui_context, (UI_Image) {.index = 0});

  // slice_iter(*directory, file, _i, {
  //   if (ui_button(&ui_context, file->name)) {
  //     // Process_Creation_Args pargs = DEFAULT_PROCESS_ARGS;
  //     // pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/xdg-open"), file->name});
  //     // unwrap_err(process_create(LIT("/bin/xdg-open"), &pargs));
  //   }
  // })

  for_range(c, 1, Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out + 1) {
    if (ui_button(&ui_context, wayland_wp_cursor_shape_device_v1_shape_string((Wayland_Wp_Cursor_Shape_Device_V1_Shape)c))) {
      wayland_wp_cursor_shape_device_v1_set_shape(conn, state->wp_cursor_shape_device, 0, (Wayland_Wp_Cursor_Shape_Device_V1_Shape)c);
    }
  }

  ui_state_render(&ui_context, state);
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}
