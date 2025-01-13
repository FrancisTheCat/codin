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
  i32       x, y, length;
  b32       vertical;
  u32       color;
} UI_Command_Line;

typedef struct {
  String    text;
  Rectangle bounds;
  i32       size;
  u32       color;
} UI_Command_Text;

typedef struct {
  i32 index;
} UI_Image;

typedef enum {
  UI_Cursor_Default = 0,
  UI_Cursor_Pointer,
  UI_Cursor_Text,
} UI_Cursor;

typedef struct {
  Rectangle rect;
  u32       tint;
  u32       outline;
  UI_Image  image;
} UI_Command_Image;

#define UI_COMMAND_TYPES(X)                                                    \
  X(UI_Command_Type_None)                                                      \
  X(UI_Command_Type_Box)                                                       \
  X(UI_Command_Type_Gradient)                                                  \
  X(UI_Command_Type_Line)                                                       \
  X(UI_Command_Type_Text)                                                      \
  X(UI_Command_Type_Image)

X_ENUM(UI_Command_Type, UI_COMMAND_TYPES)

typedef struct {
  union {
    UI_Command_Box   box;
    UI_Command_Text  text;
    UI_Command_Image image;
    UI_Command_Line  line;
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
  case UI_Command_Type_Line:
    *rect = (Rectangle) {.x0 = command->variant.line.x, .y0 = command->variant.line.y};
    if (command->variant.line.vertical) {
      rect->x1 = rect->x0;
      rect->y1 = rect->y0 + command->variant.line.length;
    } else {
      rect->y1 = rect->y0;
      rect->x1 = rect->x0 + command->variant.line.length;
    }
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
  case UI_Command_Type_Line:
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

#define RECT_CUT_SIDES(X)                                                      \
  X(Rect_Cut_Side_Left)                                                        \
  X(Rect_Cut_Side_Right)                                                       \
  X(Rect_Cut_Side_Top)                                                         \
  X(Rect_Cut_Side_Bottom)                                                      \

X_ENUM(Rect_Cut_Side, RECT_CUT_SIDES)

internal Rectangle cut_left(Rectangle* rect, i32 a) {
  i32 x0 = rect->x0;
  rect->x0 = min(rect->x1, rect->x0 + a);
  Rectangle ret = { x0, rect->y0, rect->x0, rect->y1 };
  rect->x0 += 10;
  return ret;
}

internal Rectangle cut_right(Rectangle* rect, i32 a) {
  i32 x1 = rect->x1;
  rect->x1 = max(rect->x0, rect->x1 - a);
  Rectangle ret = { rect->x1, rect->y0, x1, rect->y1 };
  rect->x1 -= 10;
  return ret;
}

internal Rectangle cut_top(Rectangle* rect, i32 a) {
  i32 y0 = rect->y0;
  rect->y0 = min(rect->y1, rect->y0 + a);
  Rectangle ret = { rect->x0, y0, rect->x1, rect->y0 };
  rect->y0 += 10;
  return ret;
}

internal Rectangle cut_bottom(Rectangle* rect, i32 a) {
  i32 y1 = rect->y1;
  rect->y1 = max(rect->y0, rect->y1 - a);
  Rectangle ret = { rect->x0, rect->y1, rect->x1, y1 };
  rect->y1 -= 10;
  return ret;
}

typedef struct {
  Rectangle*    rect;
  Rect_Cut_Side side;
} Rect_Cut;

internal Rect_Cut rectcut(Rectangle* rect, Rect_Cut_Side side) {
    return (Rect_Cut) {
        .rect = rect,
        .side = side
    };
}

internal Rectangle rectcut_cut(Rect_Cut rectcut, i32 x, i32 y) {
  Rectangle ret;
  switch (rectcut.side) {
    case Rect_Cut_Side_Left:   
      ret = cut_left(rectcut.rect,   x);
      break;
    case Rect_Cut_Side_Right:  
      ret = cut_right(rectcut.rect,  x);
      break;
    case Rect_Cut_Side_Top:    
      ret = cut_top(rectcut.rect,    y);
      break;
    case Rect_Cut_Side_Bottom: 
      ret = cut_bottom(rectcut.rect, y);
      break;
    default:
      unreachable();
  }
  return ret;
}

typedef struct {
  Rectangle     rect;
  Rect_Cut_Side side;
} UI_Layout;

#define UI_COLORS(X)                                                           \
  X(UI_Color_Background)                                                       \
  X(UI_Color_Separator)                                                        \
                                                                               \
  X(UI_Color_Image_Border)                                                     \
                                                                               \
  X(UI_Color_Label)                                                            \
  X(UI_Color_Label_Text)                                                       \
  X(UI_Color_Label_Outline)                                                    \
                                                                               \
  X(UI_Color_Button)                                                           \
  X(UI_Color_Button_2)                                                         \
  X(UI_Color_Button_Text)                                                      \
  X(UI_Color_Button_Outline)                                                   \
                                                                               \
  X(UI_Color_Button_Hovered)                                                   \
  X(UI_Color_Button_Hovered_2)                                                 \
  X(UI_Color_Button_Hovered_Text)                                              \
  X(UI_Color_Button_Hovered_Outline)                                           \
                                                                               \
  X(UI_Color_Button_Clicked)                                                   \
  X(UI_Color_Button_Clicked_2)                                                 \
  X(UI_Color_Button_Clicked_Text)                                              \
  X(UI_Color_Button_Clicked_Outline)                                           \

X_ENUM(UI_Color, UI_COLORS)

typedef isize (*ui_measure_text_proc)(String str);

typedef struct {
  Vector(UI_Command)    commands;
  Vector(UI_Layout)     layouts;
  i32                   spacing;
  Rectangle             rect;
  Rect_Cut_Side         side;
  struct {
    i32 x, y;
    b8  buttons[2];
  } mouse;
  ui_measure_text_proc  measure_text_proc;
  i32                   text_height;
  Vector(Image)         images;
  UI_Cursor             cursor;
  u32                   colors[enum_len(UI_Color)];
} UI_Context;

internal Rectangle ui_spacing(UI_Context *ctx, isize a) {
  return rectcut_cut((Rect_Cut) {.rect = &ctx->rect, .side = ctx->side}, a, a);
}

internal Rectangle ui_insert_rect(UI_Context *ctx, isize width, isize height) {
  // Rectangle rect;
  // rect.x0 = ctx->x;
  // rect.y0 = ctx->y;

  return rectcut_cut((Rect_Cut) {.rect = &ctx->rect, .side = ctx->side}, width, height);

  // if (ctx->horizontal) {
  //   rect.x1 = ctx->x + width;
  //   rect.y1 = ctx->y + height;
  //   ctx->x += width + ctx->spacing;
  //   return rect;
  // } else {
  //   rect.x1 = ctx->x + width;
  //   rect.y1 = ctx->y + height;
  //   ctx->y += height + ctx->spacing;
  //   return rect;
  // }
}

internal void ui_context_init(UI_Context *ctx, ui_measure_text_proc measure_text_proc, isize width, isize height, isize text_height, Allocator allocator) {
  vector_init(&ctx->commands, 0, 8, allocator);
  vector_init(&ctx->images,   0, 8, allocator);

  ctx->measure_text_proc = measure_text_proc;
  ctx->text_height       = text_height;
  ctx->spacing           = 2 * text_height / 3;

  ctx->colors[UI_Color_Background            ] = 0xFF1E2128;
  ctx->colors[UI_Color_Separator             ] = 0x22FFFFFF;

  ctx->colors[UI_Color_Image_Border          ] = 0xFF62AEEF;

  ctx->colors[UI_Color_Label                 ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Label_Text            ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Label_Outline         ] = 0xFFABB2BF;

  ctx->colors[UI_Color_Button                ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Button_2              ] = 0x22FFFFFF;
  ctx->colors[UI_Color_Button_Text           ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Outline        ] = 0xFF62AEEF;

  ctx->colors[UI_Color_Button_Hovered        ] = 0x33FFFFFF;
  ctx->colors[UI_Color_Button_Hovered_2      ] = 0x33FFFFFF;
  ctx->colors[UI_Color_Button_Hovered_Text   ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Hovered_Outline] = 0xFFE06B74;

  ctx->colors[UI_Color_Button_Clicked        ] = 0x44FFFFFF;
  ctx->colors[UI_Color_Button_Clicked_2      ] = 0x44FFFFFF;
  ctx->colors[UI_Color_Button_Clicked_Text   ] = 0xFFABB2BF;
  ctx->colors[UI_Color_Button_Clicked_Outline] = 0xFFE06B74;
}

internal void ui_context_destroy(UI_Context *ctx, Allocator allocator) {
  vector_delete(ctx->commands);
  vector_delete(ctx->layouts);
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

internal void ui_layout_begin(UI_Context *ctx, isize a, Rect_Cut_Side side) {
  UI_Layout layout;

  Rectangle tmp = ui_insert_rect(ctx, a, a);

  layout.side = ctx->side;
  layout.rect = ctx->rect;

  UI_Command cmd;
  cmd.type = UI_Command_Type_Line;
  UI_Command_Line line;
  line.color = ctx->colors[UI_Color_Separator];

  switch (ctx->side) {
  case Rect_Cut_Side_Left:
    line.x = tmp.x1 + ctx->spacing;
    line.y = tmp.y0;
    layout.rect.x0 += ctx->spacing;
    break;
  case Rect_Cut_Side_Right:
    line.x = tmp.x0 - ctx->spacing;
    line.y = tmp.y0;
    layout.rect.x1 -= ctx->spacing;
    break;
  case Rect_Cut_Side_Top:
    line.x = tmp.x0;
    line.y = tmp.y1 + ctx->spacing;
    layout.rect.y0 += ctx->spacing;
    break;
  case Rect_Cut_Side_Bottom:
    line.y = tmp.y0 - ctx->spacing;
    line.x = tmp.x0;
    layout.rect.y1 -= ctx->spacing;
    break;
  }

  switch (ctx->side) {
  case Rect_Cut_Side_Left:
  case Rect_Cut_Side_Right:
    line.vertical = true;
    line.length   = tmp.y1 - tmp.y0;
    break;
  case Rect_Cut_Side_Top:
  case Rect_Cut_Side_Bottom:
    line.vertical = false;
    line.vertical = false;
    line.length   = tmp.x1 - tmp.x0;
    break;
  }
  cmd.variant.line = line;
  vector_append(&ctx->commands, cmd);

  ctx->rect = tmp;
  ctx->side = side;

  vector_append(&ctx->layouts, layout);
}

internal void ui_layout_end(UI_Context *ctx) {
  UI_Layout layout = vector_pop(&ctx->layouts);
  ctx->rect = layout.rect;
  ctx->side = layout.side;
}

internal void ui_reset(UI_Context *ctx) {
  vector_clear(&ctx->commands);
  vector_clear(&ctx->layouts);

  ctx->side   = Rect_Cut_Side_Top;
  ctx->cursor = UI_Cursor_Default;
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
    ctx->cursor = UI_Cursor_Pointer;

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

  if (ui_mouse_in_rect(ctx, &rect)) {
    ctx->cursor = UI_Cursor_Text;
  }


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

// internal b8 button(UI_Context *ctx, Rect_Cut layout, String text) {
//     i32 size = ctx->measure_text_proc(text);
//     Rectangle rect = rectcut_cut(layout, size + ctx->spacing * 2, ctx->text_height + ctx->spacing * 2);
//     UI_Command cmd;
//     cmd.type        = UI_Command_Type_Gradient;
//     cmd.variant.box = (UI_Command_Box) {
//       .rect    = rect,
//       .color   = ctx->colors[UI_Color_Button],
//       .color2  = ctx->colors[UI_Color_Button],
//       .outline = ctx->colors[UI_Color_Button_Outline],
//     };
//     vector_append(&ctx->commands, cmd);

//     cmd.type        = UI_Command_Type_Text;
//     cmd.variant.text = (UI_Command_Text) {
//       .text   = text,
//       .bounds = rect,
//       .color  = ctx->colors[UI_Color_Button_Text],
//     };
//     vector_append(&ctx->commands, cmd);

//     return ui_mouse_in_rect(ctx, &rect) && ctx->mouse.buttons[0];
//     // interactions
//     // draw
// }

internal Rectangle rect_intersection(Rectangle a, Rectangle b) {
  return (Rectangle) {
    .x0 = max(a.x0, b.x0),
    .y0 = max(a.y0, b.y0),
    .x1 = min(a.x1, b.x1),
    .y1 = min(a.y1, b.y1),
  };
}
