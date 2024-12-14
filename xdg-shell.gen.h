// Generated from XML
#include "codin.h"
#include "wayland_gen_common.h"

typedef enum {
	Wayland_Xdg_Wm_Base_Error_Role = 0,
	Wayland_Xdg_Wm_Base_Error_Defunct_Surfaces = 1,
	Wayland_Xdg_Wm_Base_Error_Not_The_Topmost_Popup = 2,
	Wayland_Xdg_Wm_Base_Error_Invalid_Popup_Parent = 3,
	Wayland_Xdg_Wm_Base_Error_Invalid_Surface_State = 4,
	Wayland_Xdg_Wm_Base_Error_Invalid_Positioner = 5,
	Wayland_Xdg_Wm_Base_Error_Unresponsive = 6,
} Wayland_Xdg_Wm_Base_Error;

typedef enum {
	Wayland_Xdg_Positioner_Error_Invalid_Input = 0,
} Wayland_Xdg_Positioner_Error;

typedef enum {
	Wayland_Xdg_Positioner_Anchor_None = 0,
	Wayland_Xdg_Positioner_Anchor_Top = 1,
	Wayland_Xdg_Positioner_Anchor_Bottom = 2,
	Wayland_Xdg_Positioner_Anchor_Left = 3,
	Wayland_Xdg_Positioner_Anchor_Right = 4,
	Wayland_Xdg_Positioner_Anchor_Top_Left = 5,
	Wayland_Xdg_Positioner_Anchor_Bottom_Left = 6,
	Wayland_Xdg_Positioner_Anchor_Top_Right = 7,
	Wayland_Xdg_Positioner_Anchor_Bottom_Right = 8,
} Wayland_Xdg_Positioner_Anchor;

typedef enum {
	Wayland_Xdg_Positioner_Gravity_None = 0,
	Wayland_Xdg_Positioner_Gravity_Top = 1,
	Wayland_Xdg_Positioner_Gravity_Bottom = 2,
	Wayland_Xdg_Positioner_Gravity_Left = 3,
	Wayland_Xdg_Positioner_Gravity_Right = 4,
	Wayland_Xdg_Positioner_Gravity_Top_Left = 5,
	Wayland_Xdg_Positioner_Gravity_Bottom_Left = 6,
	Wayland_Xdg_Positioner_Gravity_Top_Right = 7,
	Wayland_Xdg_Positioner_Gravity_Bottom_Right = 8,
} Wayland_Xdg_Positioner_Gravity;

typedef enum {
	Wayland_Xdg_Positioner_Constraint_Adjustment_None = 0,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Slide_X = 1,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Slide_Y = 2,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Flip_X = 4,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Flip_Y = 8,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Resize_X = 16,
	Wayland_Xdg_Positioner_Constraint_Adjustment_Resize_Y = 32,
} Wayland_Xdg_Positioner_Constraint_Adjustment;

typedef enum {
	Wayland_Xdg_Surface_Error_Not_Constructed = 1,
	Wayland_Xdg_Surface_Error_Already_Constructed = 2,
	Wayland_Xdg_Surface_Error_Unconfigured_Buffer = 3,
	Wayland_Xdg_Surface_Error_Invalid_Serial = 4,
	Wayland_Xdg_Surface_Error_Invalid_Size = 5,
	Wayland_Xdg_Surface_Error_Defunct_Role_Object = 6,
} Wayland_Xdg_Surface_Error;

typedef enum {
	Wayland_Xdg_Toplevel_Error_Invalid_Resize_Edge = 0,
	Wayland_Xdg_Toplevel_Error_Invalid_Parent = 1,
	Wayland_Xdg_Toplevel_Error_Invalid_Size = 2,
} Wayland_Xdg_Toplevel_Error;

