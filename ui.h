#include "codin.h"
#include "bad_font.h"
#include "image.h"

typedef struct {
  i32 x0;
  i32 y0;
  i32 x1;
  i32 y1;
} Rectangle;

typedef struct {
  Rectangle rect;
  u32       color;
  u32       color2; // Used by `UI_Command_Type_Gradient` 
  u32       outline;
} UI_Command_Box;

typedef struct {
  String    text;
  Rectangle bounds;
  i32       size;
  u32       color;
} UI_Command_Text;

typedef struct {
  i32 index;
} UI_Image;

typedef struct {
  Rectangle rect;
  u32       tint;
  u32       outline;
  UI_Image  image;
} UI_Command_Image;

typedef enum {
  UI_Command_Type_None = 0,
  UI_Command_Type_Box,
  UI_Command_Type_Gradient,
  UI_Command_Type_Text,
  UI_Command_Type_Image,
} UI_Command_Type;

typedef struct {
  union {
    UI_Command_Box   box;
    UI_Command_Text  text;
    UI_Command_Image image;
  } variant;
  UI_Command_Type type;
} UI_Command;

internal u32 ui_hash_bytes(u32 in, Byte_Slice bytes) {
  slice_iter(bytes, byte, _i, {
    in += *byte;
  })
}

internal u32 hash_ui_command(u32 in, UI_Command command) {
  switch (command.type) {
  case UI_Command_Type_None:
    break;
  case UI_Command_Type_Box:
  case UI_Command_Type_Gradient:
    ui_hash_bytes(in, any_to_bytes(&command.variant.box));
    break;
  case UI_Command_Type_Text:
    ui_hash_bytes(in, string_to_bytes(command.variant.text.text));
    ui_hash_bytes(in, slice_start(any_to_bytes(&command.variant.text), offset_of(UI_Command_Text, bounds)));
    break;
  case UI_Command_Type_Image:
    ui_hash_bytes(in, any_to_bytes(&command.variant.image));
    break;
  }
  return in;
}

typedef enum {
  UI_Color_Background,
  UI_Color_Text,
  UI_Color_Image_Border,

  UI_Color_Label,
  UI_Color_Label_Text,
  UI_Color_Label_Outline,

  UI_Color_Button,
  UI_Color_Button_2,
  UI_Color_Button_Text,
  UI_Color_Button_Outline,

  UI_Color_Button_Hovered,
  UI_Color_Button_Hovered_2,
  UI_Color_Button_Hovered_Text,
  UI_Color_Button_Hovered_Outline,

  UI_Color_Button_Clicked,
  UI_Color_Button_Clicked_2,
  UI_Color_Button_Clicked_Text,
  UI_Color_Button_Clicked_Outline,
  
  UI_Color_MAX,
} UI_Color;

typedef struct {
  Vector(UI_Command) commands;
  i32                width, height;
  i32                x, y;
  i32                spacing;
  b8                 horizontal;
  struct {
    i32 x, y;
    b8  buttons[2];
  } mouse;
  BMF_Font           font;
  Slice(u32)         command_hashes;
  Slice(u32)         prev_command_hashes;
  Vector(Image)      images;
  u32                colors[UI_Color_MAX];
} UI_Context;

internal Rectangle ui_insert_rect(UI_Context *ctx, isize width, isize height) {
  Rectangle rect;
  rect.x0 = ctx->x;
  rect.y0 = ctx->y;

  if (ctx->horizontal) {
    rect.x1 = ctx->x + width;
    rect.y1 = ctx->y + height;
    ctx->x += width + ctx->spacing;
    return rect;
  } else {
    rect.x1 = ctx->x + width;
    rect.y1 = ctx->y + height;
    ctx->y += height + ctx->spacing;
    return rect;
  }
}

#define UI_HASH_CELL_SIZE 32

internal void ui_context_init(UI_Context *ctx, BMF_Font font, isize width, isize height, Allocator allocator) {
  isize n_cells = ((width  + UI_HASH_CELL_SIZE - 1) / UI_HASH_CELL_SIZE) *
                  ((height + UI_HASH_CELL_SIZE - 1) / UI_HASH_CELL_SIZE);
  slice_init(&ctx->command_hashes,      n_cells, allocator);
  slice_init(&ctx->prev_command_hashes, n_cells, allocator);
  vector_init(&ctx->commands, 0, 8, allocator);
  vector_init(&ctx->images, 0, 8, allocator);

  ctx->font    = font;
  ctx->width   = width;
  ctx->height  = height;
  ctx->spacing = font.decender;

  ctx->colors[UI_Color_Background            ] = 0xFF000000;
  ctx->colors[UI_Color_Image_Border          ] = 0xFF62AEEF;
  ctx->colors[UI_Color_Label                 ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Label_Text            ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Label_Outline         ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button                ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Button_2              ] = 0x11FFFFFF;
  ctx->colors[UI_Color_Button_Text           ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Outline        ] = 0xFF62AEEF;
  ctx->colors[UI_Color_Button_Hovered        ] = 0x33FFFFFF;
  ctx->colors[UI_Color_Button_Hovered_2      ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Button_Hovered_Text   ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Hovered_Outline] = 0xFFE06B74;
  ctx->colors[UI_Color_Button_Clicked        ] = 0x44FFFFFF;
  ctx->colors[UI_Color_Button_Clicked_2      ] = 0x33FFFFFF;
  ctx->colors[UI_Color_Button_Clicked_Text   ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Clicked_Outline] = 0xFFE06B74;
}

