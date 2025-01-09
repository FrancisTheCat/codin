#ifndef __TTF_INCLUDE__
#define __TTF_INCLUDE__

#ifdef TTF_STATIC
#define TTF_DEF static
#else
#define TTF_DEF extern
#endif

typedef unsigned char  ttf_u8;
typedef unsigned short ttf_u16;
typedef unsigned int   ttf_u32;
typedef unsigned long  ttf_u64;
typedef unsigned long  ttf_uintptr;

typedef   signed char  ttf_i8;
typedef   signed short ttf_i16;
typedef   signed int   ttf_i32;
typedef   signed long  ttf_i64;

typedef ttf_i8 ttf_bool;

#define ttf_true  1
#define ttf_false 0

#define TTF_NULL 0

typedef float  ttf_f32;
typedef double ttf_f64;

typedef union {
  struct {
    ttf_f32 x;
    ttf_f32 y;
  };
  ttf_f32 data[2];
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
  ttf_u32             n_beziers;
  ttf_u32             n_linears;
  TTF_Point           min;
  TTF_Point           max;
} TTF_Glyph_Shape;

typedef struct {
  ttf_i32 height;
  ttf_i32 bearing;
} TTF_V_Metrics;

typedef struct {
  ttf_i32 advance;
  ttf_i32 bearing;
  ttf_i32 width;
} TTF_H_Metrics;

typedef struct {
  ttf_u32 start_char_code;
  ttf_u32 end_char_code;
  union {
    ttf_u32 start_glyph_id;
    ttf_i16 glyph_id_delta;
  };
} _TTF_Sequential_Map_Group;

typedef struct {
  ttf_u8                    *data;
  _TTF_Sequential_Map_Group *groups;
  ttf_u32                    n_groups;
  ttf_u32                    len;
  ttf_u32                    cmap;
  ttf_u32                    glyf;
  ttf_u32                    loca;
  ttf_u32                    n_hmetrics;
  ttf_u32                    n_vmetrics;
  ttf_u32                    hmtx;
  ttf_u32                    n_glyphs;
  ttf_u32                    units_per_em;
  ttf_i32                    line_height;
  ttf_i32                    font_height;
  ttf_bool                   bmp;
  void                      *allocator;
} TTF_Font;

TTF_DEF ttf_bool ttf_load_bytes(ttf_u8 *data, ttf_i32 n, TTF_Font *font, void *allocator);
TTF_DEF void     ttf_destroy_font(TTF_Font const *font);

TTF_DEF ttf_u32 ttf_get_codepoint_glyph(
  TTF_Font const *font,
  ttf_u32        codepoint
);

TTF_DEF void ttf_get_glyph_shape(
  TTF_Font const  *font,
  ttf_u32          glyph,
  TTF_Glyph_Shape *shape
);
TTF_DEF void ttf_get_codepoint_shape(
  TTF_Font const  *font,
  ttf_u32          codepoint,
  TTF_Glyph_Shape *shape
);

TTF_DEF void ttf_render_codepoint_bitmap(
  TTF_Font        const *font,
  ttf_u32                codepoint,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
);
TTF_DEF void ttf_render_glyph_bitmap(
  TTF_Font        const *font,
  ttf_u32                glyph,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
);
TTF_DEF void ttf_render_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
);

TTF_DEF void ttf_render_codepoint_sdf(
  TTF_Font        const *font,
  ttf_u32                codepoint,
  ttf_f32                font_size,
  ttf_f32                spread,
  ttf_u32                stride,
  ttf_u8                *pixels
);
TTF_DEF void ttf_render_glyph_sdf(
  TTF_Font        const *font,
  ttf_u32                glyph,
  ttf_f32                font_size,
  ttf_f32                spread,
  ttf_u32                stride,
  ttf_u8                *pixels
);
TTF_DEF void ttf_render_shape_sdf(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_f32                spread,
  ttf_u32                stride,
  ttf_u8                *pixels
);

TTF_DEF void ttf_get_codepoint_bitmap_size(
  TTF_Font        const *font,
  ttf_u32                codepoint,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
);
TTF_DEF void ttf_get_glyph_bitmap_size(
  TTF_Font        const *font,
  ttf_u32                glyph,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
);
TTF_DEF void ttf_get_shape_bitmap_size(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
);

TTF_DEF void ttf_get_codepoint_bitmap(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  ttf_u32        *w,
  ttf_u32        *h,
  ttf_u8        **pixels
);
TTF_DEF void ttf_get_glyph_bitmap(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         font_size,
  ttf_u32        *w,
  ttf_u32        *h,
  ttf_u8        **pixels
);
TTF_DEF void ttf_get_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *glyph,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h,
  ttf_u8               **pixels
);

TTF_DEF void ttf_get_codepoint_h_metrics(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  TTF_H_Metrics   *metrics
);
TTF_DEF void ttf_get_glyph_h_metrics(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         font_size,
  TTF_H_Metrics   *metrics
);
TTF_DEF void ttf_get_codepoint_v_metrics(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  TTF_V_Metrics   *metrics
);
TTF_DEF void ttf_get_glyph_v_metrics(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         font_size,
  TTF_V_Metrics   *metrics
);

TTF_DEF ttf_f32 ttf_get_line_height(TTF_Font const *font, ttf_f32 font_size);
TTF_DEF ttf_f32 ttf_get_font_height(TTF_Font const *font, ttf_f32 font_size);

#ifdef TTF_IMPLEMENTATION

typedef struct {
  ttf_u16 version;
  ttf_u16 num_tables;
} _TTF_Cmap_Header;

typedef struct {
  ttf_u16 platform_id;
  ttf_u16 encoding_id;
  ttf_u32 offset;
} _TTF_Cmap_Encoding_Record;

typedef struct {
  ttf_u16 format;
  ttf_u16 _reserved;
  ttf_u32 length;
  ttf_u32 language;
  ttf_u32 num_groups;
} _TTF_Cmap_Subtable_Format_12;

typedef struct {
  ttf_u16 format;
  ttf_u16 length;
  ttf_u16 language;
  ttf_u16 segCountX2;
  ttf_u16 _searchRange;
  ttf_u16 _entrySelector;
  ttf_u16 _rangeShift;
} _TTF_Cmap_Subtable_Format_4;

typedef struct {
  ttf_u8  tableTag[4];
  ttf_u32 checksum;
  ttf_u32 offset;
  ttf_u32 length;
} _TTF_Table_Record;

typedef struct {
  ttf_i16	numberOfContours;
  ttf_i16	xMin;
  ttf_i16	yMin;
  ttf_i16	xMax;
  ttf_i16	yMax;
} _TTF_Glyph_Header;

typedef struct {
  ttf_u16 version[2];
  ttf_u16 numGlyphs;
} _TTF_Maxp_Table;

typedef struct {
  ttf_u16 majorVersion;
  ttf_u16 minorVersion;
  ttf_u32 fontRevision;
  ttf_u32 checksumAdjustment;
  ttf_u32 magicNumber;
  ttf_u16 flags;
  ttf_u16 unitsPerEm;
  ttf_u64 created;
  ttf_u64 modified;
  ttf_i16 xMin;
  ttf_i16 yMin;
  ttf_i16 xMax;
  ttf_i16 yMax;
  ttf_u16 macStyle;
  ttf_u16 lowestRecPPEM;
  ttf_i16 fontDirectionHint;
  ttf_i16 indexToLocFormat;
  ttf_i16 glyphDataFormat;
} _TTF_Font_Header_Table;

