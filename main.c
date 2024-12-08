#include "codin.h"
// #include "image.h"
#include "iter.h"

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
  Vector(isize) v;
  vector_init(&v, 0, 8, context.allocator);

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

int main() {
  // isize hour;
  // isize min;
  // isize sec;
  // isize nanos;
  // struct Time time = time_now();
  // time_precise_clock(time, &hour, &min, &sec, &nanos);
  // fmt_printfln(LIT("Time: %02d:%02d %02d.%03d"), hour + 1, min, sec, nanos / 1000000);

  // time_sleep(2 * Second);

  // time = time_now();
  // time_precise_clock(time, &hour, &min, &sec, &nanos);
  // fmt_printfln(LIT("Time: %02d:%02d %02d.%03d"), hour + 1, min, sec, nanos / 1000000);
  // return 0;
  
  Tracking_Allocator track;
  context.allocator = tracking_allocator_init(&track, context.allocator);

  context.logger = create_file_logger(1);

  Arena_Allocator arena;
  Allocator arena_allocator =
      arena_allocator_init(&arena, 1024, context.allocator);

  Directory dir = unwrap_err(read_directory_path(LIT("."), context.allocator));

  isize max_name_len = 0;
  isize max_size_len = LIT("<dir>").len;

  slice_iter(dir, file, i, {
    max_name_len = max(file->name.len, max_name_len);
    max_size_len = max(fmt_file_size_w(nil, file->size), max_size_len);
  });

  sort_slice_by(dir, i, j, string_compare_lexicographic(dir.data[i].name, dir.data[j].name));
      // ((dir.data[i].is_dir != dir.data[j].is_dir)
      //      ? dir.data[i].is_dir
      //      : string_compare_lexicographic(dir.data[i].name,
      //      dir.data[j].name)));

  String cwd = unwrap_err(_get_current_directory(context.temp_allocator));
  fmt_printfln(LIT("Directory: %S\n"), cwd);

  String spaces = slice_make(String, max_name_len, context.temp_allocator);
  slice_iter(spaces, s, _i, {
    *(char *)s = ' ';
  });

  slice_iter(dir, file, i, {
    isize n = fmt_printf(LIT("%S"), file->name);
    fmt_print(slice_range(spaces, 0, max_name_len - n));

    fmt_print(LIT(" | "));

    if (file->is_dir) {
      n = fmt_print(LIT("<dir>"));
    } else {
      n = fmt_printf(LIT("%M"), file->size);
    }
    fmt_print(slice_range(spaces, 0, max_size_len - n));

    fmt_printfln(LIT(" | %T | %T | %T"), file->creation_time, file->modification_time, file->acces_time);
  });

  fmt_println(LIT(""));

  fmt_printfln(LIT("Args: %[S]"), os_args);

  fmt_printfln(LIT("PI: %.8f"), 3.14159265359);
  fmt_printfln(LIT("E:  %.8f"), 2.71828182846);

  directory_delete(dir, context.allocator);

  Process_Creation_Args pargs = DEFAULT_PROCESS_ARGS;
  pargs.args = SLICE_VAL(Process_Args, {LIT("/bin/echo"), LIT("HELLO FROM ECHO")});
  Pid pid = unwrap_err(create_process(LIT("/bin/echo"), &pargs));

  _wait_process(pid);

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

  Socket wl_socket = wayland_display_connect();
  Wayland_State state = {
      .wl_registry = wayland_wl_display_get_registry(wl_socket),
      .w = 64,
      .h = 64,
      .stride = 64 * COLOR_CHANNELS,
  };

  // Single buffering.
  state.shm_pool_size = state.h * state.stride;
  b8 shm_ok = create_shared_memory_file(state.shm_pool_size, &state);
  assert(shm_ok);

  loop {
    byte _read_buf[4096] = {0};
    Byte_Slice read_buf = {.data = _read_buf, .len = size_of(_read_buf)};
    read_buf.len = unwrap_err(file_read(wl_socket, read_buf));

    while (read_buf.len > 0) {
      read_buf = slice_range(read_buf, handle_wayland_message(wl_socket, &state, read_buf), read_buf.len);
    }

    if (state.wl_compositor != 0 && state.wl_shm != 0 && state.xdg_wm_base != 0 && state.wl_surface == 0) {
      state.wl_surface   = wayland_wl_compositor_create_surface(wl_socket, &state);
      state.xdg_surface  = wayland_xdg_wm_base_get_xdg_surface(wl_socket, &state);
      state.xdg_toplevel = wayland_xdg_surface_get_toplevel(wl_socket, &state);

      wayland_wl_surface_commit(wl_socket, &state);
    }

    if (state.state == STATE_SURFACE_ACKED_CONFIGURE) {
      u32 *pixels = (u32 *)state.shm_pool_data;
      for_range(i, 0, state.w * state.h) {
        pixels[i] = 0xFF00FFFF;
      }

      if (!state.wl_shm_pool) {
        state.wl_shm_pool = wayland_wl_shm_create_pool(wl_socket, &state);
      }
      if (!state.wl_buffer) {
        state.wl_buffer = wayland_wl_shm_pool_create_buffer(wl_socket, &state);
      }

      wayland_wl_surface_attach(wl_socket, &state);
      wayland_wl_surface_commit(wl_socket, &state);

      state.state = STATE_SURFACE_ATTACHED;
    }
  }

  arena_allocator_destroy(arena, context.allocator);
  tracking_allocator_fmt_results_w(&stdout, &track);
  tracking_allocator_destroy(track);

  return 0;
}
