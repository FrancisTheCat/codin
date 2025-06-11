#pragma once

#include "codin.h"

typedef union {
  struct {
    f32 x;
    f32 y;
  };
  f32 data[2];
} TTF_Point;

typedef struct {
  TTF_Point a;
  TTF_Point b;
} TTF_Segment_Linear;

typedef struct {
  TTF_Point p0;
  TTF_Point p1;
  TTF_Point p2;
} TTF_Segment_Bezier;

typedef struct {
  TTF_Segment_Bezier *beziers;
  TTF_Segment_Linear *linears;
  u32                 n_beziers;
  u32                 n_linears;
  TTF_Point           min;
  TTF_Point           max;
} TTF_Glyph_Shape;

typedef struct {
  i32 height;
  i32 bearing;
} TTF_V_Metrics;

typedef struct {
  i32 advance;
  i32 bearing;
  i32 width;
} TTF_H_Metrics;

typedef struct {
  u32 start_char_code;
  u32 end_char_code;
  union {
    u32 start_glyph_id;
    i16 glyph_id_delta;
  };
} _TTF_Sequential_Map_Group;

typedef struct {
  u8                        *data;
  _TTF_Sequential_Map_Group *groups;
  u32                        n_groups;
  u32                        len;
  u32                        cmap;
  u32                        glyf;
  u32                        loca;
  u32                        n_hmetrics;
  u32                        n_vmetrics;
  u32                        hmtx;
  u32                        n_glyphs;
  u32                        units_per_em;
  i32                        line_height;
  i32                        font_height;
  bool                       bmp;
} TTF_Font;

extern bool ttf_load_bytes(u8 *data, i32 n, TTF_Font *font, Allocator allocator);
extern void ttf_destroy_font(TTF_Font const *font, Allocator allocator);

extern u32 ttf_get_codepoint_glyph(
  TTF_Font const *font,
  rune            codepoint
);

extern void ttf_get_glyph_shape(
  TTF_Font const  *font,
  u32              glyph,
  TTF_Glyph_Shape *shape
);
extern void ttf_get_codepoint_shape(
  TTF_Font const  *font,
  rune             codepoint,
  TTF_Glyph_Shape *shape
);

extern void ttf_render_codepoint_bitmap(
  TTF_Font        const *font,
  rune                   codepoint,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
);
extern void ttf_render_glyph_bitmap(
  TTF_Font        const *font,
  u32                    glyph,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
);
extern void ttf_render_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
);

extern void ttf_render_codepoint_sdf(
  TTF_Font  const *font,
  rune             codepoint,
  f32              font_size,
  f32              spread,
  u32              stride,
  u8              *pixels
);
extern void ttf_render_glyph_sdf(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  f32             spread,
  u32             stride,
  u8             *pixels
);
extern void ttf_render_shape_sdf(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  f32                    spread,
  u32                    stride,
  u8                    *pixels
);

extern void ttf_get_codepoint_bitmap_size(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  u32            *w,
  u32            *h
);
extern void ttf_get_glyph_bitmap_size(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  u32            *w,
  u32            *h
);
extern void ttf_get_shape_bitmap_size(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  u32                   *w,
  u32                   *h
);

extern void ttf_get_codepoint_bitmap(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  u32            *w,
  u32            *h,
  u8            **pixels,
  Allocator       allocator
);
extern void ttf_get_glyph_bitmap(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  u32            *w,
  u32            *h,
  u8            **pixels,
  Allocator       allocator
);
extern void ttf_get_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *glyph,
  f32                    font_size,
  u32                   *w,
  u32                   *h,
  u8                   **pixels,
  Allocator              allocator
);

extern void ttf_get_codepoint_h_metrics(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  TTF_H_Metrics  *metrics
);
extern void ttf_get_glyph_h_metrics(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  TTF_H_Metrics  *metrics
);
extern void ttf_get_codepoint_v_metrics(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  TTF_V_Metrics  *metrics
);
extern void ttf_get_glyph_v_metrics(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  TTF_V_Metrics  *metrics
);

extern f32 ttf_get_line_height(TTF_Font const *font, f32 font_size);
extern f32 ttf_get_font_height(TTF_Font const *font, f32 font_size);
