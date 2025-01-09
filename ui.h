#include "codin.h"
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

ENUM_TO_STRING_PROC_DECL(UI_Command_Type, type) {
  switch (type) {
  case UI_Command_Type_None:
    return LIT("UI_Command_Type_None");
  case UI_Command_Type_Box:
    return LIT("UI_Command_Type_Box");
  case UI_Command_Type_Gradient:
    return LIT("UI_Command_Type_Gradient");
  case UI_Command_Type_Text:
    return LIT("UI_Command_Type_Text");
  case UI_Command_Type_Image:
    return LIT("UI_Command_Type_Image");
  default:
    return LIT("UI_Command_Type_INVALID");
  }
}

typedef struct {
  union {
    UI_Command_Box   box;
    UI_Command_Text  text;
    UI_Command_Image image;
  } variant;
  UI_Command_Type type;
} UI_Command;

#define UI_HASH_INITIAL 0x811c9dc5

internal u32 ui_hash_bytes(u32 in, Byte_Slice bytes) {
  slice_iter(bytes, byte, _i, {
    in = (in ^ *byte) * 0x01000193;
  })
  return in;
}

internal void ui_command_bounds(UI_Command const *command, Rectangle *rect) {
  switch (command->type) {
  case UI_Command_Type_None:
    *rect = (Rectangle) {0};
    break;
  case UI_Command_Type_Box:
  case UI_Command_Type_Gradient:
    *rect = command->variant.box.rect;
    break;
  case UI_Command_Type_Text:
    *rect = command->variant.text.bounds;
    break;
  case UI_Command_Type_Image:
    *rect = command->variant.image.rect;
    break;
  default:
    unreachable();
  }
}

internal u32 ui_command_hash(u32 in, UI_Command const *command) {
  switch (command->type) {
  case UI_Command_Type_None:
    break;
  case UI_Command_Type_Box:
  case UI_Command_Type_Gradient:
    in = ui_hash_bytes(in, any_to_bytes(&command->variant.box));
    break;
  case UI_Command_Type_Text:
    in = ui_hash_bytes(in, string_to_bytes(command->variant.text.text));
    in = ui_hash_bytes(in, slice_start(any_to_bytes(&command->variant.text), offset_of(UI_Command_Text, bounds)));
    break;
  case UI_Command_Type_Image:
    in = ui_hash_bytes(in, any_to_bytes(&command->variant.image));
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

typedef isize (*ui_measure_text_proc)(String str);

typedef struct {
  Vector(UI_Command)    commands;
  i32                   width, height;
  i32                   x, y;
  i32                   spacing;
  b8                    horizontal;
  struct {
    i32 x, y;
    b8  hovering;
    b8  buttons[2];
  } mouse;
  ui_measure_text_proc  measure_text_proc;
  i32                   text_height;
  Slice(u32)            command_hashes;
  Slice(u32)            prev_command_hashes;
  Vector(Image)         images;
  u32                   colors[UI_Color_MAX];
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

internal void ui_context_init(UI_Context *ctx, ui_measure_text_proc measure_text_proc, isize width, isize height, isize text_height, Allocator allocator) {
  isize n_cells = ((width  + UI_HASH_CELL_SIZE - 1) / UI_HASH_CELL_SIZE) *
                  ((height + UI_HASH_CELL_SIZE - 1) / UI_HASH_CELL_SIZE);

  slice_init(&ctx->command_hashes,      n_cells, allocator);
  slice_init(&ctx->prev_command_hashes, n_cells, allocator);

  vector_init(&ctx->commands, 0, 8, allocator);
  vector_init(&ctx->images,   0, 8, allocator);

  ctx->measure_text_proc = measure_text_proc;
  ctx->width             = width;
  ctx->height            = height;
  ctx->text_height       = text_height;
  ctx->spacing           = 10;

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
  vector_delete(ctx->images);
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
  i32 width = ctx->measure_text_proc(text);
  UI_Command cmd;
  Rectangle rect = ui_insert_rect(
    ctx,
    width + ctx->spacing * 2,
    ctx->text_height + ctx->spacing * 2
  );

  UI_Color color         = UI_Color_Button;
  UI_Color color_2       = UI_Color_Button_2;
  UI_Color text_color    = UI_Color_Button_Text;
  UI_Color outline_color = UI_Color_Button_Outline;
  
  if (ui_mouse_in_rect(ctx, &rect)) {
    if (ctx->mouse.buttons[0]) {
      GB_STATIC_ASSERT(size_of(i32) == size_of(UI_Color));
      *(i32 *)&color         += 8;
      *(i32 *)&color_2       += 8;
      *(i32 *)&text_color    += 8;
      *(i32 *)&outline_color += 8;
    } else {
      *(i32 *)&color         += 4;
      *(i32 *)&color_2       += 4;
      *(i32 *)&text_color    += 4;
      *(i32 *)&outline_color += 4;
    }
  }

  cmd.type        = UI_Command_Type_Gradient;
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
  i32 width = ctx->measure_text_proc(text);
  UI_Command cmd;
  Rectangle rect = ui_insert_rect(
    ctx,
    width + ctx->spacing * 2,
    ctx->text_height + ctx->spacing * 2
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

  assert(image.components == 4);
  assert(image.pixel_type == PT_u8);

  vector_append(&ctx->images, image);
  return ret;
}

internal void ui_image(UI_Context *ctx, UI_Image img) {
  UI_Command cmd;
  Image *image = &ctx->images.data[img.index];
  Rectangle rect = ui_insert_rect(ctx, (i32)image->width, (i32)image->height);

  assert(image->components == 4);
  assert(image->pixel_type == PT_u8);

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
