#include "codin/codin.h"

#include "codin/image.h"

typedef struct {
  char magic[4];
  u32  width, height;
  u8   channels;
  b8   linear;
  byte data[0];
} __attribute__((packed)) Qoi_Header;

typedef union {
  u32 u32;
  u8  u8[4];
} Pixel;

internal isize qoi_hash(Pixel p) {
  return ((isize)p.u8[0] * 3 + (isize)p.u8[1] * 5 + (isize)p.u8[2] * 7 + (isize)p.u8[3] * 11) % 64;
}

typedef enum {
  Qoi_Op_Rgb   = 0b11111110,
  Qoi_Op_Rgba  = 0b11111111,
  Qoi_Op_Index = 0b00,
  Qoi_Op_Diff  = 0b01,
  Qoi_Op_Luma  = 0b10,
  Qoi_Op_Run   = 0b11,
} Qoi_Op;

extern bool qoi_load_bytes(Byte_Slice data, Image *image, Allocator allocator) {
  if (data.len < size_of(Qoi_Header)) {
    return false;
  }
  Qoi_Header header;
  mem_copy(&header, data.data, size_of(Qoi_Header));

  if (
    header.magic[0] != 'q' || 
    header.magic[1] != 'o' || 
    header.magic[2] != 'i' || 
    header.magic[3] != 'f'
  ) {
    return false;
  }

  header.width  = endianness_swap(header.width);
  header.height = endianness_swap(header.height);

  if (header.channels < 3 || header.channels > 4) {
    return false;
  }

  *image = (Image) {
    .width      = header.width,
    .stride     = header.width,
    .height     = header.height,
    .components = header.channels,
    .pixel_type = PT_u8,
  };

  data = slice_start(data, size_of(Qoi_Header));

  slice_init(&image->pixels, header.width * header.height * header.channels, allocator);

  Pixel array[64]  = {0};
  Pixel last_pixel = { .u8 = {0, 0, 0, 255, }, };
  isize cursor = 0;
  while (cursor < header.width * header.height) {
    // there have to be at least 8 bytes at the end of the stream
    if (data.len < 9) {
      break;
    }
    isize advance;
    u8 opcode = IDX(data, 0);
    switch (opcode) {
    case Qoi_Op_Rgb: {
        for_range(i, 0, 3) {
          last_pixel.u8[i] = data.data[i + 1];
        }
        advance = 4;
      } break;
    case Qoi_Op_Rgba: {
        for_range(i, 0, 4) {
          last_pixel.u8[i] = data.data[i + 1];
        }
        advance = 5;
      } break;
    default: {
        switch (opcode >> 6) {
        case Qoi_Op_Index: {
            last_pixel.u32 = array[opcode & 0x3F].u32;
            advance = 1;
          } break;
        case Qoi_Op_Diff: {
            u8 o = opcode;
            for_range(i, 0, 3) {
              last_pixel.u8[2 - i] += (o & 0x3) - 2;
              o >>= 2;
            }
            advance = 1;
          } break;
        case Qoi_Op_Luma: {
            u8 rb = data.data[1];
            u8 diff_green = (opcode & 0x3F) - 32;
            u8 diff_red   = (rb >>  4) + diff_green - 8;
            u8 diff_blue  = (rb & 0xF) + diff_green - 8;

            last_pixel.u8[0] += diff_red;
            last_pixel.u8[1] += diff_green;
            last_pixel.u8[2] += diff_blue;

            advance = 2;
          } break;
        case Qoi_Op_Run: {
            u8 run = opcode & 0x3F;
            if (cursor + run >= header.width * header.height) {
              return false;
            }
            for_range(i, 0, run) {
              for_range(j, 0, header.channels) {
                image->pixels.data[(cursor + i) * header.channels + j] = last_pixel.u8[j];
              }
            }
            cursor += run;

            advance = 1;
          } break;
        }
      } break;
    }

    array[qoi_hash(last_pixel)] = last_pixel;

    data = slice_start(data, advance);

    for_range(i, 0, header.channels) {
      image->pixels.data[cursor * header.channels + i] = last_pixel.u8[i];
    }
    cursor += 1;
  }

  if (data.len != 8) {
    return false;
  }
  for_range(i, 0, 7) {
    if (data.data[i] != 0) {
      return false;
    }
  }

  return data.data[7] == 1;
}