typedef enum {
	Wayland_Xdg_Toplevel_Resize_Edge_None = 0,
	Wayland_Xdg_Toplevel_Resize_Edge_Top = 1,
	Wayland_Xdg_Toplevel_Resize_Edge_Bottom = 2,
	Wayland_Xdg_Toplevel_Resize_Edge_Left = 4,
	Wayland_Xdg_Toplevel_Resize_Edge_Top_Left = 5,
	Wayland_Xdg_Toplevel_Resize_Edge_Bottom_Left = 6,
	Wayland_Xdg_Toplevel_Resize_Edge_Right = 8,
	Wayland_Xdg_Toplevel_Resize_Edge_Top_Right = 9,
	Wayland_Xdg_Toplevel_Resize_Edge_Bottom_Right = 10,
} Wayland_Xdg_Toplevel_Resize_Edge;

typedef enum {
	Wayland_Xdg_Toplevel_State_Maximized = 1,
	Wayland_Xdg_Toplevel_State_Fullscreen = 2,
	Wayland_Xdg_Toplevel_State_Resizing = 3,
	Wayland_Xdg_Toplevel_State_Activated = 4,
	Wayland_Xdg_Toplevel_State_Tiled_Left = 5,
	Wayland_Xdg_Toplevel_State_Tiled_Right = 6,
	Wayland_Xdg_Toplevel_State_Tiled_Top = 7,
	Wayland_Xdg_Toplevel_State_Tiled_Bottom = 8,
	Wayland_Xdg_Toplevel_State_Suspended = 9,
} Wayland_Xdg_Toplevel_State;

typedef enum {
	Wayland_Xdg_Toplevel_Wm_Capabilities_Window_Menu = 1,
	Wayland_Xdg_Toplevel_Wm_Capabilities_Maximize = 2,
	Wayland_Xdg_Toplevel_Wm_Capabilities_Fullscreen = 3,
	Wayland_Xdg_Toplevel_Wm_Capabilities_Minimize = 4,
} Wayland_Xdg_Toplevel_Wm_Capabilities;

typedef enum {
	Wayland_Xdg_Popup_Error_Invalid_Grab = 0,
} Wayland_Xdg_Popup_Error;

internal void wayland_xdg_wm_base_destroy(Wayland_Connection *wc, u32 xdg_wm_base) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.destroy:"), xdg_wm_base);
}

internal u32 wayland_xdg_wm_base_create_positioner(Wayland_Connection *wc, u32 xdg_wm_base) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.create_positioner:"), xdg_wm_base);
	return return_value;
}

internal u32 wayland_xdg_wm_base_get_xdg_surface(Wayland_Connection *wc, u32 xdg_wm_base, u32 surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &surface);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.get_xdg_surface:"), xdg_wm_base);
	return return_value;
}

internal void wayland_xdg_wm_base_pong(Wayland_Connection *wc, u32 xdg_wm_base, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.pong:"), xdg_wm_base);
}

