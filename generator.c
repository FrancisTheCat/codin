#include "codin.h"
#include "xml.h"

#define WAYLAND_HEADER_SIZE 8

String string_to_ada_case(String s, Allocator allocator) {
  Builder b;
  builder_init(&b, 0, s.len, allocator);

  b8 prev_underscore = true;
  slice_iter(s, c, i, {
    if (rune_is_alpha(*c)) {
      if (prev_underscore) {
        if (rune_is_lower(*c)) {
          vector_append(&b, *c + 'A' - 'a');
        } else {
          vector_append(&b, *c);
        }
      } else {
        if (rune_is_lower(*c)) {
          vector_append(&b, *c);
        } else {
          vector_append(&b, *c + 'a' - 'A');
        }
      }
    } else {
      vector_append(&b, *c);
    }
    prev_underscore = *c == '_';
  })

  return builder_to_string(b);
}

void generate_enum(Writer const *w, String prefix, XML_Object const *xml) {
  String name = {0};
  slice_iter(xml->properties, p, i, {
    if (string_equal(p->key, LIT("name"))) {
      name = p->value;
    }
  })

  prefix = string_to_ada_case(prefix, context.temp_allocator);
  name   = string_to_ada_case(name,   context.temp_allocator);

  fmt_wprintln(w, LIT("typedef enum {"));

  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("entry"))) {
      continue;
    }
    String variant_name = {0};
    slice_iter(child->properties, p, j, {
      if (string_equal(p->key, LIT("name"))) {
        variant_name = p->value;
      }
    })
    String value = {0};
    slice_iter(child->properties, p, j, {
      if (string_equal(p->key, LIT("value"))) {
        value = p->value;
      }
    })
    fmt_wprintfln(w, LIT("\tWayland_%S_%S_%S = %S,"), prefix, name, string_to_ada_case(variant_name, context.temp_allocator), value);
  });

  fmt_wprintfln(w, LIT("} Wayland_%S_%S;\n"), prefix, name);
}

