#include "codin.h"

typedef struct {
  Byte_Slice atlas;
  i32        atlas_width;
  i32        atlas_height;
	i32        glyphs_per_row;
	i32        single_w;
	i32        single_h;
	i32        size;
	i32        spacing;
	i32        decender;
} BMF_Font;

typedef struct {
	u8  magic[4];
	i32 atlas_w;
	i32 atlas_h;
	i32 glyphs_per_row;
	i32 glyph_w;
	i32 glyph_h;
	i32 font_size;
	i32 spacing;
	i32 decender;
} BMF_Font_Header;

internal b8 bmf_load_font(Byte_Slice data, BMF_Font *font) {
  BMF_Font_Header header;
  if (size_of(header) > data.len) {
    return false;
  }
  mem_copy(&header, data.data, size_of(header));

  if (!string_equal((String) {.data = (char *)header.magic, .len = size_of(header.magic)}, LIT("BMFF"))) {
    return false;
  }

  Byte_Slice atlas = slice_range(data, size_of(header), data.len);

  font->single_w       = header.glyph_w;
  font->single_h       = header.glyph_h;
  font->size           = header.font_size;
  font->spacing        = header.spacing;
  font->decender       = header.decender;
  font->glyphs_per_row = header.glyphs_per_row;
  font->atlas_width    = header.atlas_w;
  font->atlas_height   = header.atlas_h;
  font->atlas          = atlas;

  return atlas.len == header.atlas_w * header.atlas_h;
}

typedef struct {
	f32 x0;
	f32 y0;
	f32 x1;
	f32 y1;
	f32 u0;
	f32 v0;
	f32 u1;
	f32 v1;
} BMF_Baked_Quad;

typedef struct {
	i32 x0;
	i32 y0;
	i32 x1;
	i32 y1;
	i32 u0;
	i32 v0;
	i32 u1;
	i32 v1;
} BMF_Baked_Quad_I;

internal isize bmf_get_quad_index(rune c) {
  isize n = -1;

	switch (c) {
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
		n = (isize)(c - 'A');
		break;

  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
		n = (isize)(c - 'a') + 26;
		break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		n = (isize)(c - '0') + 26 * 2 + 4;
		break;
		
	case '.':
		n = (isize)(c - '.') + 26 * 2 + 10 + 4;
		break;
		
	case ',':
		n = (isize)(c - ',') + 26 * 2 + 11 + 4;
		break;
		
	case ':':
		n = (isize)(c - ':') + 26 * 2 + 12 + 4;
		break;
		
	case ';':
		n = (isize)(c - ';') + 26 * 2 + 13 + 4;
		break;
		
	case '!':
		n = (isize)(c - '!') + 26 * 2 + 14 + 4;
		break;
	}

	return n;
}

// returns false if the character was not found
internal b8 bmf_get_baked_quad(BMF_Font const *font, rune c, f32 *x, f32 *y, BMF_Baked_Quad *quad) {
  if (c == ' ') {
		*x += (f32)(font->single_w + font->spacing);
		mem_zero(quad, size_of(*quad));
		return true;
  }
  isize n = bmf_get_quad_index(c);
  if (n == -1) {
		// bmf_get_baked_quad(font, '!', x, y, quad);
		return false;
  }

	isize x0 = n % font->glyphs_per_row;
	isize y0 = n / font->glyphs_per_row;

	isize x0_px = x0 * font->single_w;
	isize y0_px = y0 * font->single_h;

	isize x1_px = x0_px + font->single_w;
	isize y1_px = y0_px + font->single_h;

	f32 fw = (f32)(font->atlas_width);
	f32 fh = (f32)(font->atlas_height);

	*x += (f32)(font->single_w + font->spacing);

	*quad = (BMF_Baked_Quad) {
  	*x - (f32)(font->spacing + font->single_w),
  	*y - (f32)(font->single_h) + (f32)(font->decender),
  	*x - (f32)(font->spacing),
  	*y + (f32)(font->decender),
  	(f32)(x0_px) / fw,
  	(f32)(y0_px) / fh,
  	(f32)(x1_px) / fw,
  	(f32)(y1_px) / fh,
  };
	return true;
}

// returns false if the character was not found
internal b8 bmf_get_baked_quad_i(BMF_Font const *font, rune c, isize *x, isize *y, BMF_Baked_Quad_I *quad) {
  if (c == ' ') {
		*x += font->single_w + font->spacing;
		mem_zero(quad, size_of(*quad));
		return true;
  }
  if (c == '\n') {
		*y += font->single_h + font->spacing;
		mem_zero(quad, size_of(*quad));
		return true;
  }
  isize n = bmf_get_quad_index(c);
  if (n == -1) {
		// bmf_get_baked_quad(font, '!', x, y, quad);
		return false;
  }

	i32 x0 = n % font->glyphs_per_row;
	i32 y0 = n / font->glyphs_per_row;

	i32 x0_px = x0 * font->single_w;
	i32 y0_px = y0 * font->single_h;

	i32 x1_px = x0_px + font->single_w;
	i32 y1_px = y0_px + font->single_h;

	*quad = (BMF_Baked_Quad_I) {
  	(i32)(*x),
  	(i32)(*y) - font->single_h + font->decender,
  	(i32)(*x) + font->single_w,
  	(i32)(*y) + font->decender,
  	x0_px,
  	y0_px,
  	x1_px,
  	y1_px,
  };

	*x += font->single_w + font->spacing;

	return true;
}

internal isize bmf_measure_text(BMF_Font const *font, String text) {
	if (text.len == 0) {
		return 0;
	}
	return text.len * (font->single_w + font->spacing) - font->spacing;
}