internal void wayland_xdg_positioner_destroy(Wayland_Connection *wc, u32 xdg_positioner) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_positioner@%d.destroy:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_size(Wayland_Connection *wc, u32 xdg_positioner, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_size:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_anchor_rect(Wayland_Connection *wc, u32 xdg_positioner, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_anchor_rect:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_anchor(Wayland_Connection *wc, u32 xdg_positioner, Wayland_Xdg_Positioner_Anchor anchor) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	i32 anchor_value = (i32)anchor;
	write_any(w, &anchor_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_anchor:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_gravity(Wayland_Connection *wc, u32 xdg_positioner, Wayland_Xdg_Positioner_Gravity gravity) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	i32 gravity_value = (i32)gravity;
	write_any(w, &gravity_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_gravity:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_constraint_adjustment(Wayland_Connection *wc, u32 xdg_positioner, Wayland_Xdg_Positioner_Constraint_Adjustment constraint_adjustment) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	i32 constraint_adjustment_value = (i32)constraint_adjustment;
	write_any(w, &constraint_adjustment_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_constraint_adjustment:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_offset(Wayland_Connection *wc, u32 xdg_positioner, i32 x, i32 y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_offset:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_reactive(Wayland_Connection *wc, u32 xdg_positioner) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_reactive:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_parent_size(Wayland_Connection *wc, u32 xdg_positioner, i32 parent_width, i32 parent_height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 8;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &parent_width);
	write_any(w, &parent_height);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_parent_size:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_parent_configure(Wayland_Connection *wc, u32 xdg_positioner, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_parent_configure:"), xdg_positioner);
}

internal void wayland_xdg_surface_destroy(Wayland_Connection *wc, u32 xdg_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_surface@%d.destroy:"), xdg_surface);
}

internal u32 wayland_xdg_surface_get_toplevel(Wayland_Connection *wc, u32 xdg_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> xdg_surface@%d.get_toplevel:"), xdg_surface);
	return return_value;
}

internal u32 wayland_xdg_surface_get_popup(Wayland_Connection *wc, u32 xdg_surface, u32 parent, u32 positioner) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &parent);
	write_any(w, &positioner);

	wayland_log_infof(LIT("-> xdg_surface@%d.get_popup:"), xdg_surface);
	return return_value;
}

internal void wayland_xdg_surface_set_window_geometry(Wayland_Connection *wc, u32 xdg_surface, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_surface@%d.set_window_geometry:"), xdg_surface);
}

internal void wayland_xdg_surface_ack_configure(Wayland_Connection *wc, u32 xdg_surface, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_surface@%d.ack_configure:"), xdg_surface);
}

internal void wayland_xdg_toplevel_destroy(Wayland_Connection *wc, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.destroy:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_parent(Wayland_Connection *wc, u32 xdg_toplevel, u32 parent) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &parent);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_parent:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_title(Wayland_Connection *wc, u32 xdg_toplevel, String title) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 12;
	isize title_size = roundup_4(title.len + 1);
	_msg_size += title_size;

	write_any(w, &_msg_size);
	i32 title_write_len = (i32)title.len + 1;
	write_any(w, &title_write_len);
	write_string(w, title);
	for_range(i, title.len, title_size) {
		write_byte(w, 0);
	}

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_title:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_app_id(Wayland_Connection *wc, u32 xdg_toplevel, String app_id) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;
	isize app_id_size = roundup_4(app_id.len + 1);
	_msg_size += app_id_size;

	write_any(w, &_msg_size);
	i32 app_id_write_len = (i32)app_id.len + 1;
	write_any(w, &app_id_write_len);
	write_string(w, app_id);
	for_range(i, app_id.len, app_id_size) {
		write_byte(w, 0);
	}

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_app_id:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_show_window_menu(Wayland_Connection *wc, u32 xdg_toplevel, u32 seat, u32 serial, i32 x, i32 y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.show_window_menu:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_move(Wayland_Connection *wc, u32 xdg_toplevel, u32 seat, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.move:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_resize(Wayland_Connection *wc, u32 xdg_toplevel, u32 seat, u32 serial, Wayland_Xdg_Toplevel_Resize_Edge edges) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);
	i32 edges_value = (i32)edges;
	write_any(w, &edges_value);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.resize:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_max_size(Wayland_Connection *wc, u32 xdg_toplevel, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_max_size:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_min_size(Wayland_Connection *wc, u32 xdg_toplevel, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 8;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_min_size:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_maximized(Wayland_Connection *wc, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_maximized:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_unset_maximized(Wayland_Connection *wc, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 10;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.unset_maximized:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_fullscreen(Wayland_Connection *wc, u32 xdg_toplevel, u32 output) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 11;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &output);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_fullscreen:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_unset_fullscreen(Wayland_Connection *wc, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 12;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.unset_fullscreen:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_minimized(Wayland_Connection *wc, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 13;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_minimized:"), xdg_toplevel);
}

internal void wayland_xdg_popup_destroy(Wayland_Connection *wc, u32 xdg_popup) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_popup@%d.destroy:"), xdg_popup);
}

internal void wayland_xdg_popup_grab(Wayland_Connection *wc, u32 xdg_popup, u32 seat, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_popup@%d.grab:"), xdg_popup);
}

internal void wayland_xdg_popup_reposition(Wayland_Connection *wc, u32 xdg_popup, u32 positioner, u32 token) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &positioner);
	write_any(w, &token);

	wayland_log_infof(LIT("-> xdg_popup@%d.reposition:"), xdg_popup);
}