typedef struct {
  ttf_u32 sfntVesion;
  ttf_u16 numTables;
  ttf_u16 searchRange;
  ttf_u16 entrySelector;
  ttf_u16 rangeShift;
} _TTF_File_Header;

typedef struct {
  ttf_u16 majorVersion;
  ttf_u16 minorVersion;
  ttf_i16 ascender;
  ttf_i16 descender;
  ttf_i16 lineGap;
  ttf_u16 advanceWidthMax;
  ttf_i16 minLeftSideBearing;
  ttf_i16 minRightSideBearing;
  ttf_i16 xMaxExtent;
  ttf_i16 caretSlopeRise;
  ttf_i16 caretSlopeRun;
  ttf_i16 caretOffset;
  ttf_i16 _reserved[4];
  ttf_i16 metricDataFormat;
  ttf_u16 numberOfHMetrics;
} _TTF_Hhea_Table;

typedef union {
  ttf_u16 version;
  struct {
    ttf_u16 version;
    ttf_i16 xAvgCharWidth;
    ttf_u16 usWeightClass;
    ttf_u16 usWidthClass;
    ttf_u16 fsType;
    ttf_i16 ySubscriptXSize;
    ttf_i16 ySubscriptYSize;
    ttf_i16 ySubscriptXOffset;
    ttf_i16 ySubscriptYOffset;
    ttf_i16 ySuperscriptXSize;
    ttf_i16 ySuperscriptYSize;
    ttf_i16 ySuperscriptXOffset;
    ttf_i16 ySuperscriptYOffset;
    ttf_i16 yStrikeoutSize;
    ttf_i16 yStrikeoutPosition;
    ttf_i16 sFamilyClass;
    ttf_u8  panose[10];
    ttf_u32 ulUnicodeRange1;
    ttf_u32 ulUnicodeRange2;
    ttf_u32 ulUnicodeRange3;
    ttf_u32 ulUnicodeRange4;
    char    achVendID[4];
    ttf_u16 fsSelection;
    ttf_u16 usFirstCharIndex;
    ttf_u16 usLastCharIndex;
    ttf_i16 sTypoAscender;
    ttf_i16 sTypoDescender;
    ttf_i16 sTypoLineGap;
    ttf_u16 usWinAscent;
    ttf_u16 usWinDescent;
  } v0;
  struct {
    ttf_u16 version;
    ttf_i16 xAvgCharWidth;
    ttf_u16 usWeightClass;
    ttf_u16 usWidthClass;
    ttf_u16 fsType;
    ttf_i16 ySubscriptXSize;
    ttf_i16 ySubscriptYSize;
    ttf_i16 ySubscriptXOffset;
    ttf_i16 ySubscriptYOffset;
    ttf_i16 ySuperscriptXSize;
    ttf_i16 ySuperscriptYSize;
    ttf_i16 ySuperscriptXOffset;
    ttf_i16 ySuperscriptYOffset;
    ttf_i16 yStrikeoutSize;
    ttf_i16 yStrikeoutPosition;
    ttf_i16 sFamilyClass;
    ttf_u8  panose[10];
    ttf_u32 ulUnicodeRange1;
    ttf_u32 ulUnicodeRange2;
    ttf_u32 ulUnicodeRange3;
    ttf_u32 ulUnicodeRange4;
    char    achVendID[4];
    ttf_u16 fsSelection;
    ttf_u16 usFirstCharIndex;
    ttf_u16 usLastCharIndex;
    ttf_i16 sTypoAscender;
    ttf_i16 sTypoDescender;
    ttf_i16 sTypoLineGap;
    ttf_u16 usWinAscent;
    ttf_u16 usWinDescent;
    ttf_u32 ulCodePageRange1;
    ttf_u32 ulCodePageRange2;
  } v1;
  struct {
    ttf_u16 version;
    ttf_i16 xAvgCharWidth;
    ttf_u16 usWeightClass;
    ttf_u16 usWidthClass;
    ttf_u16 fsType;
    ttf_i16 ySubscriptXSize;
    ttf_i16 ySubscriptYSize;
    ttf_i16 ySubscriptXOffset;
    ttf_i16 ySubscriptYOffset;
    ttf_i16 ySuperscriptXSize;
    ttf_i16 ySuperscriptYSize;
    ttf_i16 ySuperscriptXOffset;
    ttf_i16 ySuperscriptYOffset;
    ttf_i16 yStrikeoutSize;
    ttf_i16 yStrikeoutPosition;
    ttf_i16 sFamilyClass;
    ttf_u8  panose[10];
    ttf_u32 ulUnicodeRange1;
    ttf_u32 ulUnicodeRange2;
    ttf_u32 ulUnicodeRange3;
    ttf_u32 ulUnicodeRange4;
    char    achVendID[4];
    ttf_u16 fsSelection;
    ttf_u16 usFirstCharIndex;
    ttf_u16 usLastCharIndex;
    ttf_i16 sTypoAscender;
    ttf_i16 sTypoDescender;
    ttf_i16 sTypoLineGap;
    ttf_u16 usWinAscent;
    ttf_u16 usWinDescent;
    ttf_u32 ulCodePageRange1;
    ttf_u32 ulCodePageRange2;
    ttf_i16 sxHeight;
    ttf_i16 sCapHeight;
    ttf_u16 usDefaultChar;
    ttf_u16 usBreakChar;
    ttf_u16	usMaxContext;
  } v4;
  struct {
    ttf_u16 version;
    ttf_i16 xAvgCharWidth;
    ttf_u16 usWeightClass;
    ttf_u16 usWidthClass;
    ttf_u16 fsType;
    ttf_i16 ySubscriptXSize;
    ttf_i16 ySubscriptYSize;
    ttf_i16 ySubscriptXOffset;
    ttf_i16 ySubscriptYOffset;
    ttf_i16 ySuperscriptXSize;
    ttf_i16 ySuperscriptYSize;
    ttf_i16 ySuperscriptXOffset;
    ttf_i16 ySuperscriptYOffset;
    ttf_i16 yStrikeoutSize;
    ttf_i16 yStrikeoutPosition;
    ttf_i16 sFamilyClass;
    ttf_u8  panose[10];
    ttf_u32 ulUnicodeRange1;
    ttf_u32 ulUnicodeRange2;
    ttf_u32 ulUnicodeRange3;
    ttf_u32 ulUnicodeRange4;
    char    achVendID[4];
    ttf_u16 fsSelection;
    ttf_u16 usFirstCharIndex;
    ttf_u16 usLastCharIndex;
    ttf_i16 sTypoAscender;
    ttf_i16 sTypoDescender;
    ttf_i16 sTypoLineGap;
    ttf_u16 usWinAscent;
    ttf_u16 usWinDescent;
    ttf_u32 ulCodePageRange1;
    ttf_u32 ulCodePageRange2;
    ttf_i16 sxHeight;
    ttf_i16 sCapHeight;
    ttf_u16 usDefaultChar;
    ttf_u16 usBreakChar;
    ttf_u16 usMaxContext;
    ttf_u16 usLowerOpticalPointSize;
    ttf_u16 usUpperOpticalPointSize;
  } v5;
} _TTF_OS2_Header;

