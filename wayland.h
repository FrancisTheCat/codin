#include "codin.h"

#define WAYLAND_DISPLAY_OBJECT_ID                    1
#define WAYLAND_WL_REGISTRY_EVENT_GLOBAL             0
#define WAYLAND_SHM_POOL_EVENT_FORMAT                0
#define WAYLAND_WL_BUFFER_EVENT_RELEASE              0
#define WAYLAND_XDG_WM_BASE_EVENT_PING               0
#define WAYLAND_XDG_TOPLEVEL_EVENT_CONFIGURE         0
#define WAYLAND_XDG_TOPLEVEL_EVENT_CLOSE             1
#define WAYLAND_XDG_SURFACE_EVENT_CONFIGURE          0
#define WAYLAND_WL_DISPLAY_GET_REGISTRY_OPCODE       1
#define WAYLAND_WL_REGISTRY_BIND_OPCODE              0
#define WAYLAND_WL_COMPOSITOR_CREATE_SURFACE_OPCODE  0
#define WAYLAND_XDG_WM_BASE_PONG_OPCODE              3
#define WAYLAND_XDG_SURFACE_ACK_CONFIGURE_OPCODE     4
#define WAYLAND_WL_SHM_CREATE_POOL_OPCODE            0
#define WAYLAND_XDG_WM_BASE_GET_XDG_SURFACE_OPCODE   2
#define WAYLAND_WL_SHM_POOL_CREATE_BUFFER_OPCODE     0
#define WAYLAND_WL_SURFACE_ATTACH_OPCODE             1
#define WAYLAND_XDG_SURFACE_GET_TOPLEVEL_OPCODE      1
#define WAYLAND_WL_SURFACE_COMMIT_OPCODE             6
#define WAYLAND_WL_DISPLAY_ERROR_EVENT               0
#define WAYLAND_FORMAT_XRGB8888                      1
#define WAYLAND_HEADER_SIZE                          8
#define COLOR_CHANNELS                               4

u32 __wayland_current_id = 1;

#define roundup_4(n) (((n) + 3) & -4)

struct sockaddr_un {
  u16 family;
  char path[108];
};

internal Socket wayland_display_connect() {
  String xdg_runtime_dir = get_env(LIT("XDG_RUNTIME_DIR"));
  String wayland_display = get_env(LIT("WAYLAND_DISPLAY"));

  String socket_path = fmt_tprintf(LIT("%S/%S"), xdg_runtime_dir, wayland_display);
  struct sockaddr_un addr = {0};
  addr.family = 1;
  string_copy((String) {.data = addr.path, .len = size_of(addr.path) - 1}, socket_path);

  Socket s = syscall(SYS_socket, 1, SOCK_STREAM, 0);
  assert(s != -1);

  isize connect_status = syscall(SYS_connect, s, &addr, size_of(addr));
  assert(connect_status != -1)

  return s;
}

internal isize wayland_wl_display_get_registry(Socket s) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  u32 object_id = WAYLAND_DISPLAY_OBJECT_ID;
  write_any(&writer, &object_id);

  u16 opcode = WAYLAND_WL_DISPLAY_GET_REGISTRY_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + sizeof(__wayland_current_id);
  write_any(&writer, &msg_announced_size);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  (void)unwrap_err(socket_write(s, builder_to_bytes(builder)));

  log_infof(LIT("-> wl_display@%d.get_registry: wl_registry=%d"), WAYLAND_DISPLAY_OBJECT_ID, __wayland_current_id);

  return __wayland_current_id;
}

internal isize wayland_wl_registry_bind(Socket s, u32 registry, u32 name, String interface, u32 version) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &registry);

  u16 opcode = WAYLAND_WL_REGISTRY_BIND_OPCODE;
  write_any(&writer, &opcode);

  i32 interface_len = roundup_4(interface.len);
  interface.len = interface_len;

  u16 msg_announced_size =
    + WAYLAND_HEADER_SIZE
    + size_of(name)
    + size_of(interface_len)
    + interface_len
    + size_of(version)
    + size_of(__wayland_current_id);
  write_any(&writer, &msg_announced_size);

  write_any(&writer, &name);
  write_any(&writer, &interface_len);
  write_string(&writer, interface);
  write_any(&writer, &version);
  
  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  (void)unwrap_err(socket_write(s, builder_to_bytes(builder)));

  log_infof(LIT("-> wl_registry@%d.bind: name=%d interface=%s version=%d wayland_current_id=%d"), registry, name, interface.data, version, __wayland_current_id);

  return __wayland_current_id;
}

