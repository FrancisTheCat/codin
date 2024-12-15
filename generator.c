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
  String name = xml_get_property(xml, LIT("name"));

  Builder to_string;
  builder_init(&to_string, 0, 8, context.temp_allocator);
  Writer  sw = writer_from_builder(&to_string);

  String prefix_ada = string_to_ada_case(prefix, context.temp_allocator);
  String name_ada = string_to_ada_case(name, context.temp_allocator);

  fmt_wprintfln(&sw, LIT("internal String wayland_%S_%S_string(Wayland_%S_%S v) {\n\tswitch (v) {"), prefix, name, prefix_ada, name_ada);

  prefix = prefix_ada;
  name   = name_ada;

  fmt_wprintln(w, LIT("typedef enum {"));

  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("entry"))) {
      continue;
    }
    String variant_name = xml_get_property(child, LIT("name"));
    String value        = xml_get_property(child, LIT("value"));
    String enum_name    = fmt_tprintf(LIT("Wayland_%S_%S_%S"), prefix, name, string_to_ada_case(variant_name, context.temp_allocator));
    fmt_wprintfln(w, LIT("\t%S = %S,"), enum_name, value);
    fmt_wprintfln(&sw, LIT("\tcase %S:\n\t\treturn LIT(\"%S\");"), enum_name, enum_name);
  });

  fmt_wprintfln(w, LIT("} Wayland_%S_%S;\n"), prefix, name);

  fmt_wprintfln(&sw, LIT("\t}\n\treturn LIT(\"Wayland_%S_%S_Invalid_Enum_Value\");\n}\n"), prefix, name);

  fmt_wprint(w, builder_to_string(to_string));
}

