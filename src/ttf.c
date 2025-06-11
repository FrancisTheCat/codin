#include "ttf.h"

#include "codin.h"
#include "sort.h"
#include "math.h"

typedef struct {
  u16 version;
  u16 num_tables;
} _TTF_Cmap_Header;

typedef struct {
  u16 platform_id;
  u16 encoding_id;
  u32 offset;
} _TTF_Cmap_Encoding_Record;

typedef struct {
  u16 format;
  u16 _reserved;
  u32 length;
  u32 language;
  u32 num_groups;
} _TTF_Cmap_Subtable_Format_12;

typedef struct {
  u16 format;
  u16 length;
  u16 language;
  u16 segCountX2;
  u16 _searchRange;
  u16 _entrySelector;
  u16 _rangeShift;
} _TTF_Cmap_Subtable_Format_4;

typedef struct {
  u8  tableTag[4];
  u32 checksum;
  u32 offset;
  u32 length;
} _TTF_Table_Record;

typedef struct {
  i16 numberOfContours;
  i16 xMin;
  i16 yMin;
  i16 xMax;
  i16 yMax;
} _TTF_Glyph_Header;

typedef struct {
  u16 version[2];
  u16 numGlyphs;
} _TTF_Maxp_Table;

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  u32 fontRevision;
  u32 checksumAdjustment;
  u32 magicNumber;
  u16 flags;
  u16 unitsPerEm;
  u64 created;
  u64 modified;
  i16 xMin;
  i16 yMin;
  i16 xMax;
  i16 yMax;
  u16 macStyle;
  u16 lowestRecPPEM;
  i16 fontDirectionHint;
  i16 indexToLocFormat;
  i16 glyphDataFormat;
} _TTF_Font_Header_Table;

typedef struct {
  u32 sfntVesion;
  u16 numTables;
  u16 searchRange;
  u16 entrySelector;
  u16 rangeShift;
} _TTF_File_Header;

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  i16 ascender;
  i16 descender;
  i16 lineGap;
  u16 advanceWidthMax;
  i16 minLeftSideBearing;
  i16 minRightSideBearing;
  i16 xMaxExtent;
  i16 caretSlopeRise;
  i16 caretSlopeRun;
  i16 caretOffset;
  i16 _reserved[4];
  i16 metricDataFormat;
  u16 numberOfHMetrics;
} _TTF_Hhea_Table;

typedef union {
  u16 version;
  struct {
    u16  version;
    i16  xAvgCharWidth;
    u16  usWeightClass;
    u16  usWidthClass;
    u16  fsType;
    i16  ySubscriptXSize;
    i16  ySubscriptYSize;
    i16  ySubscriptXOffset;
    i16  ySubscriptYOffset;
    i16  ySuperscriptXSize;
    i16  ySuperscriptYSize;
    i16  ySuperscriptXOffset;
    i16  ySuperscriptYOffset;
    i16  yStrikeoutSize;
    i16  yStrikeoutPosition;
    i16  sFamilyClass;
    u8   panose[10];
    u32  ulUnicodeRange1;
    u32  ulUnicodeRange2;
    u32  ulUnicodeRange3;
    u32  ulUnicodeRange4;
    char achVendID[4];
    u16  fsSelection;
    u16  usFirstCharIndex;
    u16  usLastCharIndex;
    i16  sTypoAscender;
    i16  sTypoDescender;
    i16  sTypoLineGap;
    u16  usWinAscent;
    u16  usWinDescent;
  } v0;
  struct {
    u16  version;
    i16  xAvgCharWidth;
    u16  usWeightClass;
    u16  usWidthClass;
    u16  fsType;
    i16  ySubscriptXSize;
    i16  ySubscriptYSize;
    i16  ySubscriptXOffset;
    i16  ySubscriptYOffset;
    i16  ySuperscriptXSize;
    i16  ySuperscriptYSize;
    i16  ySuperscriptXOffset;
    i16  ySuperscriptYOffset;
    i16  yStrikeoutSize;
    i16  yStrikeoutPosition;
    i16  sFamilyClass;
    u8   panose[10];
    u32  ulUnicodeRange1;
    u32  ulUnicodeRange2;
    u32  ulUnicodeRange3;
    u32  ulUnicodeRange4;
    char achVendID[4];
    u16  fsSelection;
    u16  usFirstCharIndex;
    u16  usLastCharIndex;
    i16  sTypoAscender;
    i16  sTypoDescender;
    i16  sTypoLineGap;
    u16  usWinAscent;
    u16  usWinDescent;
    u32  ulCodePageRange1;
    u32  ulCodePageRange2;
  } v1;
  struct {
    u16  version;
    i16  xAvgCharWidth;
    u16  usWeightClass;
    u16  usWidthClass;
    u16  fsType;
    i16  ySubscriptXSize;
    i16  ySubscriptYSize;
    i16  ySubscriptXOffset;
    i16  ySubscriptYOffset;
    i16  ySuperscriptXSize;
    i16  ySuperscriptYSize;
    i16  ySuperscriptXOffset;
    i16  ySuperscriptYOffset;
    i16  yStrikeoutSize;
    i16  yStrikeoutPosition;
    i16  sFamilyClass;
    u8   panose[10];
    u32  ulUnicodeRange1;
    u32  ulUnicodeRange2;
    u32  ulUnicodeRange3;
    u32  ulUnicodeRange4;
    char achVendID[4];
    u16  fsSelection;
    u16  usFirstCharIndex;
    u16  usLastCharIndex;
    i16  sTypoAscender;
    i16  sTypoDescender;
    i16  sTypoLineGap;
    u16  usWinAscent;
    u16  usWinDescent;
    u32  ulCodePageRange1;
    u32  ulCodePageRange2;
    i16  sxHeight;
    i16  sCapHeight;
    u16  usDefaultChar;
    u16  usBreakChar;
    u16  usMaxContext;
  } v4;
  struct {
    u16  version;
    i16  xAvgCharWidth;
    u16  usWeightClass;
    u16  usWidthClass;
    u16  fsType;
    i16  ySubscriptXSize;
    i16  ySubscriptYSize;
    i16  ySubscriptXOffset;
    i16  ySubscriptYOffset;
    i16  ySuperscriptXSize;
    i16  ySuperscriptYSize;
    i16  ySuperscriptXOffset;
    i16  ySuperscriptYOffset;
    i16  yStrikeoutSize;
    i16  yStrikeoutPosition;
    i16  sFamilyClass;
    u8   panose[10];
    u32  ulUnicodeRange1;
    u32  ulUnicodeRange2;
    u32  ulUnicodeRange3;
    u32  ulUnicodeRange4;
    char achVendID[4];
    u16  fsSelection;
    u16  usFirstCharIndex;
    u16  usLastCharIndex;
    i16  sTypoAscender;
    i16  sTypoDescender;
    i16  sTypoLineGap;
    u16  usWinAscent;
    u16  usWinDescent;
    u32  ulCodePageRange1;
    u32  ulCodePageRange2;
    i16  sxHeight;
    i16  sCapHeight;
    u16  usDefaultChar;
    u16  usBreakChar;
    u16  usMaxContext;
    u16  usLowerOpticalPointSize;
    u16  usUpperOpticalPointSize;
  } v5;
} _TTF_OS2_Header;