#define TTF_INTERNAL static

#ifndef ttf_alloc
#include <stdlib.h>
#define ttf_alloc(U, size) malloc
#define ttf_free(U, ptr)  free
#else
#ifndef ttf_free
typedef char ttf_alloc_defined_but_ttf_free_is_not[-1];
#endif
#endif

#ifndef ttf_sort_f32s
#include <stdlib.h>
#define ttf_sort_f32s(ptr, n) qsort(ptr, n, sizeof(ttf_f32), _ttf_compare_ttf_f32s)

TTF_INTERNAL ttf_i32 _ttf_compare_ttf_f32s(const void *a, const void *b) {
  ttf_f32 fa = *(ttf_f32 *)a;
  ttf_f32 fb = *(ttf_f32 *)b;

  return fa < fb;
}
#endif

#ifndef ttf_memcpy
#include <string.h>
#define ttf_memcpy memcpy
#endif

#ifndef ttf_sqrtf
#include <math.h>
#define ttf_sqrtf sqrtf
#endif

#ifndef ttf_absf
#include <math.h>
#define ttf_absf fabs
#endif

#define ttf_be16toh(x) __builtin_bswap16(x)
#define ttf_be32toh(x) __builtin_bswap32(x)

TTF_INTERNAL void _ttf_convert_cmap_subtable_format_4(_TTF_Cmap_Subtable_Format_4 *f) {
  f->format         = ttf_be16toh(f->format);
  f->length         = ttf_be16toh(f->length);
  f->language       = ttf_be16toh(f->language);
  f->segCountX2     = ttf_be16toh(f->segCountX2);
  f->_searchRange   = ttf_be16toh(f->_searchRange);
  f->_entrySelector = ttf_be16toh(f->_entrySelector);
  f->_rangeShift    = ttf_be16toh(f->_rangeShift);
}

TTF_INTERNAL void _ttf_convert_cmap_subtable_format_12(_TTF_Cmap_Subtable_Format_12 *f) {
  f->format     = ttf_be16toh(f->format);
  f->length     = ttf_be32toh(f->length);
  f->language   = ttf_be32toh(f->language);
  f->num_groups = ttf_be32toh(f->num_groups);
}

TTF_INTERNAL void _ttf_convert_file_header(_TTF_File_Header *h) {
  h->sfntVesion    = ttf_be32toh(h->sfntVesion);
  h->numTables     = ttf_be16toh(h->numTables);
  h->searchRange   = ttf_be16toh(h->searchRange);
  h->entrySelector = ttf_be16toh(h->entrySelector);
  h->rangeShift    = ttf_be16toh(h->rangeShift);
}

TTF_INTERNAL void _ttf_convert_table_record(_TTF_Table_Record *r) {
  r->checksum = ttf_be32toh(r->checksum);
  r->offset   = ttf_be32toh(r->offset);
  r->length   = ttf_be32toh(r->length);
}

TTF_INTERNAL void _ttf_convert_glyph_header(_TTF_Glyph_Header *h) {
  h->numberOfContours = ttf_be16toh(h->numberOfContours);
  h->xMin             = ttf_be16toh(h->xMin);
  h->yMin             = ttf_be16toh(h->yMin);
  h->xMax             = ttf_be16toh(h->xMax);
  h->yMax             = ttf_be16toh(h->yMax);
}

TTF_INTERNAL void _ttf_get_glyph_header(TTF_Font const *font, ttf_u32 glyph, _TTF_Glyph_Header *header) {
  ttf_u32 offset;
  ttf_u32 next;

  #if 1
    offset = ((ttf_u32 *)&font->data[font->loca])[glyph];
    next   = ((ttf_u32 *)&font->data[font->loca])[glyph + 1];
  #else
    offset = ((ttf_u16 *)&font->data[font->loca])[glyph];
    next   = ((ttf_u16 *)&font->data[font->loca])[glyph + 1];
  #endif

  offset = ttf_be32toh(offset);
  next   = ttf_be32toh(next);

  if (next - offset < sizeof(_TTF_Glyph_Header)) {
    return;
  }

  ttf_memcpy(header, &font->data[font->glyf + offset], sizeof(*header));
  _ttf_convert_glyph_header(header);
}

#define ttf_min(a, b) ((a) > (b) ? (b) : (a))
#define ttf_max(a, b) ((a) < (b) ? (b) : (a))

TTF_DEF ttf_f32 ttf_get_line_height(TTF_Font const *font, ttf_f32 font_size) {
  return 2 * font_size * font->line_height / font->units_per_em;
}

TTF_DEF ttf_f32 ttf_get_font_height(TTF_Font const *font, ttf_f32 font_size) {
  return font_size * font->font_height / font->units_per_em;
}

TTF_DEF void ttf_get_codepoint_h_metrics(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  TTF_H_Metrics  *metrics
) {
  ttf_get_glyph_h_metrics(font, ttf_get_codepoint_glyph(font, codepoint), font_size, metrics);
}

TTF_DEF void ttf_get_glyph_h_metrics(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         font_size,
  TTF_H_Metrics   *metrics
) {
  typedef struct {
    ttf_u16 advance;
    ttf_i16 bearing;
  } TTF_Long_Hor_Metric_Record;
  TTF_Long_Hor_Metric_Record *records = (TTF_Long_Hor_Metric_Record *)&font->data[font->hmtx];
  if (glyph >= font->n_hmetrics) {
    metrics->advance = font_size * ttf_be16toh(             records[font->n_hmetrics - 1].advance                   ) / font->units_per_em;
    metrics->bearing = font_size * ttf_be16toh(((ttf_i16 *)&records[font->n_hmetrics    ])[glyph - font->n_hmetrics]) / font->units_per_em;
    return;
  }
  metrics->advance = font_size *          ttf_be16toh(records[glyph].advance) / font->units_per_em;
  metrics->bearing = font_size * (ttf_i16)ttf_be16toh(records[glyph].bearing) / font->units_per_em;
}

TTF_DEF void ttf_get_codepoint_v_metrics(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  TTF_V_Metrics  *metrics
) {
  ttf_get_glyph_v_metrics(font, ttf_get_codepoint_glyph(font, codepoint), font_size, metrics);
}

TTF_DEF void ttf_get_glyph_v_metrics(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         font_size,
  TTF_V_Metrics  *metrics
) {
  _TTF_Glyph_Header gh;
  _ttf_get_glyph_header(font, glyph, &gh);
  metrics->bearing = -font_size * (           gh.yMin) / font->units_per_em;
  metrics->height  =  font_size * ( gh.yMax - gh.yMin) / font->units_per_em;
}

