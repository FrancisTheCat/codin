#include "codin.h"
#include "image.h"
#include "iter.h"
#include "xml.h"
#include "wayland.h"

#include "math.h"

b8 test_sort() {
  Slice(isize) sorting_array;
  slice_init(&sorting_array, 1e4, context.allocator);

  slice_iter(sorting_array, elem, _i, { *elem = _i; });

  sort_slice(sorting_array);

  isize last = 0;
  slice_iter(sorting_array, elem, _i, { or_return_bool(last <= *elem); });

  slice_reverse(sorting_array);

  last = sorting_array.data[0];
  slice_iter(sorting_array, elem, _i, { or_return_bool(last >= *elem); });

  slice_delete(sorting_array, context.allocator);

  return true;
}

b8 test_vector() {
  Vector(isize) v = {0};

  for (int i = 0; i < 10; i++) {
    vector_append(&v, i);
  }
  test_expect(v.len == 10);
  test_expect(v.cap == 16);

  for (int i = 0; i < v.len; i++) {
    test_expect(i == v.data[i]);
  }
  test_expect(vector_remove_ordered(&v, 5) == 5);

  vector_iter(v, elem, i, {
    test_expect(*elem == v.data[i]);
  });

  test_expect(vector_remove(&v, 2) == 2);

  for (; v.len;) {
    test_expect(vector_pop(&v) || v.len == 0);
  }

  vector_delete(v);

  return true;
}

b8 test_pool_allocator() {
  Pool_Allocator _pool;
  Allocator pool =
      pool_allocator(&_pool, sizeof(isize), 1024, true, context.allocator);

  Vector(isize *) allocations;
  vector_init(&allocations, 0, 8, context.allocator);
  for (isize i = 0; i < 8; i += 1) {
    vector_append(&allocations, mem_tnew(isize, pool));
  }

  vector_iter(allocations, ptr, i,
              { test_expect(mem_tfree(*ptr, pool) == AE_None); });

  Allocator_Result r = mem_alloc(sizeof(isize) + 1, pool);
  test_expect(!r.value && (r.err == AE_Invalid_Arguments));

  r = mem_alloc(sizeof(isize), pool);
  test_expect(r.value && (r.err == AE_None));

  pool_allocator_destroy(_pool);
  vector_delete(allocations);

  return true;
}

b8 test_heap_allocator() {
  Vector(isize *) allocations;
  vector_init(&allocations, 0, 1000, context.allocator);
  for (isize i = 0; i < 1000; i += 1) {
    vector_append(&allocations, mem_tnew(isize, context.allocator));
  }

  typedef struct {
    isize value[2];
  } Double_Int;

  Vector(Double_Int *) allocations2;
  vector_init(&allocations2, 0, 1000, context.allocator);
  for (isize i = 0; i < 1000; i += 1) {
    vector_append(&allocations2, mem_tnew(Double_Int, context.allocator));
  }

  vector_iter(allocations, ptr, i,
              { test_expect(mem_tfree(*ptr, context.allocator) == AE_None); });

  vector_iter(allocations2, ptr, i,
              { test_expect(mem_tfree(*ptr, context.allocator) == AE_None); });

  Allocator_Result r;
  r = mem_alloc(sizeof(isize), context.allocator);
  test_expect(r.value && (r.err == AE_None));
  r.err = mem_free(r.value, size_of(isize), context.allocator);
  test_expect(r.err == AE_None);

  vector_delete(allocations);
  vector_delete(allocations2);

  return true;
}