#define ttf_be16toh(x) __builtin_bswap16(x)
#define ttf_be32toh(x) __builtin_bswap32(x)

internal void _ttf_convert_cmap_subtable_format_4(_TTF_Cmap_Subtable_Format_4 *f) {
  f->format         = ttf_be16toh(f->format);
  f->length         = ttf_be16toh(f->length);
  f->language       = ttf_be16toh(f->language);
  f->segCountX2     = ttf_be16toh(f->segCountX2);
  f->_searchRange   = ttf_be16toh(f->_searchRange);
  f->_entrySelector = ttf_be16toh(f->_entrySelector);
  f->_rangeShift    = ttf_be16toh(f->_rangeShift);
}

internal void _ttf_convert_cmap_subtable_format_12(_TTF_Cmap_Subtable_Format_12 *f) {
  f->format     = ttf_be16toh(f->format);
  f->length     = ttf_be32toh(f->length);
  f->language   = ttf_be32toh(f->language);
  f->num_groups = ttf_be32toh(f->num_groups);
}

internal void _ttf_convert_file_header(_TTF_File_Header *h) {
  h->sfntVesion    = ttf_be32toh(h->sfntVesion);
  h->numTables     = ttf_be16toh(h->numTables);
  h->searchRange   = ttf_be16toh(h->searchRange);
  h->entrySelector = ttf_be16toh(h->entrySelector);
  h->rangeShift    = ttf_be16toh(h->rangeShift);
}

internal void _ttf_convert_table_record(_TTF_Table_Record *r) {
  r->checksum = ttf_be32toh(r->checksum);
  r->offset   = ttf_be32toh(r->offset);
  r->length   = ttf_be32toh(r->length);
}

internal void _ttf_convert_glyph_header(_TTF_Glyph_Header *h) {
  h->numberOfContours = ttf_be16toh(h->numberOfContours);
  h->xMin             = ttf_be16toh(h->xMin);
  h->yMin             = ttf_be16toh(h->yMin);
  h->xMax             = ttf_be16toh(h->xMax);
  h->yMax             = ttf_be16toh(h->yMax);
}

internal void _ttf_get_glyph_header(TTF_Font const *font, u32 glyph, _TTF_Glyph_Header *header) {
  u32 offset;
  u32 next;

  #if 1
    offset = ((u32 *)&font->data[font->loca])[glyph];
    next   = ((u32 *)&font->data[font->loca])[glyph + 1];
  #else
    offset = ((u16 *)&font->data[font->loca])[glyph];
    next   = ((u16 *)&font->data[font->loca])[glyph + 1];
  #endif

  offset = ttf_be32toh(offset);
  next   = ttf_be32toh(next);

  if (next - offset < sizeof(_TTF_Glyph_Header)) {
    return;
  }

  mem_copy(header, &font->data[font->glyf + offset], sizeof(*header));
  _ttf_convert_glyph_header(header);
}

#define ttf_min(a, b) ((a) > (b) ? (b) : (a))
#define ttf_max(a, b) ((a) < (b) ? (b) : (a))

extern f32 ttf_get_line_height(TTF_Font const *font, f32 font_size) {
  return 2 * font_size * font->line_height / font->units_per_em;
}

extern f32 ttf_get_font_height(TTF_Font const *font, f32 font_size) {
  return font_size * font->font_height / font->units_per_em;
}

extern void ttf_get_codepoint_h_metrics(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  TTF_H_Metrics  *metrics
) {
  ttf_get_glyph_h_metrics(font, ttf_get_codepoint_glyph(font, codepoint), font_size, metrics);
}

extern void ttf_get_glyph_h_metrics(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  TTF_H_Metrics  *metrics
) {
  typedef struct {
    u16 advance;
    i16 bearing;
  } TTF_Long_Hor_Metric_Record;
  TTF_Long_Hor_Metric_Record *records = (TTF_Long_Hor_Metric_Record *)&font->data[font->hmtx];
  if (glyph >= font->n_hmetrics) {
    metrics->advance =
      font_size * ttf_be16toh(             records[font->n_hmetrics - 1].advance                   ) / font->units_per_em;
    metrics->bearing =
      font_size * ttf_be16toh(((i16 *)&records[font->n_hmetrics    ])[glyph - font->n_hmetrics]) / font->units_per_em;
    return;
  }
  metrics->advance = font_size *          ttf_be16toh(records[glyph].advance) / font->units_per_em;
  metrics->bearing = font_size * (i16)ttf_be16toh(records[glyph].bearing) / font->units_per_em;
}