internal void ui_context_destroy(UI_Context *ctx, Allocator allocator) {
  slice_delete(ctx->command_hashes,      allocator);
  slice_delete(ctx->prev_command_hashes, allocator);
  vector_delete(ctx->commands);
}

internal b8 ui_mouse_in_rect(UI_Context *ctx, Rectangle const *rect) {
  if (in_range(ctx->mouse.x, rect->x0, rect->x1)) {
    if (in_range(ctx->mouse.y, rect->y0, rect->y1)) {
      return true;
    }
  }
  return false;
}

internal b8 ui_button(UI_Context *ctx, String text) {
  i32 width = bmf_measure_text(&ctx->font, text);
  UI_Command cmd;
  Rectangle rect = ui_insert_rect(
    ctx,
    width + ctx->font.decender * 2,
    ctx->font.single_h + ctx->font.decender
  );

  UI_Color color         = UI_Color_Button;
  UI_Color color_2       = UI_Color_Button_2;
  UI_Color text_color    = UI_Color_Button_Text;
  UI_Color outline_color = UI_Color_Button_Outline;
  
  if (ui_mouse_in_rect(ctx, &rect)) {
    if (ctx->mouse.buttons[0]) {
      *(isize *)&color         += 8;
      *(isize *)&color_2       += 8;
      *(isize *)&text_color    += 8;
      *(isize *)&outline_color += 8;
    } else {
      *(isize *)&color         += 4;
      *(isize *)&color_2       += 4;
      *(isize *)&text_color    += 4;
      *(isize *)&outline_color += 4;
    }
  }

  cmd.type        = UI_Command_Type_Box;
  cmd.variant.box = (UI_Command_Box) {
    .rect    = rect,
    .color   = ctx->colors[color],
    .color2  = ctx->colors[color_2],
    .outline = ctx->colors[outline_color],
  };
  vector_append(&ctx->commands, cmd);

  cmd.type         = UI_Command_Type_Text;
  cmd.variant.text = (UI_Command_Text) {
    .text   = text,
    .bounds = rect,
    .color  = ctx->colors[text_color],
  };

  vector_append(&ctx->commands, cmd);

  return color == UI_Color_Button_Clicked;
}

internal void ui_label(UI_Context *ctx, String text) {
  i32 width = bmf_measure_text(&ctx->font, text);
  UI_Command cmd;
  Rectangle rect = ui_insert_rect(
    ctx,
    width + ctx->font.decender * 2,
    ctx->font.single_h + ctx->font.decender
  );

  UI_Color color         = UI_Color_Label;
  UI_Color text_color    = UI_Color_Label_Text;
  UI_Color outline_color = UI_Color_Label_Outline;
  
  cmd.type        = UI_Command_Type_Box;
  cmd.variant.box = (UI_Command_Box) {
    .rect    = rect,
    .color   = ctx->colors[color],
    .outline = ctx->colors[outline_color],
  };
  vector_append(&ctx->commands, cmd);

  cmd.type        = UI_Command_Type_Text;
  cmd.variant.text = (UI_Command_Text) {
    .text   = text,
    .bounds = rect,
    .color  = ctx->colors[text_color],
  };

  vector_append(&ctx->commands, cmd);
}

internal UI_Image ui_create_image(UI_Context *ctx, Image image) {
  UI_Image ret = {.index = (i32)ctx->images.len};
  vector_append(&ctx->images, image);
  return ret;
}

internal void ui_image(UI_Context *ctx, UI_Image img) {
  UI_Command cmd;
  Image *image = &ctx->images.data[img.index];
  Rectangle rect = ui_insert_rect(ctx, (i32)image->width, (i32)image->height);

  cmd.type          = UI_Command_Type_Image;
  cmd.variant.image = (UI_Command_Image) {
    .rect    = rect,
    .outline = ctx->colors[UI_Color_Image_Border],
    .image   = img,
  };
  vector_append(&ctx->commands, cmd);
}

// Rectangle cut_left(Rectangle* rect, i32 a) {
//     i32 x0 = rect->x0;
//     rect->x0 = min(rect->x1, rect->x0 + a);
//     return (Rectangle){ x0, rect->y0, rect->x0, rect->y1 };
// }

// Rectangle cut_right(Rectangle* rect, i32 a) {
//     i32 x1 = rect->x1;
//     rect->x1 = max(rect->x0, rect->x1 - a);
//     return (Rectangle){ rect->x1, rect->y0, x1, rect->y1 };
// }

// Rectangle cut_top(Rectangle* rect, i32 a) {
//     i32 y0 = rect->y0;
//     rect->y0 = min(rect->y1, rect->y0 + a);
//     return (Rectangle){ rect->x0, y0, rect->x1, rect->y0 };
// }

// Rectangle cut_bottom(Rectangle* rect, i32 a) {
//     i32 y1 = rect->y1;
//     rect->y1 = max(rect->y0, rect->y1 - a);
//     return (Rectangle){ rect->x0, rect->y1, rect->x1, y1 };
// }
