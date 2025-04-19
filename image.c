#include "image.h"

#include "strings.h"
#include "fmt.h"
#include "log.h"
#include "deflate.h"
#include "hash.h"

#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_ASSERT(x) assert(x)

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

extern b8 stb_image_load_bytes(Byte_Slice data, Image *image, Allocator allocator) {
  i32 x, y, c;
  image->pixels.data = stbi_load_from_memory(data.data, data.len, &x, &y, &c, 0);
  if (!image->pixels.data) {
    return false;
  }

  image->width      = x;
  image->stride     = x;
  image->height     = y;
  image->components = c;
  image->pixel_type = PT_u8;
  image->pixels.len = x * y * c;

  return true;
}

#include "zlib.h"

struct IHDR {
  u32 width;
  u32 height;
  u8  bit_depth;
  u8  color_type;
  u8  compression_method;
  u8  filter_method;
  u8  interlace_method;
} __attribute__ ((packed));

internal rawptr z_alloc_proc(rawptr allocator, u32 items, u32 size) {
  Allocator *a = (Allocator *)allocator;
  return unwrap_err(mem_alloc(items * size, *a));
}

internal void z_free_proc(rawptr allocator, rawptr address) {
  Allocator *a = (Allocator *)allocator;
  mem_free(address, 0, *a);
}

internal isize abs_isize(isize x) {
  return x > 0 ? x : -x;
}

// a = left, b = above, c = upper left
internal u8 paeth_predictor (u8 a, u8 b, u8 c) {
  isize p  = a + b - c;
  isize pa = abs_isize(p - a);
  isize pb = abs_isize(p - b);
  isize pc = abs_isize(p - c);
  if (pa <= pb && pa <= pc) {
    return a;
  } else if (pb <= pc) {
    return b;
  } else {
    return c;
  }
}

#define CHUNK 16384

internal b8 zlib_inflate(Reader source, Writer dest) {
  isize ret;
  usize have;
  z_stream strm;
  byte in_buf[CHUNK];
  byte out_buf[CHUNK];
  Byte_Slice in  = {.data = in_buf,  .len = CHUNK};
  Byte_Slice out = {.data = out_buf, .len = CHUNK};

  /* allocate inflate state */
  strm.zalloc = z_alloc_proc;
  strm.zfree = z_free_proc;
  strm.opaque = &context.allocator;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK) {
    return false;
  }

  /* decompress until deflate stream ends or end of file */
  do {
    strm.avail_in = or_do(read_bytes(&source, in), {
      (void)inflateEnd(&strm);
      return false;
    });
    if (strm.avail_in == 0) {
      break;
    }
    strm.next_in = in.data;

    /* run inflate() on input until output buffer not full */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out.data;
      ret = inflate(&strm, Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
      switch (ret) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        (void)inflateEnd(&strm);
        return false;
      }
      have = CHUNK - strm.avail_out;
      if (or_else(write_bytes(&dest, slice_end(out, have)), -1) != have) {
        (void)inflateEnd(&strm);
        return false;
      }
    } while (strm.avail_out == 0);
    /* done when inflate() says it's done */
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  (void)inflateEnd(&strm);
  return ret == Z_STREAM_END;
}

// internal b8 zlib_deflate(Reader const *source, Writer const *dest) {
//   isize ret, flush;
//   usize have;
//   z_stream strm;
//   byte in_buf[CHUNK];
//   byte out_buf[CHUNK];
//   Byte_Slice in  = {.data = in_buf,  .len = CHUNK};
//   Byte_Slice out = {.data = out_buf, .len = CHUNK};

//   strm.zalloc = z_alloc_proc;
//   strm.zfree  = z_free_proc;
//   strm.opaque = &context.allocator;

//   strm.avail_in = 0;
//   strm.next_in  = Z_NULL;

//   ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
//   if (ret != Z_OK) {
//     return false;
//   }

//   do {
//     strm.avail_in = or_do(read_bytes(source, in), {
//       (void)deflateEnd(&strm);
//       return false;
//     });
//     flush = strm.avail_in ? Z_NO_FLUSH : Z_FINISH;
//     strm.next_in = in.data;

//     do {
//       strm.avail_out = CHUNK;
//       strm.next_out = out.data;
//       ret = deflate(&strm, flush);
//       assert(ret != Z_STREAM_ERROR);
//       have = CHUNK - strm.avail_out;
//       if (or_else(write_bytes(dest, slice_end(out, have)), -1) != have) {
//         (void)deflateEnd(&strm);
//         return false;
//       }
//     } while (strm.avail_out == 0);
//     assert(strm.avail_in == 0);
//   } while (flush != Z_FINISH);
//   assert(ret == Z_STREAM_END);