TTF_DEF ttf_bool ttf_load_bytes(ttf_u8 *data, ttf_i32 n, TTF_Font *font, void *allocator) {
  *font = (TTF_Font) {0};

  if ((ttf_uintptr)data & 3) {
    return ttf_false;
  }
  if (n < sizeof(_TTF_File_Header)) {
    return ttf_false;
  }
  _TTF_File_Header header;
  ttf_memcpy(&header, data, sizeof(header));
  _ttf_convert_file_header(&header);

  font->data = data;
  font->len  = n;

  font->allocator = allocator;

  if (sizeof(_TTF_File_Header) + sizeof(_TTF_Table_Record) * header.numTables > n) {
    return ttf_false;
  }

  _TTF_Table_Record *table_records = (_TTF_Table_Record *)&data[sizeof(_TTF_File_Header)];
  
  font->n_glyphs = -1;
  font->loca     = -1;
  font->glyf     = -1;
  font->cmap     = -1;

  for (ttf_i32 table_index = 0; table_index < header.numTables; table_index += 1) {
    _TTF_Table_Record table;
    ttf_memcpy(&table, &table_index[table_records], sizeof(table));
    _ttf_convert_table_record(&table);

    #define CHECK_TAG(str) (*(ttf_u32 *)table.tableTag == *(ttf_u32 *)str)

    if (CHECK_TAG("maxp")) {
      _TTF_Maxp_Table maxp;
      ttf_memcpy(&maxp, &data[table.offset], sizeof(maxp));
      font->n_glyphs = ttf_be16toh(maxp.numGlyphs);
    }
    if (CHECK_TAG("head")) {
      _TTF_Font_Header_Table head;
      ttf_memcpy(&head, &data[table.offset], sizeof(head));
      font->units_per_em = ttf_be16toh(head.unitsPerEm);
    }
    if (CHECK_TAG("hhea")) {
      _TTF_Hhea_Table hhea;
      ttf_memcpy(&hhea, &data[table.offset], sizeof(hhea));
      font->n_hmetrics  = ttf_be16toh(hhea.numberOfHMetrics);
      font->line_height = (ttf_i16)ttf_be16toh(hhea.lineGap) + (ttf_i16)ttf_be16toh(hhea.ascender) + (ttf_i16)ttf_be16toh(hhea.descender);
    }
    if (CHECK_TAG("loca")) {
      font->loca = table.offset;
    }
    if (CHECK_TAG("glyf")) {
      font->glyf = table.offset;
    }
    if (CHECK_TAG("cmap")) {
      font->cmap = table.offset;
    }
    if (CHECK_TAG("hmtx")) {
      font->hmtx = table.offset;
    }
    if (CHECK_TAG("OS/2")) {
      _TTF_OS2_Header os2;
      ttf_memcpy(&os2, &data[table.offset], ttf_min(sizeof(os2), n - table.offset));
      switch (ttf_be16toh(os2.version)) {
      case 0:
        return false;
        break;

      case 1:
      case 2:
      case 3:
        return false;
        break;

      case 4:
        font->font_height = ttf_be16toh(os2.v4.sCapHeight);
        break;

      case 5:
        font->font_height = ttf_be16toh(os2.v5.sCapHeight);
        break;
      }
    }
  }

  if (
    font->n_glyphs == -1 ||
    font->loca     == -1 ||
    font->glyf     == -1 ||
    font->cmap     == -1
  ) {
    return ttf_false;
  }

  _TTF_Cmap_Header cmap_header;
  ttf_memcpy(&cmap_header, &data[font->cmap], sizeof(cmap_header));
  cmap_header.num_tables = ttf_be16toh(cmap_header.num_tables);
  cmap_header.version    = ttf_be16toh(cmap_header.version);

  font->groups   = TTF_NULL;
  font->n_groups = -1;

  ttf_bool full_unicode = ttf_false;

  for (ttf_i32 cmap_index = 0; cmap_index < cmap_header.num_tables; cmap_index += 1) {
    _TTF_Cmap_Encoding_Record record;
    ttf_memcpy(&record, &data[font->cmap + sizeof(cmap_header) + cmap_index * sizeof(record)], sizeof(record));

    record.offset      = ttf_be32toh(record.offset);
    record.encoding_id = ttf_be16toh(record.encoding_id);
    record.platform_id = ttf_be16toh(record.platform_id);

    if (record.platform_id == 0 && record.encoding_id == 4) {
      full_unicode = ttf_true;
    }
  }

  for (ttf_i32 cmap_index = 0; cmap_index < cmap_header.num_tables; cmap_index += 1) {
    _TTF_Cmap_Encoding_Record record;
    ttf_memcpy(&record, &data[font->cmap + sizeof(cmap_header) + cmap_index * sizeof(record)], sizeof(record));

    record.offset      = ttf_be32toh(record.offset);
    record.encoding_id = ttf_be16toh(record.encoding_id);
    record.platform_id = ttf_be16toh(record.platform_id);

    if (record.platform_id == 0) {
      switch (record.encoding_id) {
      case 3:
        if (!full_unicode) {
          if (ttf_be16toh(*(ttf_u16 *)&data[font->cmap + record.offset]) != 4) {
            break;
          }
          _TTF_Cmap_Subtable_Format_4 subtable;
          ttf_memcpy(&subtable, &data[font->cmap + record.offset], sizeof(subtable));
          _ttf_convert_cmap_subtable_format_4(&subtable);

          ttf_u16 segment_count = subtable.segCountX2 / 2;
          _TTF_Sequential_Map_Group *segments =
            (_TTF_Sequential_Map_Group *)ttf_alloc(font->allocator, sizeof(_TTF_Sequential_Map_Group) * segment_count);

          ttf_u16 *p = (ttf_u16 *)&data[font->cmap + record.offset + sizeof(subtable)];

          for (ttf_i32 i = 0; i < segment_count; i += 1) {
            segments[i].end_char_code = ttf_be16toh(*p);
            p += 1;
          }

          // padding
          if (*p != 0) {
            break;
          }
          p += 1;

          for (ttf_i32 i = 0; i < segment_count; i += 1) {
            segments[i].start_char_code = ttf_be16toh(*p);
            p += 1;
          }

          for (ttf_i32 i = 0; i < segment_count; i += 1) {
            segments[i].glyph_id_delta = ttf_be16toh(*(ttf_i16 *)p);
            p += 1;
          }

          for (ttf_i32 i = 0; i < segment_count; i += 1) {
            if (*p) {
              return ttf_false;
            }
            p += 1;
          }

          font->groups   = segments;
          font->n_groups = segment_count;
          font->bmp      = ttf_true;

          goto end_cmap;
        }
        break;
      case 4:
        {
          if (ttf_be16toh(*(ttf_u16 *)&data[font->cmap + record.offset]) != 12) {
            break;
          }
          _TTF_Cmap_Subtable_Format_12 subtable;
          ttf_memcpy(&subtable, &data[font->cmap + record.offset], sizeof(subtable));
          _ttf_convert_cmap_subtable_format_12(&subtable);

          _TTF_Sequential_Map_Group *segments =
            (_TTF_Sequential_Map_Group *)ttf_alloc(font->allocator, sizeof(_TTF_Sequential_Map_Group) * subtable.num_groups);

          if (!segments) {
            return ttf_false;
          }

          font->groups   = segments;
          font->n_groups = subtable.num_groups;

          _TTF_Sequential_Map_Group *groups   = (_TTF_Sequential_Map_Group *)&data[font->cmap + record.offset + sizeof(_TTF_Cmap_Subtable_Format_12)];

          for (ttf_i32 i = 0; i < font->n_groups; i += 1) {
            _TTF_Sequential_Map_Group group = groups[i];
            font->groups[i].start_char_code = ttf_be32toh(group.start_char_code);
            font->groups[i].end_char_code   = ttf_be32toh(group.end_char_code);
            font->groups[i].start_glyph_id  = ttf_be32toh(group.start_glyph_id);
          }

          font->bmp = ttf_false;
          goto end_cmap;
        }
        break;
      }
    }
  }
  end_cmap:

  if (font->n_groups == -1 || font->groups == TTF_NULL) {
    return ttf_false;
  }

  if (!font->font_height) {
    _TTF_Glyph_Header gh;
    _ttf_get_glyph_header(font, 0, &gh);
    font->font_height = gh.yMax - gh.yMin;
  }

  return ttf_true;
}

