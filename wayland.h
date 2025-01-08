#include "codin.h"
#include "ttf.h"
#include "bad_font.h"
#include "wayland_gen_common.h"
#include "xkb.h"

#include "wayland-gen/wayland.h"
#include "wayland-gen/xdg-shell.h"
#include "wayland-gen/cursor-shape.h"

#define WAYLAND_HEADER_SIZE 8
#define COLOR_CHANNELS      4

#include "ui.h"

#include "spall.h"

#define UI_FONT_SIZE 128

String last_key_string = {0};

SpallBuffer  spall_buffer;
SpallProfile spall_ctx;

// #define spall_buffer_begin(...)
// #define spall_buffer_end(...)

internal f64 get_time_in_micros() {
  return (f64)(time_now().nsec) / Microsecond;
}

internal b8 wayland_peek_event_info(Wayland_Connection *conn, u32 *object, u16 *event) {
  if (conn->end - conn->start < WAYLAND_HEADER_SIZE) {
    return false;
  }
  u16 size;
  mem_copy(&size,  &conn->buffer.data[conn->start + size_of(u32) + size_of(u16)], size_of(size));
  if (size > conn->end - conn->start) {
    return false;
  }
  mem_copy(object, &conn->buffer.data[conn->start + 0], size_of(*object));
  mem_copy(event,  &conn->buffer.data[conn->start + size_of(u32)], size_of(*event));
  if (*object == 1) {
    switch (*event) {
    case Wayland_Wl_Display_Event_Error:
      u32 id, code;
      String message;
      wayland_parse_event_wl_display_error(conn, &id, &code, &message, context.temp_allocator);
      log_fatalf(LIT("Fatal error: object = %d, code = %d, error = '%S'"), id, code, message);
      trap();
      break;

    case Wayland_Wl_Display_Event_Delete_Id:
      wayland_parse_event_wl_display_delete_id(conn, &id);
      vector_append(&conn->free_ids, id);
      break;
    }

    return wayland_peek_event_info(conn, object, event);
  }
  return true;
}

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

internal isize wayland_recieve_messages(Wayland_Connection *conn) {
  local_persist byte _control_buf[256] = {0};
  struct iovec iov = {
    .base = conn->buffer.data,
    .len  = conn->buffer.len,
  };
  struct msghdr msg = {
    .iov        = &iov,
    .iovlen     = 1,
    .control    = _control_buf,
    .controllen = size_of(_control_buf),
  };
  conn->end   = syscall(SYS_recvmsg, conn->socket, &msg, 0);
  conn->start = 0;

  struct cmsghdr *chdr = CMSG_FIRSTHDR(&msg);
  while (chdr) {
    vector_append(&conn->fds_in, *(i32 *)CMSG_DATA(chdr));
    chdr = CMSG_NXTHDR(&msg, chdr);
  }

  return conn->end;
}

internal void wayland_skip_event(Wayland_Connection *conn) {
  if (conn->end - conn->start < WAYLAND_HEADER_SIZE) {
    trap();
    return;
  }
  u16 size;
  mem_copy(&size, &conn->buffer.data[conn->start + size_of(u32) + size_of(u16)], size_of(size));
  if (size > conn->end - conn->start) {
    trap();
    return;
  }
  conn->start += size;
}

internal void wayland_connection_flush(Wayland_Connection *conn) {
  if (conn->builder.len) {
    Byte_Slice control_buf;
    slice_init(&control_buf, CMSG_SPACE(conn->fds_out.len * size_of(int)), context.temp_allocator);
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
    cmsg->len   = CMSG_LEN(conn->fds_out.len * size_of(int));
    mem_copy(CMSG_DATA(cmsg), conn->fds_out.data, conn->fds_out.len * size_of(int));

    syscall(SYS_sendmsg, conn->socket, &msg, MSG_NOSIGNAL);

    vector_clear(&conn->builder);
    vector_clear(&conn->fds_out);
  }
}

internal void wayland_connection_destroy(Wayland_Connection const *conn) {
  vector_delete(conn->builder);
  vector_delete(conn->fds_in);
  vector_delete(conn->fds_out);
  vector_delete(conn->free_ids);
  socket_close(conn->socket);
}