extern void ttf_get_codepoint_v_metrics(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  TTF_V_Metrics  *metrics
) {
  ttf_get_glyph_v_metrics(font, ttf_get_codepoint_glyph(font, codepoint), font_size, metrics);
}

extern void ttf_get_glyph_v_metrics(
  TTF_Font const *font,
  u32             glyph,
  f32             font_size,
  TTF_V_Metrics  *metrics
) {
  _TTF_Glyph_Header gh;
  _ttf_get_glyph_header(font, glyph, &gh);
  metrics->bearing = -font_size * (           gh.yMin) / font->units_per_em;
  metrics->height  =  font_size * ( gh.yMax - gh.yMin) / font->units_per_em;
}

extern bool ttf_load_bytes(u8 *data, i32 n, TTF_Font *font, Allocator allocator) {
  *font = (TTF_Font) {0};

  if ((uintptr)data & 3) {
    return false;
  }
  if (n < sizeof(_TTF_File_Header)) {
    return false;
  }
  _TTF_File_Header header;
  mem_copy(&header, data, sizeof(header));
  _ttf_convert_file_header(&header);

  font->data = data;
  font->len  = n;

  if (sizeof(_TTF_File_Header) + sizeof(_TTF_Table_Record) * header.numTables > n) {
    return false;
  }

  _TTF_Table_Record *table_records = (_TTF_Table_Record *)&data[sizeof(_TTF_File_Header)];
  
  font->n_glyphs   = -1;
  font->loca       = -1;
  font->glyf       = -1;
  font->cmap       = -1;
  font->n_hmetrics = -1;

  for (i32 table_index = 0; table_index < header.numTables; table_index += 1) {
    _TTF_Table_Record table;
    mem_copy(&table, &table_index[table_records], sizeof(table));
    _ttf_convert_table_record(&table);

    #define CHECK_TAG(str) (*(u32 *)table.tableTag == *(u32 *)str)

    if (CHECK_TAG("maxp")) {
      _TTF_Maxp_Table maxp;
      mem_copy(&maxp, &data[table.offset], sizeof(maxp));
      font->n_glyphs = ttf_be16toh(maxp.numGlyphs);
    }
    if (CHECK_TAG("head")) {
      _TTF_Font_Header_Table head;
      mem_copy(&head, &data[table.offset], sizeof(head));
      font->units_per_em = ttf_be16toh(head.unitsPerEm);
    }
    if (CHECK_TAG("hhea")) {
      _TTF_Hhea_Table hhea;
      mem_copy(&hhea, &data[table.offset], sizeof(hhea));
      font->n_hmetrics  = ttf_be16toh(hhea.numberOfHMetrics);
      font->line_height = (i16)ttf_be16toh(hhea.lineGap) + (i16)ttf_be16toh(hhea.ascender) + (i16)ttf_be16toh(hhea.descender);
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
      mem_copy(&os2, &data[table.offset], ttf_min(sizeof(os2), n - table.offset));
      switch (ttf_be16toh(os2.version)) {
      case 0:
        continue;
        break;

      case 1:
      case 2:
      case 3:
        continue;
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
    font->n_glyphs   == -1 ||
    font->loca       == -1 ||
    font->glyf       == -1 ||
    font->hmtx       == -1 ||
    font->n_hmetrics == -1 ||
    font->cmap       == -1
  ) {
    return false;
  }

  _TTF_Cmap_Header cmap_header;
  mem_copy(&cmap_header, &data[font->cmap], sizeof(cmap_header));
  cmap_header.num_tables = ttf_be16toh(cmap_header.num_tables);
  cmap_header.version    = ttf_be16toh(cmap_header.version);

  font->groups   = nil;
  font->n_groups = -1;

  bool full_unicode = false;

  for (i32 cmap_index = 0; cmap_index < cmap_header.num_tables; cmap_index += 1) {
    _TTF_Cmap_Encoding_Record record;
    mem_copy(&record, &data[font->cmap + sizeof(cmap_header) + cmap_index * sizeof(record)], sizeof(record));

    record.offset      = ttf_be32toh(record.offset);
    record.encoding_id = ttf_be16toh(record.encoding_id);
    record.platform_id = ttf_be16toh(record.platform_id);

    if (record.platform_id == 0 && record.encoding_id == 4) {
      full_unicode = true;
    }
  }

  for (i32 cmap_index = 0; cmap_index < cmap_header.num_tables; cmap_index += 1) {
    _TTF_Cmap_Encoding_Record record;
    mem_copy(&record, &data[font->cmap + sizeof(cmap_header) + cmap_index * sizeof(record)], sizeof(record));

    record.offset      = ttf_be32toh(record.offset);
    record.encoding_id = ttf_be16toh(record.encoding_id);
    record.platform_id = ttf_be16toh(record.platform_id);

    if (record.platform_id == 0) {
      switch (record.encoding_id) {
      case 3:
        if (!full_unicode) {
          if (ttf_be16toh(*(u16 *)&data[font->cmap + record.offset]) != 4) {
            break;
          }
          _TTF_Cmap_Subtable_Format_4 subtable;
          mem_copy(&subtable, &data[font->cmap + record.offset], sizeof(subtable));
          _ttf_convert_cmap_subtable_format_4(&subtable);

          u16 segment_count = subtable.segCountX2 / 2;
          _TTF_Sequential_Map_Group *segments =
            (_TTF_Sequential_Map_Group *)unwrap_err(mem_alloc(sizeof(_TTF_Sequential_Map_Group) * segment_count, allocator));

          u16 *p = (u16 *)&data[font->cmap + record.offset + sizeof(subtable)];

          for (i32 i = 0; i < segment_count; i += 1) {
            segments[i].end_char_code = ttf_be16toh(*p);
            p += 1;
          }

          // padding
          if (*p != 0) {
            break;
          }
          p += 1;

          for (i32 i = 0; i < segment_count; i += 1) {
            segments[i].start_char_code = ttf_be16toh(*p);
            p += 1;
          }

          for (i32 i = 0; i < segment_count; i += 1) {
            segments[i].glyph_id_delta = ttf_be16toh(*(i16 *)p);
            p += 1;
          }

          for (i32 i = 0; i < segment_count; i += 1) {
            if (*p) {
              segments[i] = (_TTF_Sequential_Map_Group) {0};
              // return false;
            }
            p += 1;
          }

          font->groups   = segments;
          font->n_groups = segment_count;
          font->bmp      = true;

          goto end_cmap;
        }
        break;
      case 4:
        {
          if (ttf_be16toh(*(u16 *)&data[font->cmap + record.offset]) != 12) {
            break;
          }
          _TTF_Cmap_Subtable_Format_12 subtable;
          mem_copy(&subtable, &data[font->cmap + record.offset], sizeof(subtable));
          _ttf_convert_cmap_subtable_format_12(&subtable);

          _TTF_Sequential_Map_Group *segments =
            (_TTF_Sequential_Map_Group *)unwrap_err(mem_alloc(sizeof(_TTF_Sequential_Map_Group) * subtable.num_groups, allocator));

          if (!segments) {
            return false;
          }

          font->groups   = segments;
          font->n_groups = subtable.num_groups;

          _TTF_Sequential_Map_Group *groups   = (_TTF_Sequential_Map_Group *)&data[font->cmap + record.offset + sizeof(_TTF_Cmap_Subtable_Format_12)];

          for (i32 i = 0; i < font->n_groups; i += 1) {
            _TTF_Sequential_Map_Group group = groups[i];
            font->groups[i].start_char_code = ttf_be32toh(group.start_char_code);
            font->groups[i].end_char_code   = ttf_be32toh(group.end_char_code);
            font->groups[i].start_glyph_id  = ttf_be32toh(group.start_glyph_id);
          }

          font->bmp = false;
          goto end_cmap;
        }
        break;
      }
    }
  }
  end_cmap:

  if (font->n_groups == -1 || font->groups == nil) {
    return false;
  }

  if (!font->font_height) {
    _TTF_Glyph_Header gh;
    _ttf_get_glyph_header(font, 0, &gh);
    font->font_height = gh.yMax - gh.yMin;
  }

  return true;
}

extern void ttf_destroy_font(TTF_Font const *font, Allocator allocator) {
  mem_free(font->groups, font->n_groups * size_of(_TTF_Sequential_Map_Group), allocator);
}

extern bool _ttf_bezier_curve_intersection(
  TTF_Segment_Bezier bezier,
  f32                y,
  f32                dy
) {
  // "U"
  if (bezier.p1.y < bezier.p0.y) {
    if (dy < 0) {
      if (bezier.p0.y > y) {
        return true;
      }
    } else {
      if (bezier.p2.y > y) {
        return true;
      }
    }
  // "^"
  } else if (bezier.p1.y >= bezier.p2.y) {
    if (dy > 0) {
      if (bezier.p0.y <= y) {
        return true;
      }
    } else {
      if (bezier.p2.y <= y) {
        return true;
      }
    }
  // "/"
  } else {
    if (
      bezier.p2.y >  y &&
      bezier.p0.y <= y
    ) {
      return true;
    }
  }

  return false;
}

extern u32 ttf_get_codepoint_glyph(
  TTF_Font const *font,
  rune            codepoint
) {
  u32 glyph_id = 0;
  if (font->bmp) {
    for (i32 i = 0; i < font->n_groups; i += 1) {
      _TTF_Sequential_Map_Group group = font->groups[i];
      if (group.start_char_code <= codepoint && codepoint <= group.end_char_code) {
        glyph_id = codepoint + group.glyph_id_delta;
        break;
      }
    }
  } else {
    for (i32 i = 0; i < font->n_groups; i += 1) {
      _TTF_Sequential_Map_Group group = font->groups[i];
      if (group.start_char_code <= codepoint && codepoint <= group.end_char_code) {
        glyph_id = group.start_glyph_id + codepoint - group.start_char_code;
        break;
      }
    }
  }
  return glyph_id;
}

extern void ttf_get_codepoint_shape(
  TTF_Font const  *font,
  rune             codepoint,
  TTF_Glyph_Shape *shape
) {
  ttf_get_glyph_shape(font, ttf_get_codepoint_glyph(font, codepoint), shape);
}

extern void ttf_get_glyph_shape(
  TTF_Font const  *font,
  u32              glyph,
  TTF_Glyph_Shape *shape
) {
  u32 offset;
  u32 next;

  #if 1
    offset = ((u32 *)&font->data[font->loca])[glyph];
    next   = ((u32 *)&font->data[font->loca])[glyph + 1];
  #else
    offset = ((u16 *)&font->data[font->loca])[glyph];
    next   = ((u16 *)&font->data[font->loca])[glyph + 1];
  #endif

  offset = ttf_be32toh(offset);
  next   = ttf_be32toh(next);

  if (next - offset < sizeof(_TTF_Glyph_Header)) {
    return;
  }

  _TTF_Glyph_Header gh;
  mem_copy(&gh, &font->data[font->glyf + offset], sizeof(gh));
  _ttf_convert_glyph_header(&gh);

  shape->min.x = gh.xMin;
  shape->min.y = gh.yMin;
  shape->max.x = gh.xMax;
  shape->max.y = gh.yMax;

  if (gh.numberOfContours >= 0) {
    u8  *description        = &font->data[font->glyf + offset + sizeof(gh)];
    u16  instruction_length = ttf_be16toh(((u16 *)&font->data[font->glyf + offset + sizeof(gh)])[gh.numberOfContours]);

    i16 *contours = (i16 *)description;

    i32 n_points = ttf_be16toh(contours[gh.numberOfContours - 1]) + 1;
    u8 *points   = &description[gh.numberOfContours * 2 + sizeof(u16) + instruction_length];

    u8        flags [n_points];
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

      u8 flag;
      for (i32 i = 0; i < n_points; i += 1) {
        flag = *points++;
        if (flag & REPEAT_FLAG) {
          i32 repeat_count = *points++;
          for (i32 j = 0; j < repeat_count + 1; j += 1) {
            flags[i + j] = flag;
          }
          i += repeat_count;
        } else {
          flags[i] = flag;
        }
      }

      i32 x = 0;
      for (i32 i = 0; i < n_points; i += 1) {
        flag = flags[i];
        if (flag & X_SHORT_VECTOR) {
          i16 dx = *points++;
          x += (flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) ? dx : -dx;
        } else {
          if (!(flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)) {
            x = x + (i16)(points[0] * 256 + points[1]);
            points += 2;
          }
        }
        coords[i].x = (i16)x;
      }

      i32 y = 0;
      for (i32 i = 0; i < n_points; i += 1) {
        flag = flags[i];
        if (flag & Y_SHORT_VECTOR) {
          i16 dy = *points++;
          y += (flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) ? dy : -dy;
        } else {
          if (!(flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)) {
            y = y + (i16)(points[0] * 256 + points[1]);
            points += 2;
          }
        }
        coords[i].y = (i16)y;
      }
    }

    i32 n_linears = 0;
    i32 n_beziers = 0;
    i32 current   = 0;

    for (i32 c = 0; c < gh.numberOfContours; c += 1) {
      i16 end = ttf_be16toh(contours[c]);

      TTF_Point prev;
      bool      prev_on;

      if (flags[end] & 1) {
        prev = end[coords];
        prev_on = true;
      } else {
        prev.x = coords[current].x / 2 + coords[end].x / 2;
        prev.y = coords[current].y / 2 + coords[end].y / 2;
        prev_on = false;
      }

      i32 start = current;
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
            prev_on = true;
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

    u16 flags = MORE_COMPONENTS, glyph_index;
    u8 *data  = &font->data[font->glyf + offset + sizeof(gh)];

    u32 n_beziers = 0;
    u32 n_linears = 0;

    while (flags & MORE_COMPONENTS) {
      mem_copy(&flags,       &data[0], sizeof(flags));
      mem_copy(&glyph_index, &data[2], sizeof(glyph_index));
      data += 4;

      flags       = ttf_be16toh(flags);
      glyph_index = ttf_be16toh(glyph_index);

      TTF_Glyph_Shape subshape = {
        .beziers   = shape->beziers ? shape->beziers + n_beziers : nil,
        .linears   = shape->beziers ? shape->linears + n_linears : nil,
        .n_beziers = shape->n_beziers - n_beziers,
        .n_linears = shape->n_linears - n_linears,
      };
      ttf_get_glyph_shape(font, glyph_index, &subshape);
      n_linears += subshape.n_linears;
      n_beziers += subshape.n_beziers;
      
      if (flags & ARGS_ARE_XY_VALUES) {
        i16 args[2];
        if (flags & ARG_1_AND_2_ARE_WORDS) {
          mem_copy(args, data, sizeof(args));
          data += sizeof(args);
          args[0] = ttf_be16toh(args[0]);
          args[1] = ttf_be16toh(args[1]);
        } else {
          i8 args8[2];
          mem_copy(args8, data, sizeof(args8));
          data += sizeof(args8);
          args[0] = args8[0];
          args[1] = args8[1];
        }
        
        if (subshape.beziers) {
          for (i32 i = 0; i < subshape.n_beziers; i += 1) {
            subshape.beziers[i].p0.data[0] += args[0];
            subshape.beziers[i].p0.data[1] += args[1];

            subshape.beziers[i].p1.data[0] += args[0];
            subshape.beziers[i].p1.data[1] += args[1];

            subshape.beziers[i].p2.data[0] += args[0];
            subshape.beziers[i].p2.data[1] += args[1];
          }
        }
        if (subshape.linears) {
          for (i32 i = 0; i < subshape.n_linears; i += 1) {
            subshape.linears[i].a.data[0] += args[0];
            subshape.linears[i].a.data[1] += args[1];

            subshape.linears[i].b.data[0] += args[0];
            subshape.linears[i].b.data[1] += args[1];
          }
        }
      }
      if (flags & WE_HAVE_A_SCALE) {
        i16 scale;
        mem_copy(&scale, data, sizeof(scale));
        data += sizeof(scale);
        // printf("WE_HAVE_A_SCALE\n");
      }
      if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
        i16 scale[2];
        mem_copy(scale, data, sizeof(scale));
        data += sizeof(scale);
        // printf("WE_HAVE_AN_X_AND_Y_SCALE\n");
      }
      if (flags & WE_HAVE_A_TWO_BY_TWO) {
        i16 mat[4];
        mem_copy(mat, data, sizeof(mat));
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
        u16 n_instructions;
        mem_copy(&n_instructions, data, sizeof(n_instructions));
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

extern void ttf_get_codepoint_bitmap_size(
  TTF_Font        const *font,
  rune                   codepoint,
  f32                    font_size,
  u32                   *w,
  u32                   *h
) {
  ttf_get_glyph_bitmap_size(font, ttf_get_codepoint_glyph(font, codepoint), font_size, w, h);
}

extern void ttf_get_glyph_bitmap_size(
  TTF_Font        const *font,
  u32                    glyph,
  f32                    font_size,
  u32                   *w,
  u32                   *h
) {
  TTF_Glyph_Shape shape = {0};
  ttf_get_glyph_shape(font, glyph, &shape);
  ttf_get_shape_bitmap_size(font, &shape, font_size, w, h);
}

extern void ttf_get_shape_bitmap_size(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  u32                   *w,
  u32                   *h
) {
  const f32 scale = font_size / font->units_per_em;
  *w = (shape->max.x - shape->min.x) * scale + 2;
  *h = (shape->max.y - shape->min.y) * scale + 2;
}

extern void ttf_render_codepoint_bitmap(
  TTF_Font        const *font,
  rune                   codepoint,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
) {
  ttf_render_glyph_bitmap(font, ttf_get_codepoint_glyph(font, codepoint), font_size, stride, pixels);
}

extern void ttf_render_glyph_bitmap(
  TTF_Font        const *font,
  u32                    glyph,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
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

internal void intersection(f32 *intersections, i32 *n_intersections, f32 x) {
  bool found = false;
  for_range(i, 0, *n_intersections) {
    if (intersections[i] < x) {
      found = true;

      for (int j = *n_intersections; j > i; j -= 1) {
        intersections[j] = intersections[j - 1];
      }

      intersections[i] = x;
      break;
    }
  }
  if (!found) {
    intersections[*n_intersections] = x;
  }
  *n_intersections += 1;
}

internal i32 _ttf_shape_get_intersections(
  TTF_Glyph_Shape const *shape,
  f32                   *intersections,
  f32                    y
) {
  i32 n_intersections = 0;

  for (i32 i = 0; i < shape->n_linears; i += 1) {
    TTF_Segment_Linear linear = shape->linears[i];
    if (
      linear.b.y >  y &&
      linear.a.y <= y
    ) {
      f32 t  = (y - linear.a.y) / (linear.b.y - linear.a.y);
      f32 vx = (1.0f - t) * linear.a.x + t * linear.b.x;

      intersection(intersections, &n_intersections, vx);
    }
  }

  for (i32 i = 0; i < shape->n_beziers; i += 1) {
    TTF_Segment_Bezier bezier = shape->beziers[i];

    f32 a = bezier.p0.y - 2.0f * bezier.p1.y + bezier.p2.y;
    f32 b = -2.0f * bezier.p0.y + 2.0f * bezier.p1.y;
    f32 c = bezier.p0.y - y;

    f32 t, vx, dy;
    if (abs_f32(a) < 0.0001) {
      if (
        bezier.p2.y >  y &&
        bezier.p0.y <= y
      ) {
        t  = (y - bezier.p0.y) / (bezier.p2.y - bezier.p0.y);
        vx = (1.0f - t) * ((1.0f - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1.0f - t) * bezier.p1.x + t * bezier.p2.x);

        intersection(intersections, &n_intersections, vx);
      }

      continue;
    }

    f32 determinant = b * b - 4.0f * a * c;
    if (determinant < 0) {
      continue;
    }

    f32 root = sqrt_f32(determinant);
    
    t  = (-b + root) / (2.0f * a);
    vx = (1.0f - t) * ((1.0f - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1.0f - t) * bezier.p1.x + t * bezier.p2.x);
    dy = 2.0f * (1.0f - t) * (bezier.p1.y - bezier.p0.y) + 2.0f * t * (bezier.p2.y - bezier.p1.y);

    if (abs_f32(dy) < 0.0001) {
      if (y == bezier.p0.y) {
        intersection(intersections, &n_intersections, bezier.p0.y);
        continue;
      }
      continue;
    }

    if (
      0.0f <= t && t <= 1.0f &&
      _ttf_bezier_curve_intersection(bezier, y, dy)
    ) {
      intersection(intersections, &n_intersections, vx);
    }

    t  = (-b - root) / (2.0f * a);
    vx = (1.0f - t) * ((1.0f - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1.0f - t) * bezier.p1.x + t * bezier.p2.x);
    dy = 2.0f * (1.0f - t) * (bezier.p1.y - bezier.p0.y) + 2.0f * t * (bezier.p2.y - bezier.p1.y);

    if (
      0.0f <= t && t <= 1.0f &&
      _ttf_bezier_curve_intersection(bezier, y, dy)
    ) {
      intersection(intersections, &n_intersections, vx);
    }
  }

  return n_intersections;
}

// should be a multiple of 8 for SIMD
#define CURVES_PER_CELL 32

typedef struct {
  f32 min, max;
} Bound;

typedef struct {
  TTF_Segment_Bezier *beziers;
  TTF_Segment_Linear *linears;
  Bound              *bounds_bezier;
  Bound              *bounds_linear;
  i32                 n_linears;
  i32                 n_beziers;
} Accel_Shape;

internal i32 _ttf_shape_get_intersections_accelerated(
  Accel_Shape const *shape,
  f32               *intersections,
  f32                y
) {
  i32 n_intersections = 0;

  for_range(i, 0, (shape->n_linears + CURVES_PER_CELL - 1) / CURVES_PER_CELL) {
    Bound bound = shape->bounds_linear[i];
    if (y < bound.min || y > bound.max) {
      continue;
    }
    for_range(j, 0, CURVES_PER_CELL) {
      if (i * CURVES_PER_CELL + j >= shape->n_linears) {
        break;
      }
      TTF_Segment_Linear linear = shape->linears[i * CURVES_PER_CELL + j];
      if (
        linear.b.y >  y &&
        linear.a.y <= y
      ) {
        f32 t  = (y - linear.a.y) / (linear.b.y - linear.a.y);
        f32 vx = (1 - t) * linear.a.x + t * linear.b.x;

        intersection(intersections, &n_intersections, vx);
      }
    }
  }

  for_range(i, 0, (shape->n_beziers + CURVES_PER_CELL - 1) / CURVES_PER_CELL) {
    Bound bound = shape->bounds_bezier[i];
    if (y < bound.min || y > bound.max) {
      continue;
    }
    for_range(j, 0, CURVES_PER_CELL) {
      if (i * CURVES_PER_CELL + j >= shape->n_beziers) {
        break;
      }
      TTF_Segment_Bezier bezier = shape->beziers[i * CURVES_PER_CELL + j];

      f32 a = bezier.p0.y - 2.0f * bezier.p1.y + bezier.p2.y;
      f32 b = -2.0f * bezier.p0.y + 2.0f * bezier.p1.y;
      f32 c = bezier.p0.y - y;

      f32 t, vx, dy;
      if (abs_f32(a) < 0.0001f) {
        if (
          bezier.p2.y >  y &&
          bezier.p0.y <= y
        ) {
          t  = (y - bezier.p0.y) / (bezier.p2.y - bezier.p0.y);
          vx = (1 - t) * ((1 - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1 - t) * bezier.p1.x + t * bezier.p2.x);

          intersection(intersections, &n_intersections, vx);
        }

        continue;
      }

      f32 determinant = b * b - 4.0f * a * c;
      if (determinant < 0) {
        continue;
      }

      f32 root = sqrt_f32(determinant);
    
      t  = (-b + root) / (2.0f * a);
      vx = (1.0f - t) * ((1.0f - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1.0f - t) * bezier.p1.x + t * bezier.p2.x);
      dy = 2.0f * (1.0f - t) * (bezier.p1.y - bezier.p0.y) + 2.0f * t * (bezier.p2.y - bezier.p1.y);

      if (abs_f32(dy) < 0.0001f) {
        if (y == bezier.p0.y) {
          intersection(intersections, &n_intersections, bezier.p0.x);
          continue;
        }
        continue;
      }

      if (
        0.0f <= t && t <= 1.0f &&
        _ttf_bezier_curve_intersection(bezier, y, dy)
      ) {
        intersection(intersections, &n_intersections, vx);
      }

      t  = (-b - root) / (2.0f * a);
      vx = (1.0f - t) * ((1.0f - t) * bezier.p0.x + t * bezier.p1.x) + t * ((1.0f - t) * bezier.p1.x + t * bezier.p2.x);
      dy = 2.0f * (1.0f - t) * (bezier.p1.y - bezier.p0.y) + 2.0f * t * (bezier.p2.y - bezier.p1.y);

      if (
        0.0f <= t && t <= 1.0f &&
        _ttf_bezier_curve_intersection(bezier, y, dy)
      ) {
        intersection(intersections, &n_intersections, vx);
      }
    }
  }

  return n_intersections;
}

extern void ttf_render_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  u32                    stride,
  u8                    *pixels
) {
  Bound bounds_bezier[(shape->n_beziers + CURVES_PER_CELL - 1) / CURVES_PER_CELL];
  Bound bounds_linear[(shape->n_linears + CURVES_PER_CELL - 1) / CURVES_PER_CELL];
  Accel_Shape as = {
    .beziers       = shape->beziers,
    .linears       = shape->linears,
    .n_beziers     = shape->n_beziers,
    .n_linears     = shape->n_linears,
    .bounds_bezier = bounds_bezier,
    .bounds_linear = bounds_linear,
  };

  Slice(TTF_Segment_Bezier) beziers = { .data = shape->beziers, .len = shape->n_beziers, };
  sort_slice_by(beziers, i, j, ({
    f32 mid_i = IDX(beziers, i).p0.y + IDX(beziers, i).p1.y + IDX(beziers, i).p2.y;
    f32 mid_j = IDX(beziers, j).p0.y + IDX(beziers, j).p1.y + IDX(beziers, j).p2.y;

    mid_i < mid_j;
  }));

  Slice(TTF_Segment_Linear) linears = { .data = shape->linears, .len = shape->n_linears, };
  sort_slice_by(linears, i, j, ({
    f32 mid_i = IDX(linears, i).a.y + IDX(linears, i).b.y;
    f32 mid_j = IDX(linears, j).a.y + IDX(linears, j).b.y;

    mid_i < mid_j;
  }));

  for_range(i, 0, (shape->n_beziers + CURVES_PER_CELL - 1) / CURVES_PER_CELL) {
    f32 min_y = F32_INFINITY, max_y = -F32_INFINITY;
    for_range(j, 0, CURVES_PER_CELL) {
      if (i * CURVES_PER_CELL + j >= shape->n_beziers) {
        break;
      }
      min_y = min(min_y, shape->beziers[i * CURVES_PER_CELL + j].p0.y);
      min_y = min(min_y, shape->beziers[i * CURVES_PER_CELL + j].p1.y);
      min_y = min(min_y, shape->beziers[i * CURVES_PER_CELL + j].p2.y);

      max_y = max(max_y, shape->beziers[i * CURVES_PER_CELL + j].p0.y);
      max_y = max(max_y, shape->beziers[i * CURVES_PER_CELL + j].p1.y);
      max_y = max(max_y, shape->beziers[i * CURVES_PER_CELL + j].p2.y);
    }

    as.bounds_bezier[i] = (Bound) { .min = min_y, .max = max_y, };
  }

  for_range(i, 0, (shape->n_linears + CURVES_PER_CELL - 1) / CURVES_PER_CELL) {
    f32 min_y = F32_INFINITY, max_y = -F32_INFINITY;
    for_range(j, 0, CURVES_PER_CELL) {
      if (i * CURVES_PER_CELL + j >= shape->n_linears) {
        break;
      }
      min_y = min(min_y, shape->linears[i * CURVES_PER_CELL + j].a.y);
      min_y = min(min_y, shape->linears[i * CURVES_PER_CELL + j].b.y);

      max_y = max(max_y, shape->linears[i * CURVES_PER_CELL + j].a.y);
      max_y = max(max_y, shape->linears[i * CURVES_PER_CELL + j].b.y);
    }

    as.bounds_linear[i] = (Bound) { .min = min_y - 0.0001f, .max = max_y + 0.0001f, };
  }

  const f32 SCALE = font_size / font->units_per_em;

  const i32 W = (shape->max.x - shape->min.x) * SCALE + 2;
  const i32 H = (shape->max.y - shape->min.y) * SCALE + 2;

  #define Y_SAMPLES 4
  #define X_SAMPLES 4

  f32   intersections[Y_SAMPLES][shape->n_linears + shape->n_beziers * 2];
  i32 n_intersections[Y_SAMPLES];

  for (i32 iy = 0; iy < H; iy += 1) {
    for (i32 y_sample = 0; y_sample < Y_SAMPLES; y_sample += 1) {
      f32 y = ((H - iy - 1) + y_sample / (f32)Y_SAMPLES - 0.5f) / SCALE + shape->min.y;
      // n_intersections[y_sample] = _ttf_shape_get_intersections(shape, intersections[y_sample], y);
      n_intersections[y_sample] = _ttf_shape_get_intersections_accelerated(&as, intersections[y_sample], y);
    }

    for (i32 ix = 0; ix < W; ix += 1) {
      i32 hits = 0;
      for (i32 y_sample = 0; y_sample < Y_SAMPLES; y_sample += 1) {
        for (i32 x_sample = 0; x_sample < X_SAMPLES; x_sample += 1) {
          i32 ni = n_intersections[y_sample];
          f32 x  = (ix + x_sample / (f32)X_SAMPLES - 0.5f) / SCALE + shape->min.x;
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
      pixels[ix + iy * (W + stride)] = (f32)255 * hits / (Y_SAMPLES * X_SAMPLES);
    }
  }
}

internal f32 _ttf_sdf2_linear(
  TTF_Segment_Linear const *linear,
  f32                       x,
  f32                       y
) {
  f32 bax = linear->b.x - linear->a.x;
  f32 bay = linear->b.y - linear->a.y;

  f32 pax = x - linear->a.x;
  f32 pay = y - linear->a.y;

  f32 t   = (bax * pax + bay * pay) / (bax * bax + bay * bay);
  if (t < 0) {
    t = 0;
  } else if (t > 1) {
    t = 1;
  }
  f32 vx = (1 - t) * linear->a.x + t * linear->b.x;
  f32 vy = (1 - t) * linear->a.y + t * linear->b.y;

  f32 v  = (vx - x) * (vx - x) + (vy - y) * (vy - y);
  return v;
}

extern void ttf_render_shape_sdf(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  f32                    spread,
  u32                    stride,
  u8                    *pixels
) {
  const f32 SCALE = font_size / font->units_per_em;

  const i32 W = (shape->max.x - shape->min.x) * SCALE + 2;
  const i32 H = (shape->max.y - shape->min.y) * SCALE + 2;

  f32 intersections[shape->n_linears + shape->n_beziers * 2];

  for (i32 iy = 0; iy < H; iy += 1) {
    f32 y = (H - iy - 1) / SCALE + shape->min.y;
    i32 n_intersections = _ttf_shape_get_intersections(shape, intersections, y);

    for (i32 ix = 0; ix < W; ix += 1) {
      f32 x = ix / SCALE + shape->min.x;

      f32 value = 1e10;

      for (i32 i = 0; i < shape->n_linears; i += 1) {
        TTF_Segment_Linear linear = shape->linears[i];
        f32 v = _ttf_sdf2_linear(&linear, x, y);

        if (value > v) {
          value = v;
        }
      }
      
      for (i32 i = 0; i < shape->n_beziers; i += 1) {
        TTF_Segment_Bezier bezier = shape->beziers[i];
        TTF_Segment_Linear linear;
        #define N_SEGMENTS 16
        for (i32 i = 0; i < N_SEGMENTS; i += 1) {
          f32 t0 =  (f32)i      / N_SEGMENTS;
          f32 t1 = ((f32)i + 1) / N_SEGMENTS;
          linear.a.x = (1 - t0) * ((1 - t0) * bezier.p0.x + t0 * bezier.p1.x) + t0 * ((1 - t0) * bezier.p1.x + t0 * bezier.p2.x);
          linear.a.y = (1 - t0) * ((1 - t0) * bezier.p0.y + t0 * bezier.p1.y) + t0 * ((1 - t0) * bezier.p1.y + t0 * bezier.p2.y);
          linear.b.x = (1 - t1) * ((1 - t1) * bezier.p0.x + t1 * bezier.p1.x) + t1 * ((1 - t1) * bezier.p1.x + t1 * bezier.p2.x);
          linear.b.y = (1 - t1) * ((1 - t1) * bezier.p0.y + t1 * bezier.p1.y) + t1 * ((1 - t1) * bezier.p1.y + t1 * bezier.p2.y);
          f32 v = _ttf_sdf2_linear(&linear, x, y);

          if (value > v) {
            value = v;
          }
        }
      }

      value  = sqrt_f32(value);
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
      pixels[ix + iy * (W + stride)] = (f32)255 * value;
    }
  }
}

extern void ttf_get_codepoint_bitmap(
  TTF_Font const *font,
  rune            codepoint,
  f32             font_size,
  u32            *w,
  u32            *h,
  u8            **pixels,
  Allocator       allocator
) {
  ttf_get_glyph_bitmap(font, ttf_get_codepoint_glyph(font, codepoint), font_size, w, h, pixels, allocator);
}

extern void ttf_get_glyph_bitmap(
  TTF_Font const *font,
  u32             glyph,
  f32             scale,
  u32            *w,
  u32            *h,
  u8            **pixels,
  Allocator       allocator
) {
  TTF_Glyph_Shape shape = {0};
  ttf_get_glyph_shape(font, glyph, &shape);
  TTF_Segment_Bezier beziers[shape.n_beziers];
  TTF_Segment_Linear linears[shape.n_linears];
  shape.beziers = beziers;
  shape.linears = linears;
  ttf_get_glyph_shape(font, glyph, &shape);

  ttf_get_shape_bitmap(font, &shape, scale, w, h, pixels, allocator);
}

extern void ttf_get_shape_bitmap(
  TTF_Font        const *font,
  TTF_Glyph_Shape const *shape,
  f32                    font_size,
  u32                   *w,
  u32                   *h,
  u8                   **pixels,
  Allocator              allocator
) {
  u32 W, H;
  ttf_get_shape_bitmap_size(font, shape, font_size, &W, &H);

  *pixels = (u8 *)unwrap_err(mem_alloc(W * H, allocator));
  if (!*pixels) {
    *w = 0;
    *h = 0;
    return;
  }

  *w = W;
  *h = H;

  ttf_render_shape_bitmap(font, shape, font_size, 0, *pixels);
}
