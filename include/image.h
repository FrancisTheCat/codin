#pragma once

#include "codin.h"
#include "io.h"

typedef enum {
  PNG_CT_Palette = 1,
  PNG_CT_Color   = 2,
  PNG_CT_Alpha   = 4,
} PNG_Color_Type;

typedef enum {
  PT_u8,
  PT_u16,
  PT_f32,
} Pixel_Type;

typedef struct {
  isize      width, height, stride, components;
  Byte_Slice pixels;
  u8         pixel_type;
} Image;

extern bool png_load_bytes(Byte_Slice data, Image *image, Allocator allocator);
extern bool png_save_writer(Writer const *writer, Image const *image);

extern bool ppm_save_writer(Writer const *writer, Image const *image);
extern bool ppm_load_bytes(Byte_Slice data, Image *image);

extern void image_clone_to_rgba8(Image const *in, Image *out, Allocator allocator);

extern bool stb_image_load_bytes(Byte_Slice data, Image *image, Allocator allocator);