internal b8 wayland_display_connect(Allocator allocator, Byte_Slice buffer, Wayland_Connection *conn) {
  struct sockaddr_un {
    u16 family;
    char path[108];
  };

  conn->current_id = 1;
  vector_init(&conn->builder,  0, 8, allocator);
  vector_init(&conn->fds_in,   0, 8, allocator);
  vector_init(&conn->fds_out,  0, 8, allocator);
  vector_init(&conn->free_ids, 0, 8, allocator);
  conn->buffer = buffer;

  String xdg_runtime_dir = get_env(LIT("XDG_RUNTIME_DIR"));
  String wayland_display = get_env(LIT("WAYLAND_DISPLAY"));

  String socket_path = fmt_tprintf(LIT("%S/%S"), xdg_runtime_dir, wayland_display);
  struct sockaddr_un addr = {0};
  addr.family = 1;
  string_copy((String) {.data = addr.path, .len = size_of(addr.path) - 1}, socket_path);

  conn->socket = syscall(SYS_socket, 1, SOCK_STREAM, 0);
  if (conn->socket < 0) {
    return false;
  }

  isize connect_status = syscall(SYS_connect, conn->socket, &addr, size_of(addr));
  if (connect_status < 0) {
    return false;
  }

  return true;
}

u32 _wayland_connection_get_next_id(Wayland_Connection *conn) {
  if (conn->free_ids.len) {
    return vector_pop(&conn->free_ids);
  }
  conn->current_id += 1;
  return conn->current_id;
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
  u32           wl_data_device_manager;
  u32           wl_data_source;
  u32           wl_data_device;
  u32           wl_keyboard;
  u32           wl_pointer;
  u32           wl_shm;
  u32           wl_shm_pool;
  u32           wl_buffer;
  u32           wl_compositor;
  u32           wl_surface;
  u32           xdg_wm_base;
  u32           xdg_surface;
  u32           xdg_toplevel;
  u32           wp_cursor_shape_manager;
  u32           wp_cursor_shape_device;
  u32           stride;
  i32           w, h;
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
  Keymap        keymap;
  Void_Slice    keymap_data;
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
  Fd fd = syscall(SYS_memfd_create, "/wayland_framebuffer_memfd", MFD_ALLOW_SEALING | MFD_CLOEXEC);
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

internal void wayland_handle_events(Wayland_Connection *conn, Wayland_State *state) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());

  u32 object_id;
  u16 opcode;
  while (wayland_peek_event_info(conn, &object_id, &opcode)) {
    if (object_id == state->wl_registry && opcode == Wayland_Wl_Registry_Event_Global) {
      u32 name, version;
      String interface;

      wayland_parse_event_wl_registry_global(conn, &name, &interface, &version, context.temp_allocator);

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

      if (string_equal(interface, LIT("wl_data_device_manager"))) {
        assert(state->wl_seat);
        state->wl_data_device_manager = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
        state->wl_data_source = wayland_wl_data_device_manager_create_data_source(conn, state->wl_data_device_manager);
        wayland_wl_data_source_offer(conn, state->wl_data_source, LIT("text/plain"));
        state->wl_data_device = wayland_wl_data_device_manager_get_data_device(conn, state->wl_data_device_manager, state->wl_seat);
        wayland_wl_data_device_set_selection(conn, state->wl_data_device, state->wl_data_source, 0);
      }

      if (string_equal(interface, LIT("wp_cursor_shape_manager_v1"))) {
        state->wp_cursor_shape_manager = wayland_wl_registry_bind(conn, state->wl_registry, name, interface, version);
      }

    } else if (object_id == state->xdg_wm_base && opcode == Wayland_Xdg_Wm_Base_Event_Ping) {
      u32 serial;
      wayland_parse_event_xdg_wm_base_ping(conn, &serial);
      wayland_xdg_wm_base_pong(conn, state->xdg_wm_base, serial);

    } else if (object_id == state->xdg_surface && opcode == Wayland_Xdg_Surface_Event_Configure) {
      u32 serial;
      wayland_parse_event_xdg_surface_configure(conn, &serial);
      wayland_xdg_surface_ack_configure(conn, state->xdg_surface, serial);
      state->surface_state = Surface_State_Acked_Configure;

    } else if (object_id == state->xdg_toplevel) {
      switch ((Wayland_Xdg_Toplevel_Event)opcode) {
      case Wayland_Xdg_Toplevel_Event_Configure_Bounds:
        i32 width;
        i32 height;
        wayland_parse_event_xdg_toplevel_configure_bounds(conn, &width, &height);
        break;

      case Wayland_Xdg_Toplevel_Event_Configure:
        Byte_Slice states;
        wayland_parse_event_xdg_toplevel_configure(conn, &state->w, &state->h, &states, context.temp_allocator);

        state->w = max(state->w, 1);
        state->h = max(state->h, 1);

        state->should_resize = true;
        break;

      case Wayland_Xdg_Toplevel_Event_Close:
        wayland_parse_event_xdg_toplevel_close(conn);
        state->should_close = true;
        break;

      default:
        wayland_skip_event(conn);
        break;
      }

    } else if (object_id == state->wl_buffer && opcode == Wayland_Wl_Buffer_Event_Release) {
      wayland_parse_event_wl_buffer_release(conn);
      state->buffer_state = Buffer_State_Released;

    } else if (object_id == state->wl_keyboard) {
      switch ((Wayland_Wl_Keyboard_Event)opcode) {
      case Wayland_Wl_Keyboard_Event_Keymap:
        Wayland_Wl_Keyboard_Keymap_Format format;
        u32 size, n;
        Fd fd;
        b8 ok;

        wayland_parse_event_wl_keyboard_keymap(conn, &format, &fd, &size);
        assert(format == Wayland_Wl_Keyboard_Keymap_Format_Xkb_V1);

        if (state->keymap_data.data) {
          os_deallocate_pages(state->keymap_data.data, state->keymap_data.len);
          slice_delete(state->keymap, context.allocator);
        }

        n = (size + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE;
        state->keymap_data.data = (rawptr)syscall(SYS_mmap, nil, n * OS_PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
        state->keymap_data.len  = n;
        ok = parse_key_codes((String){.data = (char *)state->keymap_data.data, .len = size}, &state->keymap, context.allocator);
        assert(ok);
        break;

      case Wayland_Wl_Keyboard_Event_Key:
        u32 serial;
        u32 time;
        u32 key;
        Wayland_Wl_Keyboard_Key_State key_state;

        wayland_parse_event_wl_keyboard_key(conn, &serial, &time, &key, &key_state);

        if (key + 8 < state->keymap.len) {
          if (state->keymap.data[key + 8]) {
            state->keys[state->keymap.data[key + 8]] = key_state == Wayland_Wl_Keyboard_Key_State_Pressed;
          }

          if (state->keymap.data[key + 8] == Key_Escape && key_state == Wayland_Wl_Keyboard_Key_State_Pressed) {
            state->should_close = true;
          }

          last_key_string = enum_to_string(Key, state->keymap.data[key + 8]);
          fmt_println(last_key_string);
        }
        break;

      case Wayland_Wl_Keyboard_Event_Enter:
        u32 surface;
        Byte_Slice keys;

        wayland_parse_event_wl_keyboard_enter(conn, &serial, &surface, &keys, context.temp_allocator);
        break;
      
      default:
        wayland_skip_event(conn);
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
        wayland_parse_event_wl_pointer_motion(conn, &time, &x, &y);

        state->mouse_x = x;
        state->mouse_y = y;

        break;
      case Wayland_Wl_Pointer_Event_Button:
        wayland_parse_event_wl_pointer_button(conn, &serial, &time, &button, &button_state);

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
        wayland_parse_event_wl_pointer_axis(conn, &time, &axis, &value);
        break;

      case Wayland_Wl_Pointer_Event_Enter:
        wayland_parse_event_wl_pointer_enter(conn, &serial, &surface, &x, &y);

        if (state->wp_cursor_shape_manager) {
          if (!state->wp_cursor_shape_device) {
            state->wp_cursor_shape_device =wayland_wp_cursor_shape_manager_v1_get_pointer(conn, state->wp_cursor_shape_manager, state->wl_pointer);
          }

          wayland_wp_cursor_shape_device_v1_set_shape(conn, state->wp_cursor_shape_device, serial, Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default);        }
        break;

      default:
        wayland_skip_event(conn);
        break;
      }
    } else if (object_id == state->wl_seat) {
      switch ((Wayland_Wl_Seat_Event)opcode) {
      case Wayland_Wl_Seat_Event_Capabilities:
        Wayland_Wl_Seat_Capability capabilities;
        wayland_parse_event_wl_seat_capabilities(conn, &capabilities);
        break;
      case Wayland_Wl_Seat_Event_Name:
        String name;
        wayland_parse_event_wl_seat_name(conn, &name, context.temp_allocator);
        break;
      }

    } else if (object_id == state->wl_data_source) {
      switch ((Wayland_Wl_Data_Source_Event)opcode) {
      case Wayland_Wl_Data_Source_Event_Target:
        String mime_type;
        wayland_parse_event_wl_data_source_target(conn, &mime_type, context.temp_allocator);
        fmt_debug(mime_type, "S");
        break;
      case Wayland_Wl_Data_Source_Event_Send:
        Fd fd;
        wayland_parse_event_wl_data_source_send(conn, &mime_type, &fd, context.temp_allocator);
        if (string_equal(mime_type, LIT("text/plain"))) {
          file_write(fd, string_to_bytes(LIT("Hello World")));
        }
        file_close(fd);
        break;
      case Wayland_Wl_Data_Source_Event_Cancelled:
        wayland_parse_event_wl_data_source_cancelled(conn);
        break;
      case Wayland_Wl_Data_Source_Event_Dnd_Drop_Performed:
        wayland_parse_event_wl_data_source_dnd_drop_performed(conn);
        break;
      case Wayland_Wl_Data_Source_Event_Dnd_Finished:
        wayland_parse_event_wl_data_source_dnd_finished(conn);
        break;
      case Wayland_Wl_Data_Source_Event_Action:
        Wayland_Wl_Data_Device_Manager_Dnd_Action action;
        wayland_parse_event_wl_data_source_action(conn, &action);
        break;
      }
    } else {
      wayland_skip_event(conn);
    }
  }

  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
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
        u8 t = (u8)((f32)(255 * (_x - x)) / (f32)w); // NOTE(Franz): this vectorizes *way* better in my crude tests
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

internal isize wayland_draw_text_bmf(Wayland_State *state, BMF_Font const *font, String str, u32 color, isize *x, isize *y) {
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
      isize ix = (f32)(_x * image->width) / (f32)w;
      if (_x + x >= state->w) {
        break;
      }
      u32 pixel = ((u32 *)image->pixels.data)[ix + iy * image->width];
      alpha_blend_rgba8(&pixels[_x + x + (_y + y) * state->w], pixel);
    }
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal isize measure_text_ttf(String text);
internal void wayland_draw_text_ttf(
  Wayland_State  *state,
  TTF_Font const *font,
  String          str,
  isize           font_size,
  u32             color,
  isize          *x_pos,
  isize          *y_pos
);

TTF_Font ttf_font;

internal void ui_state_render(UI_Context *ctx, Wayland_State *wl_state) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  // slice_iter(ctx->commands, cmd, _i, {
  //   u32 hash = hash_ui_command(0, *cmd);
  //   // ui_state->command_hashes
  // });
  Rectangle rect;
  isize x, y;
  slice_iter(ctx->commands, cmd, _i, {
    spall_buffer_begin(&spall_ctx, &spall_buffer, enum_to_string(UI_Command_Type, cmd->type), get_time_in_micros());
    switch(cmd->type) {
    case UI_Command_Type_None:
      break;
    case UI_Command_Type_Gradient:
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
      break;
    case UI_Command_Type_Box:
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
      break;
    case UI_Command_Type_Text:
      x = cmd->variant.text.bounds.x0 + ctx->spacing;
      y = (float)(cmd->variant.text.bounds.y0 + cmd->variant.text.bounds.y1) / 2 + ttf_get_font_height(&ttf_font, UI_FONT_SIZE) / 2 - 1; 
      wayland_draw_text_ttf(wl_state, &ttf_font, cmd->variant.text.text, UI_FONT_SIZE, cmd->variant.text.color, &x, &y);

      // x = cmd->variant.text.bounds.x0 + ctx->font.decender;
      // y = cmd->variant.text.bounds.y1 - ctx->font.decender;
      // wayland_draw_text(wl_state, &ctx->font, cmd->variant.text.text, cmd->variant.text.color, &x, &y);
      break;
    case UI_Command_Type_Image:
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
      break;
    }
    spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
  });

  vector_clear(&ctx->commands);
  ctx->x = 25;
  ctx->y = 25;
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

UI_Context ui_context;

String fps_string = {0};

typedef struct {
  TTF_V_Metrics v_metrics;
  TTF_H_Metrics h_metrics;
  u32           w, h;
  u8           *pixels;
} Cached_Glyph;

Cached_Glyph glyph_cache[0x10FFFF] = {0};

internal isize measure_text_ttf(String text) {
  isize width = 0;
  string_iter(text, codepoint, _i, {
    width += glyph_cache[codepoint].h_metrics.advance;
  });
  return width;
}

internal void wayland_draw_text_ttf(
  Wayland_State  *state,
  TTF_Font const *font,
  String          str,
  isize           font_size,
  u32             color,
  isize          *x_pos,
  isize          *y_pos
) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());

  isize start_x = *x_pos;
  u32  *pixels  = (u32 *)state->shm_pool_data;

  string_iter(str, codepoint, i, {
    if (codepoint == '\n') {
      *x_pos  = start_x;
      *y_pos += ttf_get_line_height(font, font_size);
      continue;
    }
    Cached_Glyph *cached_glyph = &glyph_cache[codepoint];
    if (!cached_glyph->pixels) {
      spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("render_glyph"), get_time_in_micros());
      u32 glyph = ttf_get_codepoint_glyph(&ttf_font, codepoint);
      ttf_get_glyph_h_metrics(&ttf_font, glyph, font_size, &cached_glyph->h_metrics);
      ttf_get_glyph_v_metrics(&ttf_font, glyph, font_size, &cached_glyph->v_metrics);

      ttf_get_glyph_bitmap(&ttf_font, glyph, font_size, &cached_glyph->w, &cached_glyph->h, &cached_glyph->pixels);
      spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
    }

    isize x = *x_pos + cached_glyph->h_metrics.bearing * (i != 0);
    isize y = *y_pos - cached_glyph->v_metrics.height + cached_glyph->v_metrics.bearing;
    for_range(iy, 0, cached_glyph->h) {
      if (iy + y >= state->h) {
        break;
      }
      if (iy + y < 0) {
        continue;
      }
      for_range(ix, 0, cached_glyph->w) {
        if (ix + x >= state->w) {
          break;
        }
        if (ix + x < 0) {
          continue;
        }
        u8 alpha = cached_glyph->pixels[ix + iy * cached_glyph->w];
        alpha_blend_rgb8(&pixels[ix + x + (iy + y) * state->w], color, alpha);
      }
    }
    *x_pos += cached_glyph->h_metrics.advance;
  });
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

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

  ui_label(&ui_context, fmt_tprintf(LIT("%07.4fms"), (f32)((f64)diff / Millisecond)));
  last_time = curr_time;

  ui_label(&ui_context, str);

  ui_context.horizontal = false;
  ui_context.x  = 25;
  ui_context.y += ui_context.spacing + ui_context.text_height + ui_context.spacing * 2;

  // ui_image(&ui_context, (UI_Image) {.index = 0});

  (void)directory;
  // slice_iter(*directory, file, _i, {
  //   if (ui_button(&ui_context, file->name)) {
  //     // Process_Creation_Args pargs = DEFAULT_PROCESS_ARGS;
  //     // pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/xdg-open"), file->name});
  //     // unwrap_err(process_create(LIT("/bin/xdg-open"), &pargs));
  //   }
  // })
  // isize font_x = 100;
  // isize font_y = 200;

  // isize font_size = 30;

  // wayland_draw_rect_outlines(
  //   state,
  //   100,
  //   200 - ttf_get_font_height(&ttf_font, font_size) + 1,
  //   measure_text(fps_string),
  //   ttf_get_font_height(&ttf_font, font_size) + 2,
  //   0xFFE06B74
  // );
  // wayland_draw_text_ttf(state, &ttf_font, fps_string, font_size, 0xFF98C379, &font_x, &font_y);

  // font_x = 100;
  // font_y = 200 + ttf_get_line_height(&ttf_font, font_size);

  // str = LIT("An LLVM logo: \nAnd an 'oh my zsh' logo: \nAll of this is written in  (but without libc)\n( +  +  btw)\nabcdefghijlkmnopqrstuvwxyz\n");
  
  // wayland_draw_text_ttf(state, &ttf_font, str, font_size, 0xFF62AEEF, &font_x, &font_y);

  // wayland_draw_text_ttf(state, &ttf_font, last_key_string, font_size, 0xFFE06B74, &font_x, &font_y);

  for_range(_c, 1, Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out + 1) {
    Wayland_Wp_Cursor_Shape_Device_V1_Shape c = (Wayland_Wp_Cursor_Shape_Device_V1_Shape)_c;
    if (ui_button(&ui_context, enum_to_string(Wayland_Wp_Cursor_Shape_Device_V1_Shape, c))) {
      wayland_wp_cursor_shape_device_v1_set_shape(conn, state->wp_cursor_shape_device, 0, c);
    }
  }

  ui_state_render(&ui_context, state);
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}