extern bool qoi_save_buffer(Byte_Slice *buffer, Image const *image, Allocator allocator) {
  if (image->components < 3 || image->components > 4) {
    return false;
  }
  Qoi_Header header = {
    .magic    = "qoif",
    .width    = image->width,
    .height   = image->height,
    .channels = image->components,
    .linear   = false,
  };
  header.width  = endianness_swap(header.width);
  header.height = endianness_swap(header.height);

  isize max_size = image->width * image->height * 5 + size_of(header) + 8;
  slice_init(buffer, max_size, allocator);

  mem_copy(buffer->data, &header, sizeof(header));
  isize write_index = size_of(header);

  Pixel array[64]  = {0};
  Pixel last_pixel = { .u8 = {0, 0, 0, 255, }, };
  isize cursor     = 0;
  while (cursor < image->width * image->height) {
    Pixel p = { .u8 = {0, 0, 0, 255, }, };
    for_range(i, 0, image->components) {
      p.u8[i] = image->pixels.data[cursor * image->components + i];
    }

    if (p.u32 == last_pixel.u32) {
      array[qoi_hash(p)] = p;

      cursor += 1;
      isize n_repeating = 1;
      while (cursor < image->width * image->height && n_repeating < 62) {
        for_range(i, 0, image->components) {
          p.u8[i] = image->pixels.data[cursor * image->components + i];
        }
        if (p.u32 != last_pixel.u32) {
          break;
        }
        cursor      += 1;
        n_repeating += 1;
      }

      buffer->data[write_index] = (Qoi_Op_Run << 6) | (n_repeating - 1);
      write_index += 1;
      continue;
    }

    isize hash = qoi_hash(p);
    if (array[hash].u32 == p.u32) {
      buffer->data[write_index] = (Qoi_Op_Index << 6) | hash;
      write_index += 1;
      goto pixel_done;
    }
    array[hash] = p;

    if (header.channels == 3 || p.u8[3] == last_pixel.u8[3]) {
  		i8 vr = p.u8[0] - last_pixel.u8[0];
  		i8 vg = p.u8[1] - last_pixel.u8[1];
  		i8 vb = p.u8[2] - last_pixel.u8[2];

  		i8 vg_r = vr - vg;
  		i8 vg_b = vb - vg;

  		if (
  			vr > -3 && vr < 2 &&
  			vg > -3 && vg < 2 &&
  			vb > -3 && vb < 2
  		) {
        buffer->data[write_index] = (Qoi_Op_Diff << 6) | ((vr + 2) << 4) | ((vg + 2) << 2) | (vb + 2);
        write_index += 1;
  			goto pixel_done;
  		}
  		if (
  			vg_r >  -9 && vg_r <  8 &&
  			vg   > -33 && vg   < 32 &&
  			vg_b >  -9 && vg_b <  8
  		) {
        buffer->data[write_index] = (Qoi_Op_Luma << 6) | (vg   + 32);
        write_index += 1;
        buffer->data[write_index] = ((vg_r + 8)  << 4) | (vg_b +  8);
        write_index += 1;
  			goto pixel_done;
  		}
    }

    if (header.channels == 3 || last_pixel.u32 >> 24 == p.u32 >> 24) {
      buffer->data[write_index] = Qoi_Op_Rgb;
      write_index += 1;
      mem_copy(&buffer->data[write_index], p.u8, 3);
      write_index += 3;
    } else {
      buffer->data[write_index] = Qoi_Op_Rgba;
      write_index += 1;
      mem_copy(&buffer->data[write_index], p.u8, 4);
      write_index += 4;
    }
pixel_done:
    last_pixel = p;
    cursor    += 1;
  }

  byte end_marker[8] = { 0, 0, 0, 0, 0, 0, 0, 1, };
  mem_copy(&buffer->data[write_index], end_marker, count_of(end_marker));
  write_index += 8;

  buffer->len = write_index;

  return true;
}

extern bool qoi_save_writer(Writer const *writer, Image const *image) {
  Byte_Slice data;
  bool ok = qoi_save_buffer(&data, image, context.temp_allocator);
  if (!ok) {
    return false;
  }
  return write_bytes(writer, data).ok;
}