void generate_request(Writer const *w, String prefix, XML_Object const *xml, isize request_id) {
  String name = xml_get_property(xml, LIT("name"));

  b8 does_return = false;
  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("arg"))) {
      continue;
    }
    if (string_equal(xml_get_property(child, LIT("type")), LIT("new_id"))) {
      assert(!does_return);
      does_return = true;
    }
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

  Builder debug_builder;
  builder_init(&debug_builder, 0, 8, context.temp_allocator);
  Writer  dw = writer_from_builder(&debug_builder);

  Builder debug_format_builder;
  builder_init(&debug_format_builder, 0, 8, context.temp_allocator);
  Writer  fw = writer_from_builder(&debug_format_builder);

  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("arg"))) {
      continue;
    }

    String arg_name = xml_get_property(child, LIT("name"));
    String arg_type = xml_get_property(child, LIT("type"));
    String arg_enum = xml_get_property(child, LIT("enum"));
    
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
      fmt_wprintfln(&bw, LIT("\tu32 %S_value = (u32)%S;"), arg_name, arg_name);
      fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S_value);"), arg_name);

      fmt_wprintf(&fw, LIT(" %S=%%S"), arg_name);
      fmt_wprintf(&dw, LIT(", %S_string(%S)"), string_to_lower(enum_name, context.temp_allocator), arg_name);
            
    } else {
      if (string_equal(arg_type, LIT("int"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", i32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

      } else if (string_equal(arg_type, LIT("uint"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", u32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

      } else if (string_equal(arg_type, LIT("object"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", u32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

      } else if (string_equal(arg_type, LIT("fixed"))) {
        args_size += 4;
        fmt_wprintf(w,     LIT(", f32 %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\ti32 %S_fixed = (i32)((f64)%S * 256.0);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\twrite_any(w, &%S_fixed);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%f"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

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

        fmt_wprintf(&fw, LIT(" %S=%%S"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

      } else if (string_equal(arg_type, LIT("fd"))) {
        fmt_wprintf(w,     LIT(", Fd %S"), arg_name);
        fmt_wprintfln(&bw, LIT("\tvector_append(&wc->fds, %S)"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", %S"), arg_name);

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

          fmt_wprint(&fw, LIT(" interface=%S version=%d"));
          fmt_wprint(&dw, LIT(", interface, version"));
        }
        args_size += 4;
        fmt_wprintln(&bw, LIT("\twc->current_id  += 1;"));
        fmt_wprintln(&bw, LIT("\tu32 return_value = wc->current_id;"));
        fmt_wprintln(&bw, LIT("\twrite_any(w, &return_value);"));

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprint(&dw, LIT(", return_value"));

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
  fmt_wprintfln(w, LIT("\tu16 _msg_size = %d;"), WAYLAND_HEADER_SIZE + args_size);
  fmt_wprintln(w,  builder_to_string(header_builder));
  fmt_wprintln(w,  LIT("\twrite_any(w, &_msg_size);"));
  fmt_wprintln(w,  builder_to_string(body_builder));
  fmt_wprintfln(w,
    LIT("\twayland_log_infof(LIT(\"-> %S@%%d.%S:%S\"), %S%S);"),
    prefix,
    name,
    builder_to_string(debug_format_builder),
    prefix,
    builder_to_string(debug_builder)
  );

  if (does_return) {
    fmt_wprintln(w, LIT("\treturn return_value;"));
  }

  fmt_wprintln(w, LIT("}\n"));
}

void generate_event_parser(Writer const *w, String prefix, XML_Object const *xml, isize event_id) {
  String prefix_lower = string_to_lower(prefix, context.temp_allocator);
  String event_name   = xml_get_property(xml, LIT("name"));

  b8 has_allocator_arg = false;

  Builder body_builder; // lol
  builder_init(&body_builder, 0, 8, context.temp_allocator);
  Writer  bw = writer_from_builder(&body_builder);

  Builder debug_builder;
  builder_init(&debug_builder, 0, 8, context.temp_allocator);
  Writer  dw = writer_from_builder(&debug_builder);

  Builder debug_format_builder;
  builder_init(&debug_format_builder, 0, 8, context.temp_allocator);
  Writer  fw = writer_from_builder(&debug_format_builder);

  fmt_wprintf(w, LIT("internal isize wayland_parse_event_%S_%S(Byte_Slice data"), prefix_lower, event_name);
  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("arg"))) {
      continue;
    }

    String arg_name = xml_get_property(child, LIT("name"));
    String arg_type = xml_get_property(child, LIT("type"));
    String arg_enum = xml_get_property(child, LIT("enum"));
    
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
      fmt_wprintf(w, LIT(", %S *%S"), enum_name, arg_name);
      fmt_wprintfln(&bw, LIT("\tu32 %S_value;"), arg_name, arg_name);
      fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, &%S_value), -1);"), arg_name);
      fmt_wprintfln(&bw, LIT("\t*%S = (%S)%S_value;"), arg_name, enum_name, arg_name);

      fmt_wprintf(&fw, LIT(" %S=%%S"), arg_name);
      fmt_wprintf(&dw, LIT(", %S_string(*%S)"), string_to_lower(enum_name, context.temp_allocator), arg_name);
            
    } else {
      if (string_equal(arg_type, LIT("int"))) {
        fmt_wprintf(w,     LIT(", i32 *%S"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, %S), -1);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", *%S"), arg_name);

      } else if (string_equal(arg_type, LIT("uint"))) {
        fmt_wprintf(w,     LIT(", u32 *%S"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, %S), -1);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", *%S"), arg_name);

      } else if (string_equal(arg_type, LIT("object"))) {
        fmt_wprintf(w,     LIT(", u32 *%S"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, %S), -1);"), arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        fmt_wprintf(&dw, LIT(", *%S"), arg_name);

      } else if (string_equal(arg_type, LIT("fixed"))) {
        fmt_wprintf(w,     LIT(", f32 *%S"), arg_name);
        fmt_wprintfln(&bw, LIT("\ti32 %S_fixed;"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, &%S_fixed), -1);"), arg_name);
        fmt_wprintfln(&bw, LIT("\t*%S = (f64)%S_fixed / 256.0;"), arg_name, arg_name);

        fmt_wprintf(&fw, LIT(" %S=%%f"), arg_name);
        fmt_wprintf(&dw, LIT(", *%S"), arg_name);

      } else if (string_equal(arg_type, LIT("string"))) {
        fmt_wprintf(w,     LIT(", String *%S"), arg_name);

        fmt_wprintfln(&bw, LIT("\tu32 %S_len;"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, &%S_len), -1);"), arg_name);
        fmt_wprintfln(&bw, LIT("\tslice_init(%S, %S_len, allocator);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_bytes(&r, string_to_bytes(*%S)), -1);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\t%S->len -= 1;"), arg_name);
        fmt_wprintfln(&bw, LIT("\tif (%S_len %% 4) {"), arg_name);
        fmt_wprintfln(&bw, LIT("\t\tbyte %S_pad_buf[4];"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\t\tor_return(read_bytes(&r, (Byte_Slice) {.data = %S_pad_buf, .len = 4 - (%S_len %% 4) }), -1);"), arg_name, arg_name);
        fmt_wprintln(&bw, LIT("\t}"));

        has_allocator_arg = true;

        fmt_wprintf(&fw, LIT(" %S=%%S"), arg_name);
        fmt_wprintf(&dw, LIT(", *%S"), arg_name);

      } else if (string_equal(arg_type, LIT("fd"))) {
        fmt_wprintf(w,     LIT(", Fd *%S"), arg_name);
        // fmt_wprintfln(&bw, LIT("\tvector_append(&wc->fds, %S)"), arg_name);

        // fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        // fmt_wprintf(&dw, LIT(", %S"), arg_name);

      } else if (string_equal(arg_type, LIT("new_id"))) {
        fmt_wprintf(w,     LIT(", u32 *%S"), arg_name);
        // fmt_wprintln(&bw, LIT("\twc->current_id  += 1;"));
        // fmt_wprintln(&bw, LIT("\tu32 return_value = wc->current_id;"));
        // fmt_wprintln(&bw, LIT("\twrite_any(w, &return_value);"));

        // fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        // fmt_wprint(&dw, LIT(", return_value"));

      } else if (string_equal(arg_type, LIT("array"))) {
        fmt_wprintf(w,     LIT(", Byte_Slice *%S"), arg_name);

        fmt_wprintfln(&bw, LIT("\tu32 %S_len;"), arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_any(&r, &%S_len), -1);"), arg_name);
        fmt_wprintfln(&bw, LIT("\tslice_init(%S, %S_len, allocator);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\tor_return(read_bytes(&r, *%S), -1);"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\tif (%S_len %% 4) {"), arg_name);
        fmt_wprintfln(&bw, LIT("\t\tbyte %S_pad_buf[4];"), arg_name, arg_name);
        fmt_wprintfln(&bw, LIT("\t\tor_return(read_bytes(&r, (Byte_Slice) {.data = %S_pad_buf, .len = 4 - (%S_len %% 4) }), -1);"), arg_name, arg_name);
        fmt_wprintln(&bw, LIT("\t}"));

        has_allocator_arg = true;

        // fmt_wprintf(&fw, LIT(" %S=%%d"), arg_name);
        // fmt_wprint(&dw, LIT(", return_value"));

      } else {
        unimplemented();
      }
    }
  })

  if (has_allocator_arg) {
    fmt_wprint(w,  LIT(", Allocator allocator"));
  }
  fmt_wprintln(w,  LIT(") {"));
  fmt_wprintln(w,  LIT("\tReader r = buffer_reader(&data);"));
  fmt_wprintln(w,  LIT("\tu32 _object_id;"));
  fmt_wprintln(w,  LIT("\tu16 _opcode, _msg_size;"));
  fmt_wprintln(w,  LIT("\tor_return(read_any(&r, &_object_id), -1);"));
  fmt_wprintln(w,  LIT("\tor_return(read_any(&r, &_opcode),    -1);"));
  fmt_wprintln(w,  LIT("\tor_return(read_any(&r, &_msg_size),  -1);"));
  fmt_wprintfln(w, LIT("\tassert(_opcode == %d);"), event_id);
  fmt_wprint(w,    builder_to_string(body_builder));

  fmt_wprintfln(w,
    LIT("\twayland_log_infof(LIT(\"<- %S@%%d.%S:%S\"), _object_id%S);"),
    prefix_lower,
    event_name,
    builder_to_string(debug_format_builder),
    builder_to_string(debug_builder)
  );

  fmt_wprintln(w,  LIT("\treturn _msg_size;"));
  fmt_wprintln(w,  LIT("}\n"));
}

void generate_events(Writer const *w, String prefix, XML_Object const *xml) {
  prefix = string_to_ada_case(prefix, context.temp_allocator);

  b8 found = false;
  slice_iter(xml->children, child, i, {
    if (string_equal(child->type, LIT("event"))) {
      found = true;
      break;
    }
  });
  if (!found) {
    return;
  }

  Builder parser_builder;
  builder_init(&parser_builder, 0, 8, context.temp_allocator);
  Writer  pw = writer_from_builder(&parser_builder);

  fmt_wprintln(w, LIT("typedef enum {"));

  isize event_id = 0;
  slice_iter(xml->children, child, i, {
    if (!string_equal(child->type, LIT("event"))) {
      continue;
    }
    String event_name = xml_get_property(child, LIT("name"));
    fmt_wprintfln(w, LIT("\tWayland_%S_Event_%S = %d,"), prefix, string_to_ada_case(event_name, context.temp_allocator), event_id);
    generate_event_parser(&pw, prefix, child, event_id);
    event_id += 1;
  });

  fmt_wprintfln(w, LIT("} Wayland_%S_Event;\n"), prefix);

  fmt_wprint(w, builder_to_string(parser_builder));
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
    "\n"
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
      String name = xml_get_property(c, LIT("name"));
      slice_iter(c->children, ic, j, {
        if (string_equal(ic->type, LIT("enum"))) {
          generate_enum(&w, name, ic);
        }
      })
    }
  })

  slice_iter(xml.children, c, i, {
    if (string_equal(c->type, LIT("interface"))) {
      String name = xml_get_property(c, LIT("name"));
      isize request_id = 0;
      slice_iter(c->children, ic, j, {
        if (string_equal(ic->type, LIT("request"))) {
          generate_request(&w, name, ic, request_id);
          request_id += 1;
        }
      })
      generate_events(&w, name, c);
    }
  })

  write_entire_file_path(out_path, builder_to_bytes(generated));

  return 0;
}