typedef enum {
  STATE_NONE,
  STATE_SURFACE_ACKED_CONFIGURE,
  STATE_SURFACE_ATTACHED,
} Framebuffer_State_t;

typedef struct {
  u32 wl_registry;
  u32 wl_shm;
  u32 wl_shm_pool;
  u32 wl_buffer;
  u32 xdg_wm_base;
  u32 xdg_surface;
  u32 wl_compositor;
  u32 wl_surface;
  u32 xdg_toplevel;
  u32 stride;
  u32 w;
  u32 h;
  u32 shm_pool_size;
  i32 shm_fd;
  u8 *shm_pool_data;
  Framebuffer_State_t state;
} Wayland_State;

internal b8 create_shared_memory_file(uintptr size, Wayland_State *state) {
  #define MFD_CLOEXEC		0x0001U
  #define MFD_ALLOW_SEALING	0x0002U
  #define MFD_HUGETLB		0x0004U
  /* not executable and sealed to prevent changing to executable. */
  #define MFD_NOEXEC_SEAL		0x0008U
  /* executable */
  #define MFD_EXEC		0x0010U
  Fd fd = syscall(SYS_memfd_create, "my_wayland_shared_memory_file_123123", MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd == -1) {
    return false;
  }
  
  isize t = syscall(SYS_ftruncate, fd, size);
  if (t != 0) {
    return false;
  }
  
  state->shm_pool_data =
    (u8 *)syscall(SYS_mmap, nil, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (state->shm_pool_data == (rawptr)-1) {
    return false;
  }
  state->shm_fd = fd;

  // char name[255] = "my_wayland_client0911";

  // int fd = syscall(SYS_open, name, O_RDWR | O_EXCL | O_CREAT, 0600);
  // if (fd == -1) {
  //   return false;
  // }

  // if (syscall(SYS_unlink, name) == -1) {
  //   return false;
  // }

  // if (syscall(SYS_ftruncate, fd, size) == -1) {
  //   return false;
  // }

  // state->shm_pool_data = (u8 *)syscall(SYS_mmap, nil, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  // state->shm_pool_data[0] = 1;
  // assert(state->shm_pool_data != nil);
  // state->shm_fd = fd;

  return true;
}

internal void wayland_xdg_wm_base_pong(Socket socket, Wayland_State *state, u32 ping) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->xdg_wm_base);

  u16 opcode = WAYLAND_XDG_WM_BASE_PONG_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(ping);
  write_any(&writer, &msg_announced_size);

  write_any(&writer, &ping);

  log_infof(LIT("-> xdg_wm_base@%d.pong:"), state->xdg_wm_base);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));
}

internal void wayland_xdg_surface_ack_configure(Socket socket, Wayland_State *state, u32 configure) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->xdg_wm_base);

  u16 opcode = WAYLAND_XDG_WM_BASE_PONG_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(configure);
  write_any(&writer, &msg_announced_size);

  write_any(&writer, &configure);

  log_infof(LIT("-> xdg_surface@%d.ack_configure:"), state->xdg_surface);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));
}