TTF_DEF void ttf_destroy_font(TTF_Font const *font) {
  ttf_free(font->allocator, font->groups);
}

TTF_DEF ttf_bool _ttf_bezier_curve_intersection(
  TTF_Segment_Bezier bezier,
  ttf_f32              y,
  ttf_f32              dy
) {
  // "U"
  if (bezier.p1.y < bezier.p0.y) {
    if (dy < 0) {
      if (bezier.p0.y > y) {
        return ttf_true;
      }
    } else {
      if (bezier.p2.y > y) {
        return ttf_true;
      }
    }
  // "^"
  } else if (bezier.p1.y >= bezier.p2.y) {
    if (dy > 0) {
      if (bezier.p0.y <= y) {
        return ttf_true;
      }
    } else {
      if (bezier.p2.y <= y) {
        return ttf_true;
      }
    }
  // "/"
  } else {
    if (
      bezier.p2.y >  y &&
      bezier.p0.y <= y
    ) {
      return ttf_true;
    }
  }

  return ttf_false;
}

TTF_DEF ttf_u32 ttf_get_codepoint_glyph(
  TTF_Font const *font,
  ttf_u32        codepoint
) {
  ttf_u32 glyph_id = 0;
  if (font->bmp) {
    for (ttf_i32 i = 0; i < font->n_groups; i += 1) {
      _TTF_Sequential_Map_Group group = font->groups[i];
      if (group.start_char_code <= codepoint && codepoint <= group.end_char_code) {
        glyph_id = codepoint + group.glyph_id_delta;
        break;
      }
    }
  } else {
    for (ttf_i32 i = 0; i < font->n_groups; i += 1) {
      _TTF_Sequential_Map_Group group = font->groups[i];
      if (group.start_char_code <= codepoint && codepoint <= group.end_char_code) {
        glyph_id = group.start_glyph_id + codepoint - group.start_char_code;
        break;
      }
    }
  }
  return glyph_id;
}

TTF_DEF void ttf_get_codepoint_shape(
  TTF_Font const  *font,
  ttf_u32          codepoint,
  TTF_Glyph_Shape *shape
) {
  ttf_get_glyph_shape(font, ttf_get_codepoint_glyph(font, codepoint), shape);
}

