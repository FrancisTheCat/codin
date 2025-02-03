#include "codin.h"
#include "image.h"
#include "iter.h"
#include "xml.h"

#define ttf_alloc(U, size) (unwrap_err(mem_alloc(size, *(Allocator *)U)))
#define ttf_free(U, ptr) ((void)mem_free(ptr, 0, *(Allocator *)U))

#define ttf_sort_f32s(ptr, n) {                                          \
  Slice(f32) ttf_sort_f32s_slice = {.data = ptr, .len = n};              \
  sort_slice_by(                                                         \
    ttf_sort_f32s_slice,                                                 \
    index_i,                                                             \
    index_j,                                                             \
    ttf_sort_f32s_slice.data[index_i] >                                  \
    ttf_sort_f32s_slice.data[index_j]                                    \
  );                                                                     \
}

#define ttf_memcpy mem_copy

#define ttf_absf ttf_absf
f32 ttf_absf(f32 x) {
  return x > 0 ? x : -x;
}

// #define ttf_sqrtf ttf_sqrtf
// f32 ttf_sqrtf(f32 x) {
//     f32 xhalf = 0.5f * x;
//     union {
//         f32 x;
//         i32 i;
//     } u;
//     u.x = x;
//     u.i = 0x5f375a86 - (u.i >> 1);
//     for_range(i, 0, 1000) {
//       u.x = u.x * (1.5f - xhalf * u.x * u.x);
//     }
//     return 1.0 / u.x;
// }

#define ttf_sqrtf(x) sqrt(x)

#define TTF_IMPLEMENTATION
#include "ttf.h"
#undef TTF_IMPLEMENTATION

#include "wayland.h"

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

b8 spall_write_callback(SpallProfile *self, const void *data, isize length) {
  return file_write((Fd)self->data, (Byte_Slice) {.data = (byte *)data, .len = length}).err == OSE_None;
}

void spall_close_callback(SpallProfile *self) {
  file_close((Fd)self->data);
}