b8 test_growing_arena_allocator() {
  Growing_Arena_Allocator arena;
  Allocator ga = growing_arena_allocator_init(&arena, 1024, context.allocator);

  Allocator_Result r;

  r = mem_alloc(512, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(512, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(512, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(1024, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(1, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(1025, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(1, ga);
  test_expect(r.value && (r.err == AE_None));

  r = mem_alloc(0, ga);
  test_expect(!r.value && (r.err == AE_None));

  r = mem_alloc(1024, ga);
  test_expect(r.value && (r.err == AE_None));

  growing_arena_allocator_destroy(arena);

  return true;
}

b8 test_hash_map() {
  Hash_Map(isize, isize) map;
  hash_map_init(&map, 4096 * 32, nil, nil, context.allocator);

#define N 1e5

  for_range(i, 0, N) {
    test_expect(map.len == i);
    hash_map_insert(&map, i, 512 - i);
  }
  test_expect(map.len == N);

  hash_map_iter(map, key, value, { assert(key == 512 - *value); });
  for_range(i, 0, N) { test_expect(*hash_map_get(map, i) == 512 - i); }

  hash_map_clear(&map);
  test_expect(map.len == 0);

  for_range(i, 0, N) { hash_map_insert(&map, i, i); }

  for_range(i, 0, N) {
    test_expect(*hash_map_get(map, i) == i);
    hash_map_remove(&map, i);
  }

  hash_map_iter(map, _k, _v, { test_fail(); });
  for_range(i, 0, N) { test_expect(hash_map_get(map, i) == nil); }

  hash_map_delete(map);

#undef N

  return true;
}

// static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//     VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//     VkDebugUtilsMessageTypeFlagsEXT messageType,
//     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//     void* pUserData) {

//     log_errorf(LIT("validation layer: '%s'"),pCallbackData->pMessage);

//     return VK_FALSE;
// }

// VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
//     VkInstance                                  instance,
//     const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
//     const VkAllocationCallbacks*                pAllocator,
//     VkDebugUtilsMessengerEXT*                   pMessenger) {
//   PFN_vkCreateDebugUtilsMessengerEXT func =
//     (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//   if (func) {
//     return func(instance, pCreateInfo, pAllocator, pMessenger);
//   } else {
//     return VK_ERROR_EXTENSION_NOT_PRESENT;
//   }
// }

// VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
//     VkInstance                                  instance,
//     VkDebugUtilsMessengerEXT                    pMessenger,
//     const VkAllocationCallbacks*                pAllocator) {
//   PFN_vkDestroyDebugUtilsMessengerEXT func =
//     (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//   if (func) {
//     func(instance, pMessenger, pAllocator);
//   }
// }

b8 spall_write_callback(SpallProfile *self, const void *data, isize length) {
  return file_write((Fd)self->data, (Byte_Slice) {.data = (byte *)data, .len = length}).err == OSE_None;
}

void spall_close_callback(SpallProfile *self) {
  file_close((Fd)self->data);
}

int main() {
  Tracking_Allocator track;
  context.allocator = tracking_allocator_init(&track, context.allocator);

  context.logger = create_file_logger(1);

  Fd spall_fd = unwrap_err(file_open(LIT("trace.spall"), FP_Create | FP_Read_Write | FP_Truncate));
  spall_ctx = spall_init_callbacks(1, spall_write_callback, nil, spall_close_callback, (rawptr)spall_fd);

	int buffer_size = 1 * 1024 * 1024;
	byte *buffer = mem_alloc(buffer_size, context.allocator).value;
	spall_buffer = (SpallBuffer){
		.length = buffer_size,
		.data = buffer,
	};

	spall_buffer_init(&spall_ctx, &spall_buffer);

  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("read_directory"), get_time_in_micros());
  Directory directory = unwrap_err(read_directory_path(LIT("."), context.allocator));
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

  isize max_name_len = 0;
  isize max_size_len = LIT("<dir>").len;

  slice_iter(directory, file, i, {
    max_name_len = max(file->name.len, max_name_len);
    max_size_len = max(fmt_file_size_w(nil, file->size), max_size_len);
  });

  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("sort"), get_time_in_micros());
  sort_slice_by(directory, i, j, string_compare_lexicographic(directory.data[i].name, directory.data[j].name));
      // ((dir.data[i].is_dir != dir.data[j].is_dir)
      //      ? dir.data[i].is_dir
      //      : string_compare_lexicographic(dir.data[i].name,
      //      dir.data[j].name)));
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

  String cwd = unwrap_err(_get_current_directory(context.temp_allocator));
  fmt_printfln(LIT("Directory: %S\n"), cwd);

  String spaces = slice_make(String, max_name_len, context.temp_allocator);
  slice_iter(spaces, s, _i, {
    *(char *)s = ' ';
  });

  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("print_directory"), get_time_in_micros());
  {
    Builder buffer;
    builder_init(&buffer, 0, 8, context.temp_allocator);
    Writer w = writer_from_builder(&buffer);
    slice_iter(directory, file, i, {
      isize n = fmt_wprintf(&w, LIT("%S"), file->name);
      fmt_wprint(&w, slice_range(spaces, 0, max_name_len - n));

      fmt_wprint(&w, LIT(" | "));

      if (file->is_dir) {
        n = fmt_wprint(&w, LIT("<dir>"));
      } else {
        n = fmt_wprintf(&w, LIT("%M"), file->size);
      }
      fmt_wprint(&w, slice_range(spaces, 0, max_size_len - n));

      fmt_wprintfln(&w, LIT(" | %T | %T | %T"), file->creation_time, file->modification_time, file->acces_time);
    });
    fmt_print(builder_to_string(buffer));
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

  fmt_println(LIT(""));

  fmt_printfln(LIT("Args: %[S]"), os_args);

  fmt_printfln(LIT("PI: %.8f"), 3.14159265359);
  fmt_printfln(LIT("E:  %.8f"), 2.71828182846);

  // directory_delete(dir, context.allocator);

  // Process_Creation_Args pargs = DEFAULT_PROCESS_ARGS;
  // pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/echo"), LIT("HELLO FROM ECHO")});
  // Pid pid = unwrap_err(process_create(LIT("/bin/echo"), &pargs));

  // process_wait(pid);

  // pargs = DEFAULT_PROCESS_ARGS;
  // pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/xdg-open"), LIT("test.ppm")});
  // pid = unwrap_err(process_create(LIT("/bin/xdg-open"), &pargs));

  // process_wait(pid);

  // pargs = DEFAULT_PROCESS_ARGS;
  // pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/alacritty")});
  // pid = unwrap_err(process_create(LIT("/bin/alacritty"), &pargs));

  // process_wait(pid);

  // return 0;

  fmt_println(LIT(""));

  Test_Context tc;
  test_context_init(&tc, context.allocator);

  test_add(&tc, test_growing_arena_allocator);
  test_add(&tc, test_pool_allocator);
  test_add(&tc, test_heap_allocator);
  test_add(&tc, test_hash_map);
  test_add(&tc, test_vector);
  test_add(&tc, test_sort);

  test_context_execute(&tc);
  test_context_destroy(tc);

  // Image image;
  // Byte_Slice png_data = unwrap_err(read_entire_file_path(LIT("wallpaper.png"), context.temp_allocator));
  // b8 ok = png_load_bytes(png_data, &image, context.allocator);
  // assert(ok);

  // Fd file;
  // Writer output_writer;

  // file = unwrap_err(file_open(LIT("output.ppm"), FP_Read_Write | FP_Create | FP_Truncate));
  // output_writer = writer_from_handle(file);
  // ok = ppm_save_writer(&output_writer, &image);
  // // assert(ok);
  // file_close(file);

  // file = unwrap_err(file_open(LIT("output.png"), FP_Read_Write | FP_Create | FP_Truncate));
  // output_writer = writer_from_handle(file);
  // ok = png_save_writer(&output_writer, &image);
  // assert(ok);
  // file_close(file);
  // slice_delete(image.pixels, context.allocator);

  // image = (Image){0};
  // Byte_Slice ppm_data = unwrap_err(read_entire_file_path(LIT("output.ppm"), context.temp_allocator));
  // ok = ppm_load_bytes(ppm_data, &image);
  // assert(ok);
  
  // Byte_Slice _data = unwrap_err(read_entire_file_path(LIT("main.c"), context.temp_allocator));
  // String data = transmute(String, _data);
  // Strings_Iterator iter = string_lines_iterator(data);

  // iterate(iter, line, i, {
  //   fmt_printfln(LIT("%03d:\t%S"), i, line);
  // });

  // Dynlib lib = unwrap(dynlib_load(LIT("./dynlib_test/dynlib_test.so"), context.allocator));
  // // Dynlib lib = unwrap(dynlib_load(LIT("/lib/libm.so.6"), context.allocator));
  // // Dynlib lib = unwrap(dynlib_load(LIT("/lib/libOpenGL.so"), context.allocator));
  // isize max_symbol_len = 0;
  // hash_map_iter(lib.symbols, str, addr, {
  //   max_symbol_len = max(max_symbol_len, str.len); 
  // });
  // String format_string = fmt_tprintf(LIT("%%-%dS: %%x"), max_symbol_len);
  // hash_map_iter(lib.symbols, str, addr, {
  //   fmt_printfln(format_string, str, *addr);
  // });

  // ((void(*)(void))dynlib_get_symbol(lib, LIT("__$startup_runtime")))();

  // String *s = (String *)((uintptr)dynlib_get_symbol(lib, LIT("hello")));
  // log_infof(LIT("hello: '%S'"), *s);

  // i32 four = *(i32 *)((uintptr)dynlib_get_symbol(lib, LIT("four")));
  // log_infof(LIT("four: %x"), (isize)four);

  // i32 (*add)(i32, i32) = dynlib_get_symbol(lib, LIT("add"));
  // assert(add);
  // i32 three = add(1, 2);
  // assert(three == 3);

  // String(*say_hello)(void) = dynlib_get_symbol(lib, LIT("say_hello"));
  // assert(say_hello);
  // String hello = say_hello();

  // log_infof(LIT("say_hello: '%S'"), hello);
  // log_infof(LIT("hello: '%S'"), *s);

  // dynlib_unload(lib);

  // context.logger.proc = nil;

  Wayland_Connection wl_connection = wayland_display_connect(context.allocator);
  Wayland_State state = {
      .wl_registry = wayland_wl_display_get_registry(&wl_connection, 1),
      .w      = 64,
      .h      = 64,
      .stride = 64 * COLOR_CHANNELS,
  };
  wayland_wl_display_sync(&wl_connection, 1);

  // Single buffering.
  state.shm_pool_size = state.h * state.stride;
  b8 shm_ok = create_shared_memory_file(state.shm_pool_size, &state);
  assert(shm_ok);

  Byte_Slice font_data = unwrap_err(read_entire_file_path(LIT("out12.bmf"), context.allocator));
  BMF_Font font;
  b8 font_ok = bmf_load_font(font_data, &font);
  assert(font_ok);

  ui_context_init(&ui_context, font, 1, 1, context.allocator);
  Byte_Slice image_data = unwrap_err(read_entire_file_path(LIT("flame.ppm"), context.temp_allocator));
  Image backing_image;
  Image rgba8_image;
  b8 ok = ppm_load_bytes(image_data, &backing_image);
  assert(ok);
  image_clone_to_rgba8(&backing_image, &rgba8_image, context.allocator);
  assert(ok);
  UI_Image image = ui_create_image(&ui_context, rgba8_image);

  struct Time last_fps_print = time_now();
  isize frames_since_print = 0;

  for_range(_i, 0, 256) {
    u8 i = _i;

    f32 f = sqrtf((f32)i / 255.0);

    __fixed_0_8_sqrt_lut[i] = (u8)(f * 255);
  }

  for_range(_i, 0, 256) {
    u8 i = _i;

    f32 x = (f32)i / 255.0;

    f32 f = x * x * (3.0f - 2.0f * x);
    __fixed_0_8_smoothstep_lut[i] = (u8)(f * 255);
  }

  while (!state.should_close) {
    if (wl_connection.builder.len) {
      Byte_Slice control_buf;
      slice_init(&control_buf, CMSG_SPACE(wl_connection.fds.len * size_of(int)), context.temp_allocator);
      struct iovec iov = {
        .base = wl_connection.builder.data,
        .len  = wl_connection.builder.len,
      };
      struct msghdr msg = {
        .name       = nil,
        .namelen    = 0,
        .control    = control_buf.data,
        .controllen = control_buf.len,
        .iov        = &iov,
        .iovlen     = 1,
      };

      struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
      cmsg->level = SOL_SOCKET;
      cmsg->type  = 1;
      cmsg->len   = CMSG_LEN(wl_connection.fds.len * size_of(int));
      mem_copy(CMSG_DATA(cmsg), wl_connection.fds.data, wl_connection.fds.len * size_of(int));

      syscall(SYS_sendmsg, wl_connection.socket, &msg, MSG_NOSIGNAL);

      vector_clear(&wl_connection.builder);
      vector_clear(&wl_connection.fds);
    }

    byte _read_buf[4096] = {0};
    byte _control_buf[256] = {0};
    Byte_Slice read_buf = {.data = _read_buf, .len = size_of(_read_buf)};
    struct iovec iov = {
      .base = _read_buf,
      .len  = size_of(_read_buf),
    };
    struct msghdr msg = {
      .control    = _control_buf,
      .controllen = size_of(_control_buf),
      .iov        = &iov,
      .iovlen     = 1,
    };
    read_buf.len = syscall(SYS_recvmsg, wl_connection.socket, &msg, 0);

    struct cmsghdr *chdr = CMSG_FIRSTHDR(&msg);
    while (chdr) {
      vector_append(&state.fds_in, *(i32 *)CMSG_DATA(chdr));
      chdr = CMSG_NXTHDR(&msg, chdr);
    }

    while (read_buf.len > 0) {
      read_buf = slice_range(read_buf, wayland_handle_message(&wl_connection, &state, read_buf), read_buf.len);
    }

    if (state.wl_compositor != 0 && state.wl_shm != 0 && state.xdg_wm_base != 0 && state.wl_surface == 0) {
      state.wl_surface   = wayland_wl_compositor_create_surface(&wl_connection, state.wl_compositor);
      state.xdg_surface  = wayland_xdg_wm_base_get_xdg_surface(&wl_connection,  state.xdg_wm_base, state.wl_surface);
      state.xdg_toplevel = wayland_xdg_surface_get_toplevel(&wl_connection,     state.xdg_surface);

      wayland_wl_surface_commit(&wl_connection, state.wl_surface);
    }

    if (state.wl_seat) {
      if (!state.wl_keyboard) {
        state.wl_keyboard = wayland_wl_seat_get_keyboard(&wl_connection, state.wl_seat);
      }
      if (!state.wl_pointer) {
        state.wl_pointer = wayland_wl_seat_get_pointer(&wl_connection, state.wl_seat);
      }
    }

    if (state.surface_state == Surface_State_Acked_Configure && state.buffer_state == Buffer_State_Released) {
      if (!state.wl_shm_pool) {
        state.wl_shm_pool = wayland_wl_shm_create_pool(&wl_connection, state.wl_shm, state.shm_fd, state.shm_pool_size);
      }
      if (!state.wl_buffer) {
        state.wl_buffer = wayland_wl_shm_pool_create_buffer(&wl_connection, state.wl_shm_pool, 0, state.w, state.h, state.stride, Wayland_Wl_Shm_Format_Xrgb8888);
      }

      state.surface_state = Surface_State_Attached;
      state.should_resize = true;
    }

    if (state.surface_state == Surface_State_Attached && state.should_resize && state.buffer_state == Buffer_State_Released) {
      wayland_wl_buffer_destroy(&wl_connection, state.wl_buffer);

      state.stride = state.w * COLOR_CHANNELS;

      if (state.shm_pool_size < state.h * state.stride) {
        isize result;
        result = syscall(SYS_munmap, state.shm_pool_data, state.shm_pool_size);
        assert(result == 0);

        state.shm_pool_size = state.h * state.stride;
        result = syscall(SYS_ftruncate, state.shm_fd, state.shm_pool_size);
        assert(result == 0);

        state.shm_pool_data =
          (u8 *)syscall(SYS_mmap, nil, state.shm_pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, state.shm_fd, 0);
        assert(state.shm_pool_data);
        wayland_wl_shm_pool_resize(&wl_connection, state.wl_shm_pool, state.shm_pool_size);
      }
    
      state.wl_buffer = wayland_wl_shm_pool_create_buffer(&wl_connection, state.wl_shm_pool, 0, state.w, state.h, state.stride, Wayland_Wl_Shm_Format_Xrgb8888);

      state.buffer_state  = Buffer_State_Released;
      state.should_resize = false;
    }

    if (state.surface_state == Surface_State_Attached && state.buffer_state == Buffer_State_Released) {
      frames_since_print += 1;

      if (time_now().nsec - last_fps_print.nsec > Second) {
        if (fps_string.data) {
          string_delete(fps_string, context.allocator);
        }
        fps_string = fmt_aprintf(context.allocator, LIT("FPS: %d"), frames_since_print);
        wayland_xdg_toplevel_set_title(&wl_connection, state.xdg_toplevel, fps_string);
        last_fps_print = time_now();
        frames_since_print = 0;
      }

      wayland_render(&state, &directory);

      wayland_wl_surface_attach(&wl_connection, state.wl_surface, state.wl_buffer, 0, 0);
      wayland_wl_surface_damage_buffer(&wl_connection, state.wl_surface, 0, 0, state.w, state.h);
      wayland_wl_surface_commit(&wl_connection, state.wl_surface);

      state.buffer_state = Buffer_State_Busy;
    }
    
    mem_free_all(context.temp_allocator);
  }

  directory_delete(directory, context.allocator);
  ui_context_destroy(&ui_context, context.allocator);

	spall_buffer_quit(&spall_ctx, &spall_buffer);
	mem_free((rawptr)buffer, buffer_size, context.allocator);
	spall_quit(&spall_ctx);
  
  tracking_allocator_fmt_results_w(&stdout, &track);
  tracking_allocator_destroy(track);

  return 0;
}