TTF_DEF void ttf_get_glyph_shape(
  TTF_Font const  *font,
  ttf_u32          glyph,
  TTF_Glyph_Shape *shape
) {
  ttf_u32 offset;
  ttf_u32 next;

  #if 1
    offset = ((ttf_u32 *)&font->data[font->loca])[glyph];
    next   = ((ttf_u32 *)&font->data[font->loca])[glyph + 1];
  #else
    offset = ((ttf_u16 *)&font->data[font->loca])[glyph];
    next   = ((ttf_u16 *)&font->data[font->loca])[glyph + 1];
  #endif

  offset = ttf_be32toh(offset);
  next   = ttf_be32toh(next);

  if (next - offset < sizeof(_TTF_Glyph_Header)) {
    return;
  }

  _TTF_Glyph_Header gh;
  ttf_memcpy(&gh, &font->data[font->glyf + offset], sizeof(gh));
  _ttf_convert_glyph_header(&gh);

  shape->min.x = gh.xMin;
  shape->min.y = gh.yMin;
  shape->max.x = gh.xMax;
  shape->max.y = gh.yMax;

  if (gh.numberOfContours >= 0) {
    ttf_u8  *description        = &font->data[font->glyf + offset + sizeof(gh)];
    ttf_u16  instruction_length = ttf_be16toh(((ttf_u16 *)&font->data[font->glyf + offset + sizeof(gh)])[gh.numberOfContours]);

    ttf_i16 *contours = (ttf_i16 *)description;

    ttf_i32  n_points = ttf_be16toh(contours[gh.numberOfContours - 1]) + 1;
    ttf_u8  *points   = &description[gh.numberOfContours * 2 + sizeof(ttf_u16) + instruction_length];

    ttf_u8    flags [n_points];
    TTF_Point coords[n_points];

    {
      typedef enum {
        ON_CURVE_POINT                       = 0x01,
        X_SHORT_VECTOR                       = 0x02,
        Y_SHORT_VECTOR                       = 0x04,
        REPEAT_FLAG                          = 0x08,
        X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
        Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
        OVERLAP_SIMPLE                       = 0x40,
        Reserved                             = 0x80,
      } TTF_Simple_Glyph_Flag;

      ttf_u8 flag;
      for (ttf_i32 i = 0; i < n_points; i += 1) {
        flag = *points++;
        if (flag & REPEAT_FLAG) {
          ttf_i32 repeat_count = *points++;
          for (ttf_i32 j = 0; j < repeat_count + 1; j += 1) {
            flags[i + j] = flag;
          }
          i += repeat_count;
        } else {
          flags[i] = flag;
        }
      }

      ttf_i32 x = 0;
      for (ttf_i32 i = 0; i < n_points; i += 1) {
        flag = flags[i];
        if (flag & X_SHORT_VECTOR) {
          ttf_i16 dx = *points++;
          x += (flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) ? dx : -dx;
        } else {
          if (!(flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)) {
            x = x + (ttf_i16)(points[0] * 256 + points[1]);
            points += 2;
          }
        }
        coords[i].x = (ttf_i16)x;
      }

      ttf_i32 y = 0;
      for (ttf_i32 i = 0; i < n_points; i += 1) {
        flag = flags[i];
        if (flag & Y_SHORT_VECTOR) {
          ttf_i16 dy = *points++;
          y += (flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) ? dy : -dy;
        } else {
          if (!(flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)) {
            y = y + (ttf_i16)(points[0] * 256 + points[1]);
            points += 2;
          }
        }
        coords[i].y = (ttf_i16)y;
      }
    }

    ttf_i32 n_linears = 0;
    ttf_i32 n_beziers = 0;
    ttf_i32 current  = 0;

    for (ttf_i32 c = 0; c < gh.numberOfContours; c += 1) {
      ttf_i16 end = ttf_be16toh(contours[c]);

      TTF_Point prev;
      ttf_bool  prev_on;

      if (flags[end] & 1) {
        prev = end[coords];
        prev_on = ttf_true;
      } else {
        prev.x = coords[current].x / 2 + coords[end].x / 2;
        prev.y = coords[current].y / 2 + coords[end].y / 2;
        prev_on = ttf_false;
      }

      ttf_i32 start = current;
      while (current <= end) {
        if (flags[current] & 1) {
          if (prev_on) {
            if (!!shape->linears && shape->n_linears > n_linears) {
              if (prev.y < current[coords].y) {
                shape->linears[n_linears] = (TTF_Segment_Linear) {.a = prev, .b = current[coords]};
              } else {
                shape->linears[n_linears] = (TTF_Segment_Linear) {.a = current[coords], .b = prev};
              }
            }
            n_linears += 1;
          } else {
            prev_on = ttf_true;
          }
          prev = current[coords];
        } else {
          if (current + 1 > end) {
            if (!!shape->beziers && shape->n_beziers > n_beziers) {
              if (flags[start] & 1) {
                shape->beziers[n_beziers] =
                  (TTF_Segment_Bezier) {.p0 = prev, .p1 = coords[current], .p2 = coords[start]};
              } else {
                TTF_Point mid;
                mid.x = coords[current].x / 2 + coords[start].x / 2;
                mid.y = coords[current].y / 2 + coords[start].y / 2;
                shape->beziers[n_beziers] =
                  (TTF_Segment_Bezier) {.p0 = prev, .p1 = coords[current], .p2 = mid};
              }
              if (shape->beziers[n_beziers].p0.y > shape->beziers[n_beziers].p2.y) {
                TTF_Point tmp                = shape->beziers[n_beziers].p0;
                shape->beziers[n_beziers].p0 = shape->beziers[n_beziers].p2;
                shape->beziers[n_beziers].p2 = tmp;
              }
            }
            n_beziers += 1;
            break;
          }
          if (!!shape->beziers && shape->n_beziers > n_beziers) {
            if (flags[current + 1] & 1) {
              shape->beziers[n_beziers] = (TTF_Segment_Bezier) {.p0 = prev, .p1 = coords[current], .p2 = coords[current + 1]};
              prev = coords[current + 1];
              current += 1;
            } else {
              TTF_Point mid;
              mid.x = coords[current].x / 2 + coords[current + 1].x / 2;
              mid.y = coords[current].y / 2 + coords[current + 1].y / 2;
              shape->beziers[n_beziers] = (TTF_Segment_Bezier) {.p0 = prev, .p1 = coords[current], .p2 = mid};
              prev = mid;
            }
            if (shape->beziers[n_beziers].p0.y > shape->beziers[n_beziers].p2.y) {
              TTF_Point tmp                = shape->beziers[n_beziers].p0;
              shape->beziers[n_beziers].p0 = shape->beziers[n_beziers].p2;
              shape->beziers[n_beziers].p2 = tmp;
            }
          }
          n_beziers += 1;
        }
        current += 1;
      }
      current = end + 1;
    }

    if (shape->beziers) {
      shape->n_beziers = ttf_min(n_beziers, shape->n_beziers);
    } else {
      shape->n_beziers = n_beziers;
    }
    if (shape->linears) {
      shape->n_linears = ttf_min(n_linears, shape->n_linears);
    } else {
      shape->n_linears = n_linears;
    }
  } else {
    typedef enum {
      ARG_1_AND_2_ARE_WORDS     = 0x0001,
      ARGS_ARE_XY_VALUES        = 0x0002,
      ROUND_XY_TO_GRID          = 0x0004,
      WE_HAVE_A_SCALE           = 0x0008,
      MORE_COMPONENTS           = 0x0020,
      WE_HAVE_AN_X_AND_Y_SCALE  = 0x0040,
      WE_HAVE_A_TWO_BY_TWO      = 0x0080,
      WE_HAVE_INSTRUCTIONS      = 0x0100,
      USE_MY_METRICS            = 0x0200,
      OVERLAP_COMPOUND          = 0x0400,
      SCALED_COMPONENT_OFFSET   = 0x0800,
      UNSCALED_COMPONENT_OFFSET = 0x1000,
    } TTF_Component_Glyph_Flag;

    ttf_u16 flags = MORE_COMPONENTS, glyph_index;
    ttf_u8 *data  = &font->data[font->glyf + offset + sizeof(gh)];

    ttf_u32 n_beziers = 0;
    ttf_u32 n_linears = 0;

    while (flags & MORE_COMPONENTS) {
      ttf_memcpy(&flags,       &data[0], sizeof(flags));
      ttf_memcpy(&glyph_index, &data[2], sizeof(glyph_index));
      data += 4;

      flags       = ttf_be16toh(flags);
      glyph_index = ttf_be16toh(glyph_index);

      TTF_Glyph_Shape subshape = {
        .beziers   = shape->beziers ? shape->beziers + n_beziers : TTF_NULL,
        .linears   = shape->beziers ? shape->linears + n_linears : TTF_NULL,
        .n_beziers = shape->n_beziers - n_beziers,
        .n_linears = shape->n_linears - n_linears,
      };
      ttf_get_glyph_shape(font, glyph_index, &subshape);
      n_linears += subshape.n_linears;
      n_beziers += subshape.n_beziers;
      
      if (flags & ARGS_ARE_XY_VALUES) {
        ttf_i16 args[2];
        if (flags & ARG_1_AND_2_ARE_WORDS) {
          ttf_memcpy(args, data, sizeof(args));
          data += sizeof(args);
          args[0] = ttf_be16toh(args[0]);
          args[1] = ttf_be16toh(args[1]);
        } else {
          ttf_i8 args8[2];
          ttf_memcpy(args8, data, sizeof(args8));
          data += sizeof(args8);
          args[0] = args8[0];
          args[1] = args8[1];
        }
        
        if (subshape.beziers) {
          for (ttf_i32 i = 0; i < subshape.n_beziers; i += 1) {
            subshape.beziers[i].p0.data[0] += args[0];
            subshape.beziers[i].p0.data[1] += args[1];

            subshape.beziers[i].p1.data[0] += args[0];
            subshape.beziers[i].p1.data[1] += args[1];

            subshape.beziers[i].p2.data[0] += args[0];
            subshape.beziers[i].p2.data[1] += args[1];
          }
        }
        if (subshape.linears) {
          for (ttf_i32 i = 0; i < subshape.n_linears; i += 1) {
            subshape.linears[i].a.data[0] += args[0];
            subshape.linears[i].a.data[1] += args[1];

            subshape.linears[i].b.data[0] += args[0];
            subshape.linears[i].b.data[1] += args[1];
          }
        }
      }
      if (flags & WE_HAVE_A_SCALE) {
        ttf_i16 scale;
        ttf_memcpy(&scale, data, sizeof(scale));
        data += sizeof(scale);
        // printf("WE_HAVE_A_SCALE\n");
      }
      if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
        ttf_i16 scale[2];
        ttf_memcpy(scale, data, sizeof(scale));
        data += sizeof(scale);
        // printf("WE_HAVE_AN_X_AND_Y_SCALE\n");
      }
      if (flags & WE_HAVE_A_TWO_BY_TWO) {
        ttf_i16 mat[4];
        ttf_memcpy(mat, data, sizeof(mat));
        data += sizeof(mat);
        // printf("WE_HAVE_A_TWO_BY_TWO\n");
      }
      if (flags & USE_MY_METRICS) {
      //   printf("USE_MY_METRICS\n");
      }
      if (flags & SCALED_COMPONENT_OFFSET) {
      //   printf("SCALED_COMPONENT_OFFSET\n");
      }
      if (flags & UNSCALED_COMPONENT_OFFSET) {
        // printf("UNSCALED_COMPONENT_OFFSET\n");
      }
      if (flags & WE_HAVE_INSTRUCTIONS) {
        ttf_u16 n_instructions;
        ttf_memcpy(&n_instructions, data, sizeof(n_instructions));
        data += sizeof(n_instructions) + ttf_be16toh(n_instructions);
      }

      if (flags & 0xE010) {
        return;
      }
    }

    if (shape->beziers) {
      shape->n_beziers = ttf_min(n_beziers, shape->n_beziers);
    } else {
      shape->n_beziers = n_beziers;
    }
    if (shape->linears) {
      shape->n_linears = ttf_min(n_linears, shape->n_linears);
    } else {
      shape->n_linears = n_linears;
    }
  }
}