i32 main() {
  Tracking_Allocator track;
  context.allocator = tracking_allocator_init(&track, context.allocator);

  context.logger = create_file_logger(1);

  Fd spall_fd = unwrap_err(file_open(LIT("trace.spall"), FP_Create | FP_Read_Write | FP_Truncate));
  spall_ctx   = spall_init_callbacks(1, spall_write_callback, nil, spall_close_callback, (rawptr)spall_fd);

 //  File_Info fi;
 //  file_stat(spall_fd, &fi);
 //  assert(fi.readable);
 //  assert(fi.writeable);
 //  assert(!fi.executable);

	Byte_Slice spall_buffer_backing = slice_make(Byte_Slice, 1024 * 1024, context.allocator);
	spall_buffer = (SpallBuffer){
		.length = spall_buffer_backing.len,
		.data   = spall_buffer_backing.data,
	};

	spall_buffer_init(&spall_ctx, &spall_buffer);

  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("read_directory"), get_time_in_micros());
 //  Directory directory = unwrap_err(read_directory_path(LIT("."), context.allocator));
 //  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

 //  isize max_name_len = 0;
 //  isize max_size_len = LIT("<dir>").len;

 //  slice_iter(directory, file, i, {
 //    max_name_len = max(file->name.len, max_name_len);
 //    max_size_len = max(fmt_file_size_w(nil, file->size), max_size_len);
 //  });

 //  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("sort"), get_time_in_micros());
 //  // sort_slice_by(directory, i, j, string_compare_lexicographic(directory.data[i].name, directory.data[j].name));
 //  sort_slice_by(
 //    directory,
 //    i,
 //    j,
 //    (directory.data[i].is_dir != directory.data[j].is_dir)
 //     ? directory.data[i].is_dir
 //     : string_compare_lexicographic(directory.data[i].name, directory.data[j].name)
 //  );
 //  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

 //  String cwd = unwrap_err(_get_current_directory(context.temp_allocator));
 //  fmt_printfln(LIT("Directory: %S\n"), cwd);

 //  String spaces = slice_make(String, max_size_len, context.temp_allocator);
 //  slice_iter(spaces, s, _i, {
 //    *(char *)s = ' ';
 //  });

 //  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("print_directory"), get_time_in_micros());

 //  String name_format = fmt_tprintf(LIT("%%-%dS | "), max_name_len);

 //  Builder b = builder_make(0, 1024, context.temp_allocator);
  
 //  slice_iter(directory, file, i, {
 //    fmt_sbprintf(&b, name_format, file->name);

 //    String str;
 //    if (file->is_dir) {
 //      str = fmt_tprintf(LIT("<dir>"), 0);
 //    } else {
 //      str = fmt_tprintf(LIT("%M"), file->size);
 //    }

 //    fmt_sbprintf(
 //      &b,
 //      LIT("%S%S | %T | %T | %T\n"),
 //      slice_range(spaces, 0, max_size_len - str.len),
 //      str,
 //      file->creation_time,
 //      file->modification_time,
 //      file->acces_time
 //    );
 //  });
  
 //  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

 //  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("write_directory"), get_time_in_micros());
 //  write_bytes(&stdout, builder_to_bytes(b));
 //  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());

 //  fmt_println(LIT(""));

 //  fmt_printfln(LIT("Args: %[S]"), os_args);

 //  fmt_printfln(LIT("PI: %.8f"), 3.14159265359);
 //  fmt_printfln(LIT("E:  %.8f"), 2.71828182846);

 //  Test_Context tc;
 //  test_context_init(&tc, context.allocator);

  // test_add(&tc, test_growing_arena_allocator);
  // test_add(&tc, test_pool_allocator);
  // test_add(&tc, test_heap_allocator);
  // test_add(&tc, test_hash_map);
  // test_add(&tc, test_vector);
  // test_add(&tc, test_sort);

  // test_context_execute(&tc);
  // test_context_destroy(tc);

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

  byte _connection_buffer[1024];
  Wayland_Connection wl_connection;
  b8 conn_ok = wayland_display_connect(context.allocator, (Byte_Slice) {.data = _connection_buffer, .len = size_of(_connection_buffer)}, &wl_connection);
  assert(conn_ok);
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

  // Byte_Slice font_data = unwrap_err(read_entire_file_path(LIT("out12.bmf"), context.allocator));
  // BMF_Font font;
  // b8 font_ok = bmf_load_font(font_data, &font);
  // assert(font_ok);

  Growing_Arena_Allocator font_arena;
  Allocator font_allocator = growing_arena_allocator_init(&font_arena, 4096, context.allocator);

  Byte_Slice ttf_font_data = unwrap_err(read_entire_file_path(LIT("JetBrainsMonoNerdFont-Medium.ttf"), font_allocator));
  // Byte_Slice ttf_font_data = unwrap_err(read_entire_file_path(LIT("Roboto-Regular.ttf"), font_allocator));
  // Byte_Slice ttf_font_data = unwrap_err(read_entire_file_path(LIT("Lexend-Regular.ttf"), font_allocator));
  // Byte_Slice ttf_font_data = unwrap_err(read_entire_file_path(LIT("Crimson-Roman.ttf"), font_allocator));

  b8 font_ok = ttf_load_bytes(ttf_font_data.data, ttf_font_data.len, &ttf_font, &font_allocator);
  assert(font_ok);

  ui_context_init(&ui_context, measure_text_ttf, 1, 1, ttf_get_font_height(&ttf_font, UI_FONT_SIZE), context.allocator);

  ui_hash_chunks_x = (state.w + UI_HASH_CHUNK_SIZE - 1) / UI_HASH_CHUNK_SIZE;
  ui_hash_chunks_y = (state.h + UI_HASH_CHUNK_SIZE - 1) / UI_HASH_CHUNK_SIZE;
  slice_init(&ui_hash_chunks, ui_hash_chunks_x * ui_hash_chunks_y, context.allocator);
  slice_init(&ui_prev_chunks, ui_hash_chunks_x * ui_hash_chunks_y, context.allocator);
  
  Byte_Slice image_data = unwrap_err(read_entire_file_path(LIT("flame.ppm"), context.temp_allocator));
  Image backing_image;
  Image rgba8_image;
  b8 ok = ppm_load_bytes(image_data, &backing_image);
  assert(ok);
  image_clone_to_rgba8(&backing_image, &rgba8_image, context.allocator);
  assert(ok);
  UI_Image image = ui_create_image(&ui_context, rgba8_image);
  assert(image.index == 0);

  struct Time last_fps_print = time_now();
  isize frames_since_print = 0;

  while (!state.should_close) {
    wayland_connection_flush(&wl_connection);
    wayland_recieve_messages(&wl_connection);
    wayland_handle_events(&wl_connection, &state);

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

      if (!!state.wl_data_device_manager && !state.wl_data_source) {
        state.wl_data_source = wayland_wl_data_device_manager_create_data_source(&wl_connection, state.wl_data_device_manager);
        wayland_wl_data_source_offer(&wl_connection, state.wl_data_source, LIT("text/plain"));
        state.wl_data_device = wayland_wl_data_device_manager_get_data_device(&wl_connection, state.wl_data_device_manager, state.wl_seat);
        wayland_wl_data_device_set_selection(&wl_connection, state.wl_data_device, state.wl_data_source, 0);
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

      ui_hash_chunks_x = (state.w + UI_HASH_CHUNK_SIZE - 1) / UI_HASH_CHUNK_SIZE;
      ui_hash_chunks_y = (state.h + UI_HASH_CHUNK_SIZE - 1) / UI_HASH_CHUNK_SIZE;
      slice_delete(ui_hash_chunks, context.allocator);
      slice_delete(ui_prev_chunks, context.allocator);
      slice_init(&ui_hash_chunks, ui_hash_chunks_x * ui_hash_chunks_y, context.allocator);
      slice_init(&ui_prev_chunks, ui_hash_chunks_x * ui_hash_chunks_y, context.allocator);
  
      state.buffer_state  = Buffer_State_Released;
      state.should_resize = false;
    }

    if (state.surface_state == Surface_State_Attached && state.buffer_state == Buffer_State_Released) {
      frames_since_print += 1;

      struct Time current_time = time_now();

      if (time_diff(current_time, last_fps_print) > Second) {
        if (fps_string.data) {
          string_delete(fps_string, context.allocator);
        }
        fps_string = fmt_aprintf(context.allocator, LIT("FPS: %d"), frames_since_print);
        wayland_xdg_toplevel_set_title(&wl_connection, state.xdg_toplevel, fps_string);
        last_fps_print = current_time;
        frames_since_print = 0;
      }

      if (!render_threads.len) {
        renderer_init(&ui_context, &state, 8);
      }

      Directory directory = {0};
      wayland_render(&wl_connection, &state, &directory);

      wayland_wl_surface_attach(&wl_connection, state.wl_surface, state.wl_buffer, 0, 0);
      wayland_wl_surface_damage_buffer(&wl_connection, state.wl_surface, 0, 0, state.w, state.h);
      wayland_wl_surface_commit(&wl_connection, state.wl_surface);

      state.buffer_state = Buffer_State_Busy;
    }
    
    mem_free_all(context.temp_allocator);
  }

  renderer_destroy();

  growing_arena_allocator_destroy(font_arena);

  // directory_delete(directory, context.allocator);
  ui_context_destroy(&ui_context, context.allocator);

  if (state.keymap_data.data) {
    xkb_keymap_destroy(&state.keymap, context.allocator);
    os_deallocate_pages(state.keymap_data.data, state.keymap_data.len);
  }
  wayland_connection_destroy(&wl_connection);

  slice_delete(rgba8_image.pixels, context.allocator);
  // slice_delete(font_data, context.allocator);
  if (fps_string.len) {
    slice_delete(fps_string, context.allocator);
  }

  slice_delete(ui_hash_chunks, context.allocator);
  slice_delete(ui_prev_chunks, context.allocator);

	spall_buffer_quit(&spall_ctx, &spall_buffer);
	spall_quit(&spall_ctx);
	slice_delete(spall_buffer_backing, context.allocator);

  tracking_allocator_fmt_results_w(&stdout, &track);
  tracking_allocator_destroy(track);

  return 0;
}