//   (void)deflateEnd(&strm);
//   return true;
// }

// #undef CHUNK

#define PNG_HEADER LIT("\x89PNG\x0d\x0a\x1a\x0a")

extern b8 png_load_bytes(Byte_Slice data, Image *image, Allocator allocator) {
  if (!string_equal(PNG_HEADER, (String) {.data = (char *)data.data, .len = min(data.len, 8)})) {
    return false;
  }

  isize i = 8;

  Byte_Slice ihdr_chunk = (Byte_Slice){.data = &data.data[i + 8]};
  ihdr_chunk.len = endianness_swap(*(u32*)&data.data[i]);
  if (!string_equal((String) {.data = (char *)&data.data[i + 4], .len = 4}, LIT("IHDR"))) {
    return false;
  }
  i += ihdr_chunk.len + 12;

  struct IHDR ihdr = *(struct IHDR*)&ihdr_chunk.data[0];

  if (size_of(struct IHDR) != ihdr_chunk.len) {
    return false;
  }

  image->components = 1;
  isize bpp = 1;

  if (ihdr.color_type & PNG_CT_Color) {
    image->components += 2;
    bpp += 2;
  }
  if (ihdr.color_type & PNG_CT_Alpha) {
    image->components += 1;
    bpp += 1;
  }
  if (ihdr.color_type & PNG_CT_Palette) {
    return false;
  }

  image->pixel_type = PT_u8;

  switch (ihdr.bit_depth) {
  case 1:
  case 2:
  case 4:
    return false;
  case 8:
    break;
  case 16:
    image->pixel_type = PT_u16;
    bpp *= 2;
    break;
  }

  ihdr.width  = endianness_swap(ihdr.width);
  ihdr.height = endianness_swap(ihdr.height);

  image->width  = ihdr.width;
  image->height = ihdr.height;

  Byte_Buffer pixels = vector_make(Byte_Buffer, 0, data.len, allocator);

  Byte_Buffer temp_buffer = vector_make(Byte_Buffer, 0, data.len, allocator);
  Writer buffer_writer = writer_from_buffer(&temp_buffer);

  Byte_Buffer compressed_data = vector_make(Byte_Buffer, 0, data.len, allocator);

  while (i < data.len) {
    i += 8;
    Byte_Slice chunk = (Byte_Slice){.data = &data.data[i]};
    chunk.len = endianness_swap(*(u32*)&chunk.data[-8]);
    String type = (String) {.data = (char *)&chunk.data[-4], .len = 4};

    Byte_Slice hash_chunk = {
      .data = &chunk.data[-4],
      .len = chunk.len + 4,
    };
    u32 hash = ~hash_bytes(~0, hash_crc_32, hash_chunk);

    if (hash != endianness_swap(*(u32 *)&chunk.data[chunk.len])) {
      goto fail;
    }

    if (string_equal(type, LIT("IDAT"))) {
      vector_append_slice(&compressed_data, chunk);
    } else if (string_equal(type, LIT("IEND"))) {
      break;
    } else {
      log_warnf(LIT("Unknown chunk type: '%S'"), type);
    }

    i += chunk.len + 4;
  }

  Byte_Slice buf = vector_to_slice(Byte_Slice, compressed_data);

  // {
  //   Byte_Slice copy = buf; 
  //   Reader reader = buffer_reader(&copy);
  //   Writer writer = null_writer();
  //   b8 result = my_inflate(&reader, &writer);
  //   log_infof(LIT("result: %B"), (isize)result);
  // }

  b8 inflate_ok = zlib_inflate(buffer_reader(&buf), buffer_writer);
  if (buf.len != 0 || !inflate_ok) {
    goto fail;
  }
  
  for_range(y, 0, ihdr.height) {
    byte filter = temp_buffer.data[bpp * y * ihdr.width + y];

    Byte_Slice prev_row;
    prev_row.len = ihdr.width * bpp;
    prev_row.data = temp_buffer.data + (bpp * (y - 1) * ihdr.width + y);

    Byte_Slice row;
    row.len = ihdr.width * bpp;
    row.data = temp_buffer.data + (bpp * y * ihdr.width + y + 1);

    switch (filter) {
    case 0:
      break;
    // sub
    case 1: 
      for_range(x, 0, ihdr.width * bpp) {
        u8 prev = (x >= bpp) ? row.data[x - bpp] : 0;
        row.data[x] += prev;
      }
      break;
      
    // up (could be simd)
    case 2:
      for_range(x, 0, ihdr.width * bpp) {
        u8 prev = (y == 0) ? 0 : prev_row.data[x];
        row.data[x] += prev;
      }
      break;

    // average
    case 3:
      for_range(x, 0, ihdr.width * bpp) {
        u8 up   = (y == 0) ? 0 : prev_row.data[x];
        u8 left = (x >= bpp) ?      row.data[x - bpp] : 0;

        row.data[x] += (up + left) / 2;
      }
      break;

    // paeth
    case 4:
      for_range(x, 0, ihdr.width * bpp) {
        u8 up      = (y == 0) ? 0 :           prev_row.data[x];
        u8 up_left = (y == 0) ? 0 : (x >= bpp) ? prev_row.data[x - bpp] : 0;
        u8 left    = (x >= bpp) ?      row.data[x - bpp] : 0;

        row.data[x] += paeth_predictor(left, up, up_left);
      }
      break;

    default:
      return false;
    }

    vector_append_slice(&pixels, row);
  }

  vector_delete(temp_buffer);
  vector_delete(compressed_data);

  image->pixels.data = pixels.data;
  image->pixels.len  = pixels.len;

  image->stride = image->width;
  
  return true;

fail:
  vector_delete(pixels);
  vector_delete(temp_buffer);
  vector_delete(compressed_data);

  return false;
}