TTF_DEF void ttf_get_codepoint_bitmap_size(
  TTF_Font        const *font,
  ttf_u32                codepoint,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
) {
  ttf_get_glyph_bitmap_size(font, ttf_get_codepoint_glyph(font, codepoint), font_size, w, h);
}

TTF_DEF void ttf_get_glyph_bitmap_size(
  TTF_Font        const *font,
  ttf_u32                glyph,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
) {
  TTF_Glyph_Shape shape = {0};
  ttf_get_glyph_shape(font, glyph, &shape);
  ttf_get_shape_bitmap_size(font, &shape, font_size, w, h);
}

TTF_DEF void ttf_get_shape_bitmap_size(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h
) {
  const ttf_f32 scale = font_size / font->units_per_em;
  *w = (shape->max.x - shape->min.x) * scale + 2;
  *h = (shape->max.y - shape->min.y) * scale + 2;
}

TTF_DEF void ttf_render_codepoint_bitmap(
  TTF_Font        const *font,
  ttf_u32                codepoint,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
) {
  ttf_render_glyph_bitmap(font, ttf_get_codepoint_glyph(font, codepoint), font_size, stride, pixels);
}

TTF_DEF void ttf_render_glyph_bitmap(
  TTF_Font        const *font,
  ttf_u32                glyph,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
) {
  TTF_Glyph_Shape shape = {0};
  ttf_get_glyph_shape(font, glyph, &shape);
  TTF_Segment_Bezier beziers[shape.n_beziers];
  TTF_Segment_Linear linears[shape.n_linears];
  shape.beziers = beziers;
  shape.linears = linears;
  ttf_get_glyph_shape(font, glyph, &shape);

  ttf_render_shape_bitmap(font, &shape, font_size, stride, pixels);
}

TTF_INTERNAL ttf_i32 _ttf_shape_get_intersections(
  TTF_Glyph_Shape const *shape,
  ttf_f32               *intersections,
  ttf_f32                y
) {
  ttf_i32 n_intersections = 0;

  #define INTERSECTION(x) {intersections[n_intersections] = x; n_intersections += 1;}

  for (ttf_i32 i = 0; i < shape->n_linears; i += 1) {
    TTF_Segment_Linear linear = shape->linears[i];
    if (ttf_absf(linear.a.y - linear.b.y) < 0.0001) {
      continue;
    }
    if (
      linear.b.y >  y &&
      linear.a.y <= y
    ) {
      ttf_f32 t  = (y - linear.a.y) / (linear.b.y - linear.a.y);
      ttf_f32 vx = (1 - t) * linear.a.x + t * linear.b.x;

      INTERSECTION(vx);
    }
  }

  for (ttf_i32 i = 0; i < shape->n_beziers; i += 1) {
    TTF_Segment_Bezier bezier = shape->beziers[i];

    ttf_f32 a = bezier.p0.y - 2 * bezier.p1.y + bezier.p2.y;
    ttf_f32 b = -2 * bezier.p0.y + 2 * bezier.p1.y;
    ttf_f32 c = bezier.p0.y - y;

    ttf_f32 t, vx, dy;
    if (ttf_absf(a) < 0.0001) {
      if (bezier.p0.y == bezier.p2.y) {
        continue;
      }
      if (
        bezier.p2.y >  y &&
        bezier.p0.y <= y
      ) {
        t  = (y - bezier.p0.y) / (bezier.p2.y - bezier.p0.y);
        vx = (1 - t) * ((1 - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1 - t) * bezier.p1.x + t * bezier.p2.x);

        INTERSECTION(vx);
      }

      continue;
    }

    ttf_f32 determinant = b * b - 4 * a * c;
    if (determinant < 0) {
      continue;
    }

    ttf_f32 root = ttf_sqrtf(determinant);
    
    t  = (-b + root) / (2 * a);
    vx = (1 - t) * ((1 - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1 - t) * bezier.p1.x + t * bezier.p2.x);
    dy = 2 * (1 - t) * (bezier.p1.y - bezier.p0.y) + 2 * t * (bezier.p2.y - bezier.p1.y);

    if (ttf_absf(dy) < 0.0001) {
      if (y == bezier.p0.y) {
        INTERSECTION(bezier.p0.x);
        continue;
      }
      continue;
    }

    if (
      0 <= t && t <= 1 &&
      _ttf_bezier_curve_intersection(bezier, y, dy)
    ) {
      INTERSECTION(vx);
    }

    t  = (-b - root) / (2 * a);
    vx = (1 - t) * ((1 - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1 - t) * bezier.p1.x + t * bezier.p2.x);
    dy = 2 * (1 - t) * (bezier.p1.y - bezier.p0.y) + 2 * t * (bezier.p2.y - bezier.p1.y);

    if (
      0 <= t && t <= 1 &&
      _ttf_bezier_curve_intersection(bezier, y, dy)
    ) {
      INTERSECTION(vx);
    }
  }

  ttf_sort_f32s(intersections, n_intersections);
  
  return n_intersections;
}