void generate_request(Writer const *w, String prefix, XML_Object const *xml, isize request_id) {
  String name = {0};
  slice_iter(xml->properties, p, i, {
    if (string_equal(p->key, LIT("name"))) {
      name = p->value;
    }
  })

  b8 does_return = false;
  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("arg"))) {
      continue;
    }
    slice_iter(child->properties, p, j, {
      if (string_equal(p->key, LIT("type"))) {
        if (string_equal(p->value, LIT("new_id"))) {
          assert(!does_return);
          does_return = true;
        }
      }
    })
  });

  fmt_wprintf(
    w,
    LIT("internal %S wayland_%S_%S(Wayland_Connection *wc, u32 %S"),
    does_return ? LIT("u32") : LIT("void"),
    prefix,
    name,
    prefix
  );

  isize args_size = 0;

  Builder body_builder; // lol
  builder_init(&body_builder, 0, 8, context.temp_allocator);
  Writer  bw = writer_from_builder(&body_builder);

  Builder header_builder;
  builder_init(&header_builder, 0, 8, context.temp_allocator);
  Writer  hw = writer_from_builder(&header_builder);

  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("arg"))) {
      continue;
    }

    String arg_name = {0};
    String arg_type = {0};
    String arg_enum = {0};
    slice_iter(child->properties, p, j, {
      if (string_equal(p->key, LIT("name"))) {
        arg_name = p->value;
      }
      if (string_equal(p->key, LIT("type"))) {
        arg_type = p->value;
      }
      if (string_equal(p->key, LIT("enum"))) {
        arg_enum = p->value;
      }
    })
    
    if (arg_enum.len) {
      String enum_name = arg_enum;
      isize dot = string_index_byte(enum_name, '.');
      if (dot == -1) {
        enum_name = string_to_ada_case(
          fmt_tprintf(LIT("Wayland_%S_%S"), prefix, arg_enum),
          context.temp_allocator
        );
      } else {
        enum_name = string_to_ada_case(
          fmt_tprintf(LIT("Wayland_%S_%S"), slice_end(arg_enum, dot), slice_start(arg_enum, dot + 1)),
          context.temp_allocator
        );
      }
      args_size += 4;
      fmt_wprintf(w, LIT(", %S %S"), enum_name, arg_name);
      fmt_wprintfln(&bw, LIT("\ti32 %S_value = (i32)%S;"), arg_name, arg_name);
      fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S_value);"), arg_name);
    } else {
      if (string_equal(arg_type, LIT("int"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", i32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

      } else if (string_equal(arg_type, LIT("uint"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", u32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

      } else if (string_equal(arg_type, LIT("object"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", u32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

      } else if (string_equal(arg_type, LIT("fixed"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", f32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\ti32 %S_fixed = (i32)((f64)%S * 256.0);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S_fixed);"), arg_name);

      } else if (string_equal(arg_type, LIT("string"))) {
        args_size += 4;
        fmt_wprintf(w, LIT(", String %S"), arg_name);

        fmt_wprintfln(&hw, LIT("\tisize %S_size = roundup_4(%S.len + 1);"), arg_name, arg_name);
        fmt_wprintfln(&hw, LIT("\t_msg_size += %S_size;"), arg_name);

        fmt_wprintfln(&bw, LIT("\ti32 %S_write_len = (i32)%S.len + 1;"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S_write_len);"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_string(w, %S);"), arg_name);

        fmt_wprintfln(&bw, LIT("\tfor_range(i, %S.len, %S_size) {"), arg_name, arg_name);
        fmt_wprintln(&bw , LIT("\t\twrite_byte(w, 0);"));
        fmt_wprintln(&bw,  LIT("\t}"));

      } else if (string_equal(arg_type, LIT("fd"))) {
        fmt_wprintf(w,     LIT(", Fd %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\tvector_append(&wc->fds, %S)"), arg_name);

      } else if (string_equal(arg_type, LIT("new_id"))) {
        if (string_equal(prefix, LIT("wl_registry")) && string_equal(name, LIT("bind"))) {
          fmt_wprint(w, LIT(", String interface, u32 version"));

          fmt_wprintln(&hw, LIT("\tisize interface_size = roundup_4(interface.len + 1);"));
          fmt_wprintln(&hw, LIT("\t_msg_size += interface_size;"));

          fmt_wprintln(&bw, LIT("\ti32 interface_write_len = (i32)interface.len + 1;"));
          fmt_wprintln(&bw, LIT("\twrite_any(w, &interface_write_len);"));
          fmt_wprintln(&bw, LIT("\twrite_string(w, interface);"));
          fmt_wprintln(&bw, LIT("\tfor_range(i, interface.len, interface_size) {"));
          fmt_wprintln(&bw , LIT("\t\twrite_byte(w, 0);"));
          fmt_wprintln(&bw,  LIT("\t}"));
          fmt_wprintln(&bw, LIT("\twrite_any(w, &version);"));
          args_size += 8;
        }
        args_size += 4;
        fmt_wprintln(&bw, LIT("\twc->current_id  += 1;"));
        fmt_wprintln(&bw, LIT("\tu32 return_value = wc->current_id;"));
        fmt_wprintln(&bw, LIT("\twrite_any(w, &return_value);"));

      } else {
        unimplemented();
      }
    }
  })

  fmt_wprintln(w, LIT(") {"));

  fmt_wprintln(w, LIT("\tWriter _w = writer_from_builder(&wc->builder);"));
  fmt_wprintln(w, LIT("\tWriter *w = &_w;"));

  fmt_wprintfln(w, LIT("\twrite_any(w, &%S);"),  prefix);
  fmt_wprintfln(w, LIT("\tu16 _opcode = %d;"),   request_id);
  fmt_wprintln(w,  LIT("\twrite_any(w, &_opcode);"));
  fmt_wprintfln(w, LIT("\tu16 _msg_size = %d;"),     WAYLAND_HEADER_SIZE + args_size);
  fmt_wprintln(w,  builder_to_string(header_builder));
  fmt_wprintln(w,  LIT("\twrite_any(w, &_msg_size);"));
  fmt_wprintln(w,  builder_to_string(body_builder));
  fmt_wprintfln(w, LIT("\twayland_log_infof(LIT(\"-> %S@%%d.%S:\"), %S);"), prefix, name, prefix);

  if (does_return) {
    fmt_wprintln(w, LIT("\treturn return_value;"));
  }

  fmt_wprintln(w, LIT("}\n"));
}

int main() {
  if (os_args.len != 3) {
    fmt_eprintln(LIT("Usage: ./generator in.xml out.h"));
    return 1;
  }

  String in_path  = os_args.data[1];
  String out_path = os_args.data[2];

  Builder generated;
  builder_init(&generated, 0, 8, context.allocator);
  Writer  w = writer_from_builder(&generated);

  fmt_wprint(&w, LIT(
    "// Generated from XML\n"
    "#include \"codin.h\"\n"
    "#include \"wayland_gen_common.h\"\n"
    "\n"
  ));

  Byte_Slice xml_data = unwrap_err(read_entire_file_path(in_path, context.allocator));
  XML_Object xml;
  b8 xml_ok = xml_parse_file(bytes_to_string(xml_data), &xml, context.allocator);
  assert(xml_ok);

  slice_iter(xml.children, c, i, {
    if (string_equal(c->type, LIT("interface"))) {
      String name;
      slice_iter(c->properties, p, i, {
        if (string_equal(p->key, LIT("name"))) {
          name = p->value;
        }
      })
      slice_iter(c->children, ic, j, {
        if (string_equal(ic->type, LIT("enum"))) {
          generate_enum(&w, name, ic);
        }
      })
    }
  })

  slice_iter(xml.children, c, i, {
    if (string_equal(c->type, LIT("interface"))) {
      String name;
      slice_iter(c->properties, p, i, {
        if (string_equal(p->key, LIT("name"))) {
          name = p->value;
        }
      })
      isize request_id = 0;
      isize event_id   = 0;
      slice_iter(c->children, ic, j, {
        if (string_equal(ic->type, LIT("request"))) {
          generate_request(&w, name, ic, request_id);
          request_id += 1;
        }
        if (string_equal(ic->type, LIT("event"))) {
          // generate_event(&w, name, ic, event_id);
          event_id += 1;
        }
      })
    }
  })

  write_entire_file_path(out_path, builder_to_bytes(generated));

  return 0;
}