internal void _png_write_chunk(Writer const *writer, String type, Byte_Slice chunk) {
  u32 hash = ~0;
  Hashing_Writer_32 hasher = {
    .hash = &hash,
    .next = writer,
    .proc = hash_crc_32,
  };
  Writer hashing_writer = hashing_writer_32(&hasher);
  u32 len = endianness_swap((u32)chunk.len);
  write_any(writer, &len);
  write_string(&hashing_writer, type);
  write_bytes(&hashing_writer, chunk);
  hash = ~endianness_swap(hash);
  write_any(writer, &hash);
}

extern b8 png_save_writer(Writer const *writer, Image const *image) {
  if (!write_string(writer, PNG_HEADER).ok) {
    return false;
  }

  struct IHDR ihdr = {0};

  isize bpp = image->components * (image->pixel_type == PT_u8 ? 1 : 2);
  if (image->pixels.len != bpp * image->width * image->height) {
    return false;
  }

  ihdr.width  = endianness_swap((u32)((Image *)image)->width);
  ihdr.height = endianness_swap((u32)((Image *)image)->height);
  ihdr.bit_depth = image->pixel_type == PT_u8 ? 8 : 16;
  switch (image->components) {
  case 1:
    ihdr.color_type = 0;
    break;
  case 3:
    ihdr.color_type = PNG_CT_Color;
    break;
  case 4:
    ihdr.color_type = PNG_CT_Color | PNG_CT_Alpha;
    break;
  default:
    return false;
  }

  _png_write_chunk(writer, LIT("IHDR"), (Byte_Slice){.data = (byte *)&ihdr, .len = size_of(ihdr)});

  Byte_Buffer uncompressed = vector_make(Byte_Buffer, 0, 1024, context.allocator);
  Byte_Buffer compressed   = vector_make(Byte_Buffer, 0, 1024, context.allocator);

  Byte_Slice prev_row;
  Byte_Slice current_row;

  isize histogram[256] = {0};
  isize max_simplicity = 0;
  isize best_filter;

  for_range(y, 0, image->height) {
    prev_row = slice_range_type(
      Byte_Slice,
      ((Image *)image)->pixels,
      (y - 1) * image->width * bpp,
      y       * image->width * bpp
    );
    current_row = slice_range_type(
      Byte_Slice,
      ((Image *)image)->pixels,
      y        * image->width * bpp,
      (y + 1)  * image->width * bpp
    );

    for_range(filter, 0, 5) {
      switch (filter) {
      case 0:
        for_range(x, 0, image->width * bpp) {
          histogram[current_row.data[x]] += 1;
        }
        break;
      case 1:
        for_range(x, 0, image->width * bpp) {
          u8 left = (x >= bpp) ? current_row.data[x - bpp] : 0;
          histogram[(u8)(current_row.data[x] - left)] += 1;
        }
        break;
      case 2:
        for_range(x, 0, image->width * bpp) {
          u8 up = (y == 0) ? 0 : prev_row.data[x];
          histogram[(u8)(current_row.data[x] - up)] += 1;
        }
        break;
      case 3:
        for_range(x, 0, image->width * bpp) {
          u8 up   = (y == 0) ? 0 : prev_row.data[x];
          u8 left = (x >= bpp) ? current_row.data[x - bpp] : 0;
          histogram[(u8)(current_row.data[x] - (u8)(((u16)up + (u16)left) / 2))] += 1;
        }
        break;
      case 4:
        for_range(x, 0, image->width * bpp) {
          u8 up      = (y == 0) ? 0 : prev_row.data[x];
          u8 up_left = (y == 0) ? 0 : (x >= bpp) ? prev_row.data[x - bpp] : 0;
          u8 left    = (x >= bpp) ? current_row.data[x - bpp] : 0;

          histogram[(u8)(current_row.data[x] - paeth_predictor(left, up, up_left))] += 1;
        }
        break;
      }

      isize simplicity = 0;
      for_range(i, 0, count_of(histogram)) {
        simplicity += histogram[i] * (8 * size_of(isize) - __builtin_clzll(histogram[i]));
      }
      if (simplicity > max_simplicity) {
        best_filter    = filter;
        max_simplicity = simplicity;
      }

      mem_zero(&histogram, size_of(histogram));
    }

    vector_append(&uncompressed, best_filter);
    switch (best_filter) {
      case 0:
        vector_append_slice(&uncompressed, current_row);
        break;
      case 1:
        for_range(x, 0, image->width * bpp) {
          u8 left = (x >= bpp) ? current_row.data[x - bpp] : 0;
          vector_append(&uncompressed, current_row.data[x] - left);
        }
        break;
      case 2:
        for_range(x, 0, image->width * bpp) {
          u8 up = (y == 0) ? 0 : prev_row.data[x];
          vector_append(&uncompressed, current_row.data[x] - up);
        }
        break;
      case 3:
        for_range(x, 0, image->width * bpp) {
          u8 up   = (y == 0) ? 0 : prev_row.data[x];
          u8 left = (x >= bpp) ? current_row.data[x - bpp] : 0;
          vector_append(&uncompressed, current_row.data[x] - (u8)(((u16)up + (u16)left) / 2));
        }
        break;
      case 4:
        for_range(x, 0, image->width * bpp) {
          u8 up      = (y == 0) ? 0 : prev_row.data[x];
          u8 up_left = (y == 0) ? 0 : (x >= bpp) ? prev_row.data[x - bpp] : 0;
          u8 left    = (x >= bpp) ? current_row.data[x - bpp] : 0;

          vector_append(&uncompressed, current_row.data[x] - paeth_predictor(left, up, up_left));
        }
        break;
    }

    max_simplicity = 0;
  }

  Byte_Slice uncompressed_slice = (Byte_Slice) {.data = uncompressed.data, .len = uncompressed.len};

  Reader compression_reader = buffer_reader(&uncompressed_slice); 
  Writer compression_writer = writer_from_buffer(&compressed);
  b8 result = my_deflate(&compression_reader, &compression_writer);
  assert(result);

  _png_write_chunk(writer, LIT("IDAT"), (Byte_Slice){.data = compressed.data, .len = compressed.len});

  _png_write_chunk(writer, LIT("IEND"), (Byte_Slice){0});

  return true;
}