TTF_DEF void ttf_render_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_u32                stride,
  ttf_u8                *pixels
) {
  const ttf_f32 SCALE = font_size / font->units_per_em;

  const ttf_i32 W = (shape->max.x - shape->min.x) * SCALE + 2;
  const ttf_i32 H = (shape->max.y - shape->min.y) * SCALE + 2;

  #define Y_SAMPLES 4
  #define X_SAMPLES 4

  ttf_f32   intersections[Y_SAMPLES][shape->n_linears + shape->n_beziers * 2];
  ttf_i32 n_intersections[Y_SAMPLES];

  for (ttf_i32 iy = 0; iy < H; iy += 1) {
    for (ttf_i32 y_sample = 0; y_sample < Y_SAMPLES; y_sample += 1) {
      ttf_f32 y = ((H - iy - 1) + y_sample / (ttf_f32)Y_SAMPLES - 0.5) / SCALE + shape->min.y;
      n_intersections[y_sample] = _ttf_shape_get_intersections(shape, intersections[y_sample], y);
    }

    for (ttf_i32 ix = 0; ix < W; ix += 1) {
      ttf_i32 hits = 0;
      for (ttf_i32 y_sample = 0; y_sample < Y_SAMPLES; y_sample += 1) {
        for (ttf_i32 x_sample = 0; x_sample < X_SAMPLES; x_sample += 1) {
          ttf_i32 ni = n_intersections[y_sample];
          ttf_f32 x  = (ix + x_sample / (ttf_f32)X_SAMPLES - 0.5) / SCALE + shape->min.x;
          if (ni > 0) {
            while ((ni != 0) && x > intersections[y_sample][ni - 1]) {
              ni -= 1;
            }
            if (ni & 1) {
              hits += 1;
            }
          }
        }
      }
      pixels[ix + iy * (W + stride)] = (ttf_f32)255 * hits / (Y_SAMPLES * X_SAMPLES);
    }
  }
}

TTF_INTERNAL ttf_f32 _ttf_sdf2_linear(
  TTF_Segment_Linear const *linear,
  ttf_f32                   x,
  ttf_f32                   y
) {
  ttf_f32 bax = linear->b.x - linear->a.x;
  ttf_f32 bay = linear->b.y - linear->a.y;

  ttf_f32 pax = x - linear->a.x;
  ttf_f32 pay = y - linear->a.y;

  ttf_f32 t   = (bax * pax + bay * pay) / (bax * bax + bay * bay);
  if (t < 0) {
    t = 0;
  } else if (t > 1) {
    t = 1;
  }
  ttf_f32 vx = (1 - t) * linear->a.x + t * linear->b.x;
  ttf_f32 vy = (1 - t) * linear->a.y + t * linear->b.y;

  ttf_f32 v  = (vx - x) * (vx - x) + (vy - y) * (vy - y);
  return v;
}

TTF_DEF void ttf_render_shape_sdf(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_f32                spread,
  ttf_u32                stride,
  ttf_u8                *pixels
) {
  const ttf_f32 SCALE = font_size / font->units_per_em;

  const ttf_i32 W = (shape->max.x - shape->min.x) * SCALE + 2;
  const ttf_i32 H = (shape->max.y - shape->min.y) * SCALE + 2;

  ttf_f32 intersections[shape->n_linears + shape->n_beziers * 2];

  for (ttf_i32 iy = 0; iy < H; iy += 1) {
    ttf_f32 y = (H - iy - 1) / SCALE + shape->min.y;
    ttf_i32 n_intersections = _ttf_shape_get_intersections(shape, intersections, y);

    for (ttf_i32 ix = 0; ix < W; ix += 1) {
      ttf_f32 x = ix / SCALE + shape->min.x;

      ttf_f32 value = 1e10;

      for (ttf_i32 i = 0; i < shape->n_linears; i += 1) {
        TTF_Segment_Linear linear = shape->linears[i];
        ttf_f32 v = _ttf_sdf2_linear(&linear, x, y);

        if (value > v) {
          value = v;
        }
      }
      
      for (ttf_i32 i = 0; i < shape->n_beziers; i += 1) {
        TTF_Segment_Bezier bezier = shape->beziers[i];
        TTF_Segment_Linear linear;
        #define N_SEGMENTS 16
        for (ttf_i32 i = 0; i < N_SEGMENTS; i += 1) {
          ttf_f32 t0 =  (ttf_f32)i      / N_SEGMENTS;
          ttf_f32 t1 = ((ttf_f32)i + 1) / N_SEGMENTS;
          linear.a.x = (1 - t0) * ((1 - t0) * bezier.p0.x + t0 * bezier.p1.x) + t0 * ((1 - t0) * bezier.p1.x + t0 * bezier.p2.x);
          linear.a.y = (1 - t0) * ((1 - t0) * bezier.p0.y + t0 * bezier.p1.y) + t0 * ((1 - t0) * bezier.p1.y + t0 * bezier.p2.y);
          linear.b.x = (1 - t1) * ((1 - t1) * bezier.p0.x + t1 * bezier.p1.x) + t1 * ((1 - t1) * bezier.p1.x + t1 * bezier.p2.x);
          linear.b.y = (1 - t1) * ((1 - t1) * bezier.p0.y + t1 * bezier.p1.y) + t1 * ((1 - t1) * bezier.p1.y + t1 * bezier.p2.y);
          ttf_f32 v = _ttf_sdf2_linear(&linear, x, y);

          if (value > v) {
            value = v;
          }
        }
      }

      value  = ttf_sqrtf(value);
      value /= spread;
      value /= font->units_per_em;

      if (value > 1) {
        value = 1;
      } else if (value < 0) {
        value = 0;
      }
    
      while ((n_intersections != 0) && x > intersections[n_intersections - 1]) {
        n_intersections -= 1;
      }

      if (n_intersections & 1) {
        value = 0.5 * value + 0.5;
      } else {
        value = 0.5 * (1 - value);
      }
      pixels[ix + iy * (W + stride)] = (ttf_f32)255 * value;
    }
  }
}

TTF_DEF void ttf_get_codepoint_bitmap(
  TTF_Font const *font,
  ttf_u32         codepoint,
  ttf_f32         font_size,
  ttf_u32        *w,
  ttf_u32        *h,
  ttf_u8        **pixels
) {
  ttf_get_glyph_bitmap(font, ttf_get_codepoint_glyph(font, codepoint), font_size, w, h, pixels);
}

TTF_DEF void ttf_get_glyph_bitmap(
  TTF_Font const *font,
  ttf_u32         glyph,
  ttf_f32         scale,
  ttf_u32        *w,
  ttf_u32        *h,
  ttf_u8        **pixels
) {
  TTF_Glyph_Shape shape = {0};
  ttf_get_glyph_shape(font, glyph, &shape);
  TTF_Segment_Bezier beziers[shape.n_beziers];
  TTF_Segment_Linear linears[shape.n_linears];
  shape.beziers = beziers;
  shape.linears = linears;
  ttf_get_glyph_shape(font, glyph, &shape);

  ttf_get_shape_bitmap(font, &shape, scale, w, h, pixels);
}

TTF_DEF void ttf_get_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  ttf_f32                font_size,
  ttf_u32               *w,
  ttf_u32               *h,
  ttf_u8               **pixels
) {
  ttf_u32 W;
  ttf_u32 H;

  ttf_get_shape_bitmap_size(font, shape, font_size, &W, &H);

  *pixels = (ttf_u8 *)ttf_alloc(font->allocator, W * H);
  if (!*pixels) {
    *w = 0;
    *h = 0;
    return;
  }

  *w = W;
  *h = H;

  ttf_render_shape_bitmap(font, shape, font_size, 0, *pixels);
  // ttf_render_shape_sdf(font, shape, font_size, 0.1, 0, *pixels);
}

#undef ttf_max
#undef ttf_min

#undef TTF_INTERNAL

#endif // TTF_IMPLEMENTATION

#undef TTF_DEF

#endif // __TTF_INCLUDE__