internal isize handle_wayland_message(Socket socket, Wayland_State *state, Byte_Slice data) {
  if (data.len < 8) {
    fmt_panicf(LIT("Invalid len: %d"), data.len);
  }

  isize data_len = data.len;

  Reader reader = buffer_reader(&data);

  u32 object_id;
  read_any(&reader, &object_id);

  u16 opcode;
  read_any(&reader, &opcode);

  u16 announced_size;
  read_any(&reader, &announced_size);

  // log_infof(LIT("%d %d %d"), (isize)object_id, (isize)opcode, (isize)announced_size);

  if (object_id == state->wl_registry && opcode == WAYLAND_WL_REGISTRY_EVENT_GLOBAL) {
    u32 name;
    read_any(&reader, &name);

    u32 interface_len;
    read_any(&reader, &interface_len);
    
    String interface;
    slice_init(&interface, roundup_4(interface_len), context.temp_allocator);

    read_bytes(&reader, string_to_bytes(interface));
    interface.len = interface_len - 1;

    u32 version;
    read_any(&reader, &version);

    // log_infof(LIT("name: %d, interface: '%S', version: %d"), name, interface, version);

    if (string_equal(interface, LIT("wl_shm"))) {
      state->wl_shm = wayland_wl_registry_bind(socket, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("xdg_wm_base"))) {
      state->xdg_wm_base = wayland_wl_registry_bind(socket, state->wl_registry, name, interface, version);
    }

    if (string_equal(interface, LIT("wl_compositor"))) {
      state->wl_compositor = wayland_wl_registry_bind(socket, state->wl_registry, name, interface, version);
    }
  } else if (object_id == state->xdg_wm_base && opcode == WAYLAND_XDG_WM_BASE_EVENT_PING) {
    u32 ping;
    read_any(&reader, &ping);
    wayland_xdg_wm_base_pong(socket, state, ping);
  } else if (object_id == state->xdg_surface && opcode == WAYLAND_XDG_SURFACE_EVENT_CONFIGURE) {
    u32 configure;
    read_any(&reader, &configure);
    wayland_xdg_surface_ack_configure(socket, state, configure);
    state->state = STATE_SURFACE_ACKED_CONFIGURE;
  } else if (object_id == WAYLAND_DISPLAY_OBJECT_ID && opcode == WAYLAND_WL_DISPLAY_ERROR_EVENT) {
    u32 target_object_id;
    read_any(&reader, &target_object_id);

    u32 code;
    read_any(&reader, &code);

    u32 error_len;
    read_any(&reader, &error_len);

    Byte_Slice err;
    slice_init(&err, roundup_4(error_len), context.temp_allocator);

    read_bytes(&reader, err);

    err.len = error_len;
    log_fatalf(LIT("Fatal error: object = %d, code = %d, error = '%S'"), target_object_id, code, err);
    trap();
  }

  return announced_size;
}

internal i32 wayland_wl_compositor_create_surface(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->wl_compositor);

  u16 opcode = WAYLAND_WL_COMPOSITOR_CREATE_SURFACE_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(__wayland_current_id);
  write_any(&writer, &msg_announced_size);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));

  log_infof(LIT("-> wl_compositor@%d.create_surface: wl_surface=%d"), state->wl_compositor, __wayland_current_id);

  return __wayland_current_id;
}

internal i32 wayland_xdg_wm_base_get_xdg_surface(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->xdg_wm_base);

  u16 opcode = WAYLAND_XDG_WM_BASE_GET_XDG_SURFACE_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(__wayland_current_id) + size_of(state->wl_surface);
  write_any(&writer, &msg_announced_size);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  write_any(&writer, &state->wl_surface);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));

  log_infof(LIT("-> xdg_wm_base@%d.get_xdg_surface: xdg_surface=%d"), state->xdg_wm_base, __wayland_current_id);

  return __wayland_current_id;
}

internal i32 wayland_xdg_surface_get_toplevel(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->xdg_surface);

  u16 opcode = WAYLAND_XDG_SURFACE_GET_TOPLEVEL_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(__wayland_current_id);
  write_any(&writer, &msg_announced_size);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));

  log_infof(LIT("-> xdg_surface@%d.get_toplevel: xdg_toplevel=%d"), state->xdg_surface, __wayland_current_id);

  return __wayland_current_id;
}

internal void wayland_wl_surface_commit(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->wl_surface);

  u16 opcode = WAYLAND_WL_SURFACE_COMMIT_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE;
  write_any(&writer, &msg_announced_size);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));

  log_infof(LIT("-> wl_surface@%d.commit:"), state->wl_surface);
}