extern b8 ppm_save_writer(Writer const *writer, Image const *image) {
  if (image->components != 3 || image->pixel_type != PT_u8) {
    return false;
  }
  fmt_wprintf(writer, LIT("P6 %d %d 255\n"), image->width, image->height);
  return write_bytes(writer, slice_to_bytes(image->pixels)).ok;
}

extern b8 ppm_load_bytes(Byte_Slice data, Image *image) {
  if (data.len < 7 || data.data[0] != 'P' || data.data[1] != '6') {
    return false;
  }
  image->components = 3;
  image->pixel_type = PT_u8;

  isize i = 3;

  for (; i < data.len; i += 1) {
    b8 stop = false;
    switch (data.data[i]) {
    case '\t':
    case '\n':
    case '\r':
    case ' ':
      break;
    default:
      stop = true;
    }
    if (stop) {
      break;
    }
  }

  image->width  = 0;
  image->height = 0;

  for (; i < data.len; i += 1) {
    if (in_range(data.data[i], '0', '9' + 1)) {
      image->width *= 10;
      image->width += data.data[i] - '0';
    } else {
      break;
    }
  }
  image->stride = image->width;
  
  for (; i < data.len; i += 1) {
    b8 stop = false;
    switch (data.data[i]) {
    case '\t':
    case '\n':
    case '\r':
    case ' ':
      break;
    default:
      stop = true;
    }
    if (stop) {
      break;
    }
  }

  for (; i < data.len; i += 1) {
    if (in_range(data.data[i], '0', '9' + 1)) {
      image->height *= 10;
      image->height += data.data[i] - '0';
    } else {
      break;
    }
  }

  for (; i < data.len; i += 1) {
    b8 stop = false;
    switch (data.data[i]) {
    case '\t':
    case '\n':
    case '\r':
    case ' ':
      break;
    default:
      stop = true;
    }
    if (stop) {
      break;
    }
  }

  if (!(image->width * image->height)) {
    return false;
  }

  if (image->width * image->height * 3 == data.len - i) {
    return false;
  }

  image->pixels.data = &data.data[i];
  image->pixels.len  = image->width * image->height * 3;

  return true;
}

internal u16 read_u16(Byte_Slice data, isize *cursor) {
  u16 value = (IDX(data, *cursor) << 8) + IDX(data, *cursor + 1);
  *cursor  += 2;
  return value;
}

extern b8 jpeg_load_bytes(Byte_Slice data, Image *image, Allocator allocator) {
  if (data.len < 2 + 2 + 2 + 2 + 5 + 2 + 1 + 2 + 2 + 1 + 1) {
    return false;
  }

  isize cursor = 0;

  if (IDX(data, cursor + 0) != 0xFF) {
    return false;
  }
  if (IDX(data, cursor + 1) != 0xD8) { // SOI
    return false;
  }
  cursor += 2;
  
  if (IDX(data, cursor + 0) != 0xFF) {
    return false;
  }
  if (IDX(data, cursor + 1) != 0xE0) { // APP0
    return false;
  }
  cursor += 2;
  
  isize app0_start = cursor;
  u16 app0_len = read_u16(data, &cursor);

  if (app0_len < 2 + 5 + 2 + 1 + 2 + 2 + 1 + 1) {
    return false;
  }

  if (!string_equal((String) { .data = (char *)data.data + cursor, .len = 5, }, LIT("JFIF\00"))) {
    return false;
  }
  cursor += 5;

  u16 app0_version = read_u16(data, &cursor);
  u8  app0_units   = IDX(data, cursor);
  cursor += 1;

  u16 app0_x_density = read_u16(data, &cursor);
  u16 app0_y_density = read_u16(data, &cursor);

  u8 app0_thumbnail_x = IDX(data, cursor);
  cursor += 1;

  u8 app0_thumbnail_y = IDX(data, cursor);
  cursor += 1;

  fmt_printflnc("version: 0x%x, units: %d, x: %d y: %d", app0_version, app0_units, app0_thumbnail_x, app0_thumbnail_y);

  cursor += app0_thumbnail_x * app0_thumbnail_y * 3;
  if (cursor != app0_start + app0_len) {
    return false;
  }

  while (cursor < data.len) {
    if (IDX(data, cursor) != 0xFF) {
      return false;
    }
    cursor += 1;

    u8 marker = IDX(data, cursor);
    cursor += 1;

    fmt_printflnc("next: %x", marker);
    // if (marker == 0xDB || marker == 0xC0) {
    // }
    cursor += read_u16(data, &cursor) - 2;
  }

  return true;
}

extern void image_clone_to_rgba8(Image const *in, Image *out, Allocator allocator) {
  assert(in->pixel_type == PT_u8);
  assert(in->components == 3);

  *out = *in;
  out->stride = 0;

  out->components = 4;
  out->pixel_type = PT_u8;

  slice_init(&out->pixels, in->width * in->height * 4, allocator);

  for_range(y, 0, in->height) {
    for_range(x, 0, in->width) {
      u8 b = in->pixels.data[(x + y * in->stride) * in->components + 0];
      u8 r = in->pixels.data[(x + y * in->stride) * in->components + 1];
      u8 g = in->pixels.data[(x + y * in->stride) * in->components + 2];

      ((u32 *)out->pixels.data)[x + y * out->width] = (0xFF << 24) | (r << 16) | (g << 8) | b;
    }
  }
}