internal void wayland_wl_surface_attach(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->wl_surface);

  u16 opcode = WAYLAND_WL_SURFACE_COMMIT_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size = WAYLAND_HEADER_SIZE + size_of(state->wl_buffer) + size_of(u32) * 2;
  write_any(&writer, &msg_announced_size);

  write_any(&writer, &state->wl_buffer);
  
  u32 zero;
  write_any(&writer, &zero);
  write_any(&writer, &zero);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));
}

internal i32 wayland_wl_shm_create_pool(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->wl_shm);

  u16 opcode = WAYLAND_WL_SHM_CREATE_POOL_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size =
    WAYLAND_HEADER_SIZE + size_of(__wayland_current_id) + size_of(state->shm_pool_size);
  write_any(&writer, &msg_announced_size);

  assert(builder.len == WAYLAND_HEADER_SIZE);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  File_Info info;
  file_stat(state->shm_fd, &info);
  assert(info.size == state->shm_pool_size);

  write_any(&writer, &state->shm_pool_size);

  assert(builder.len == msg_announced_size);

  // I dont even really know what to say about this

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
    // u8    data[0];
  };

  char control_buf[CMSG_SPACE(size_of(state->shm_fd))] __attribute__ ((aligned(8))) = "";

  struct iovec {
    rawptr base;
    usize  len;
  } iovec = {
    .base = builder.data,
    .len  = (usize)builder.len,
  };

  struct msghdr {
    rawptr        name;       /* Optional address */
    u32           namelen;    /* Size of address */
    struct iovec *iov;        /* Scatter/gather array */
    usize         iovlen;     /* # elements in msg_iov */
    void         *control;    /* Ancillary data, see below */
    usize         controllen; /* Ancillary data buffer len */
    i32           flags;      /* Flags (unused) */
  } socket_msg = {
    .iov        = &iovec,
    .iovlen     = 1,
    .control    = control_buf,
    .controllen = size_of(control_buf),
  };

  #define SCM_RIGHTS 1

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg);
  cmsg->level = SOL_SOCKET;
  cmsg->type  = SCM_RIGHTS;
  cmsg->len   = CMSG_LEN(size_of(state->shm_fd));

  mem_copy(CMSG_DATA(cmsg), &state->shm_fd, size_of(state->shm_fd));
  socket_msg.controllen = CMSG_SPACE(size_of(state->shm_fd));

  isize status = syscall(SYS_sendmsg, socket, &socket_msg, MSG_NOSIGNAL | 0x40);
  assert(status != -1);

  log_infof(LIT("-> wl_shm@%d.create_pool: wl_shm_pool=%d"), state->wl_shm, __wayland_current_id);

  return __wayland_current_id;
}

internal i32 wayland_wl_shm_pool_create_buffer(Socket socket, Wayland_State *state) {
  Builder builder;
  builder_init(&builder, 0, 64, context.temp_allocator);
  Writer writer = writer_from_builder(&builder);

  write_any(&writer, &state->wl_shm);

  u16 opcode = WAYLAND_WL_SHM_CREATE_POOL_OPCODE;
  write_any(&writer, &opcode);

  u16 msg_announced_size =
    + WAYLAND_HEADER_SIZE
    + size_of(__wayland_current_id)
    + size_of(i32)
    + size_of(state->w)
    + size_of(state->h)
    + size_of(state->stride)
    + size_of(u32);
  write_any(&writer, &msg_announced_size);

  __wayland_current_id += 1;
  write_any(&writer, &__wayland_current_id);
  
  i32 offset = 0;
  write_any(&writer, &offset);
  write_any(&writer, &state->w);
  write_any(&writer, &state->h);
  write_any(&writer, &state->stride);
  i32 format = WAYLAND_FORMAT_XRGB8888;
  write_any(&writer, &format);

  (void)unwrap_err(socket_write(socket, builder_to_bytes(builder)));

  log_infof(LIT("-> wl_shm_pool@%d.create_buffer: wl_buffer=%d"), state->wl_shm_pool, __wayland_current_id);

  return __wayland_current_id;
}
