// Generated from XML

#include "codin.h"
#include "wayland_gen_common.h"

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Wm_Base_Error_Role, 0)\
	X(Wayland_Xdg_Wm_Base_Error_Defunct_Surfaces, 1)\
	X(Wayland_Xdg_Wm_Base_Error_Not_The_Topmost_Popup, 2)\
	X(Wayland_Xdg_Wm_Base_Error_Invalid_Popup_Parent, 3)\
	X(Wayland_Xdg_Wm_Base_Error_Invalid_Surface_State, 4)\
	X(Wayland_Xdg_Wm_Base_Error_Invalid_Positioner, 5)\
	X(Wayland_Xdg_Wm_Base_Error_Unresponsive, 6)\

X_ENUM_EXPLICIT(Wayland_Xdg_Wm_Base_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Positioner_Error_Invalid_Input, 0)\

X_ENUM_EXPLICIT(Wayland_Xdg_Positioner_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Positioner_Anchor_None, 0)\
	X(Wayland_Xdg_Positioner_Anchor_Top, 1)\
	X(Wayland_Xdg_Positioner_Anchor_Bottom, 2)\
	X(Wayland_Xdg_Positioner_Anchor_Left, 3)\
	X(Wayland_Xdg_Positioner_Anchor_Right, 4)\
	X(Wayland_Xdg_Positioner_Anchor_Top_Left, 5)\
	X(Wayland_Xdg_Positioner_Anchor_Bottom_Left, 6)\
	X(Wayland_Xdg_Positioner_Anchor_Top_Right, 7)\
	X(Wayland_Xdg_Positioner_Anchor_Bottom_Right, 8)\

X_ENUM_EXPLICIT(Wayland_Xdg_Positioner_Anchor, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Positioner_Gravity_None, 0)\
	X(Wayland_Xdg_Positioner_Gravity_Top, 1)\
	X(Wayland_Xdg_Positioner_Gravity_Bottom, 2)\
	X(Wayland_Xdg_Positioner_Gravity_Left, 3)\
	X(Wayland_Xdg_Positioner_Gravity_Right, 4)\
	X(Wayland_Xdg_Positioner_Gravity_Top_Left, 5)\
	X(Wayland_Xdg_Positioner_Gravity_Bottom_Left, 6)\
	X(Wayland_Xdg_Positioner_Gravity_Top_Right, 7)\
	X(Wayland_Xdg_Positioner_Gravity_Bottom_Right, 8)\

X_ENUM_EXPLICIT(Wayland_Xdg_Positioner_Gravity, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_None, 0)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Slide_X, 1)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Slide_Y, 2)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Flip_X, 4)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Flip_Y, 8)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Resize_X, 16)\
	X(Wayland_Xdg_Positioner_Constraint_Adjustment_Resize_Y, 32)\

X_ENUM_EXPLICIT(Wayland_Xdg_Positioner_Constraint_Adjustment, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Surface_Error_Not_Constructed, 1)\
	X(Wayland_Xdg_Surface_Error_Already_Constructed, 2)\
	X(Wayland_Xdg_Surface_Error_Unconfigured_Buffer, 3)\
	X(Wayland_Xdg_Surface_Error_Invalid_Serial, 4)\
	X(Wayland_Xdg_Surface_Error_Invalid_Size, 5)\
	X(Wayland_Xdg_Surface_Error_Defunct_Role_Object, 6)\

X_ENUM_EXPLICIT(Wayland_Xdg_Surface_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Toplevel_Error_Invalid_Resize_Edge, 0)\
	X(Wayland_Xdg_Toplevel_Error_Invalid_Parent, 1)\
	X(Wayland_Xdg_Toplevel_Error_Invalid_Size, 2)\

X_ENUM_EXPLICIT(Wayland_Xdg_Toplevel_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Toplevel_Resize_Edge_None, 0)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Top, 1)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Bottom, 2)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Left, 4)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Top_Left, 5)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Bottom_Left, 6)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Right, 8)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Top_Right, 9)\
	X(Wayland_Xdg_Toplevel_Resize_Edge_Bottom_Right, 10)\

X_ENUM_EXPLICIT(Wayland_Xdg_Toplevel_Resize_Edge, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Toplevel_State_Maximized, 1)\
	X(Wayland_Xdg_Toplevel_State_Fullscreen, 2)\
	X(Wayland_Xdg_Toplevel_State_Resizing, 3)\
	X(Wayland_Xdg_Toplevel_State_Activated, 4)\
	X(Wayland_Xdg_Toplevel_State_Tiled_Left, 5)\
	X(Wayland_Xdg_Toplevel_State_Tiled_Right, 6)\
	X(Wayland_Xdg_Toplevel_State_Tiled_Top, 7)\
	X(Wayland_Xdg_Toplevel_State_Tiled_Bottom, 8)\
	X(Wayland_Xdg_Toplevel_State_Suspended, 9)\

X_ENUM_EXPLICIT(Wayland_Xdg_Toplevel_State, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Toplevel_Wm_Capabilities_Window_Menu, 1)\
	X(Wayland_Xdg_Toplevel_Wm_Capabilities_Maximize, 2)\
	X(Wayland_Xdg_Toplevel_Wm_Capabilities_Fullscreen, 3)\
	X(Wayland_Xdg_Toplevel_Wm_Capabilities_Minimize, 4)\

X_ENUM_EXPLICIT(Wayland_Xdg_Toplevel_Wm_Capabilities, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Xdg_Popup_Error_Invalid_Grab, 0)\

X_ENUM_EXPLICIT(Wayland_Xdg_Popup_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

internal void wayland_xdg_wm_base_destroy(Wayland_Connection *conn, u32 xdg_wm_base) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.destroy:"), xdg_wm_base);
}

internal u32 wayland_xdg_wm_base_create_positioner(Wayland_Connection *conn, u32 xdg_wm_base) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.create_positioner: id=%d"), xdg_wm_base, return_value);
	return return_value;
}

internal u32 wayland_xdg_wm_base_get_xdg_surface(Wayland_Connection *conn, u32 xdg_wm_base, u32 surface) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);
	write_any(w, &surface);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.get_xdg_surface: id=%d surface=%d"), xdg_wm_base, return_value, surface);
	return return_value;
}

internal void wayland_xdg_wm_base_pong(Wayland_Connection *conn, u32 xdg_wm_base, u32 serial) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_wm_base);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_wm_base@%d.pong: serial=%d"), xdg_wm_base, serial);
}

typedef enum {
	Wayland_Xdg_Wm_Base_Event_Ping = 0,
} Wayland_Xdg_Wm_Base_Event;

internal isize wayland_parse_event_xdg_wm_base_ping(Wayland_Connection *conn, u32 *serial) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, serial), -1);
	wayland_log_infof(LIT("<- xdg_wm_base@%d.ping: serial=%d"), _object_id, *serial);
	return _msg_size;
}

internal void wayland_xdg_positioner_destroy(Wayland_Connection *conn, u32 xdg_positioner) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_positioner@%d.destroy:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_size(Wayland_Connection *conn, u32 xdg_positioner, i32 width, i32 height) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_size: width=%d height=%d"), xdg_positioner, width, height);
}

internal void wayland_xdg_positioner_set_anchor_rect(Wayland_Connection *conn, u32 xdg_positioner, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&conn->builder);
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

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_anchor_rect: x=%d y=%d width=%d height=%d"), xdg_positioner, x, y, width, height);
}

internal void wayland_xdg_positioner_set_anchor(Wayland_Connection *conn, u32 xdg_positioner, Wayland_Xdg_Positioner_Anchor anchor) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 anchor_value = (u32)anchor;
	write_any(w, &anchor_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_anchor: anchor=%S"), xdg_positioner, enum_to_string(Wayland_Xdg_Positioner_Anchor, anchor));
}

internal void wayland_xdg_positioner_set_gravity(Wayland_Connection *conn, u32 xdg_positioner, Wayland_Xdg_Positioner_Gravity gravity) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 gravity_value = (u32)gravity;
	write_any(w, &gravity_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_gravity: gravity=%S"), xdg_positioner, enum_to_string(Wayland_Xdg_Positioner_Gravity, gravity));
}

internal void wayland_xdg_positioner_set_constraint_adjustment(Wayland_Connection *conn, u32 xdg_positioner, Wayland_Xdg_Positioner_Constraint_Adjustment constraint_adjustment) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 constraint_adjustment_value = (u32)constraint_adjustment;
	write_any(w, &constraint_adjustment_value);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_constraint_adjustment: constraint_adjustment=%S"), xdg_positioner, enum_to_string(Wayland_Xdg_Positioner_Constraint_Adjustment, constraint_adjustment));
}

internal void wayland_xdg_positioner_set_offset(Wayland_Connection *conn, u32 xdg_positioner, i32 x, i32 y) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_offset: x=%d y=%d"), xdg_positioner, x, y);
}

internal void wayland_xdg_positioner_set_reactive(Wayland_Connection *conn, u32 xdg_positioner) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_reactive:"), xdg_positioner);
}

internal void wayland_xdg_positioner_set_parent_size(Wayland_Connection *conn, u32 xdg_positioner, i32 parent_width, i32 parent_height) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 8;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &parent_width);
	write_any(w, &parent_height);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_parent_size: parent_width=%d parent_height=%d"), xdg_positioner, parent_width, parent_height);
}

internal void wayland_xdg_positioner_set_parent_configure(Wayland_Connection *conn, u32 xdg_positioner, u32 serial) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_positioner);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_positioner@%d.set_parent_configure: serial=%d"), xdg_positioner, serial);
}

internal void wayland_xdg_surface_destroy(Wayland_Connection *conn, u32 xdg_surface) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_surface@%d.destroy:"), xdg_surface);
}

internal u32 wayland_xdg_surface_get_toplevel(Wayland_Connection *conn, u32 xdg_surface) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> xdg_surface@%d.get_toplevel: id=%d"), xdg_surface, return_value);
	return return_value;
}

internal u32 wayland_xdg_surface_get_popup(Wayland_Connection *conn, u32 xdg_surface, u32 parent, u32 positioner) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);
	write_any(w, &parent);
	write_any(w, &positioner);

	wayland_log_infof(LIT("-> xdg_surface@%d.get_popup: id=%d parent=%d positioner=%d"), xdg_surface, return_value, parent, positioner);
	return return_value;
}

internal void wayland_xdg_surface_set_window_geometry(Wayland_Connection *conn, u32 xdg_surface, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&conn->builder);
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

	wayland_log_infof(LIT("-> xdg_surface@%d.set_window_geometry: x=%d y=%d width=%d height=%d"), xdg_surface, x, y, width, height);
}

internal void wayland_xdg_surface_ack_configure(Wayland_Connection *conn, u32 xdg_surface, u32 serial) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_surface);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_surface@%d.ack_configure: serial=%d"), xdg_surface, serial);
}

typedef enum {
	Wayland_Xdg_Surface_Event_Configure = 0,
} Wayland_Xdg_Surface_Event;

internal isize wayland_parse_event_xdg_surface_configure(Wayland_Connection *conn, u32 *serial) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, serial), -1);
	wayland_log_infof(LIT("<- xdg_surface@%d.configure: serial=%d"), _object_id, *serial);
	return _msg_size;
}

internal void wayland_xdg_toplevel_destroy(Wayland_Connection *conn, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.destroy:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_parent(Wayland_Connection *conn, u32 xdg_toplevel, u32 parent) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &parent);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_parent: parent=%d"), xdg_toplevel, parent);
}

internal void wayland_xdg_toplevel_set_title(Wayland_Connection *conn, u32 xdg_toplevel, String title) {
	Writer _w = writer_from_builder(&conn->builder);
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

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_title: title=%S"), xdg_toplevel, title);
}

internal void wayland_xdg_toplevel_set_app_id(Wayland_Connection *conn, u32 xdg_toplevel, String app_id) {
	Writer _w = writer_from_builder(&conn->builder);
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

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_app_id: app_id=%S"), xdg_toplevel, app_id);
}

internal void wayland_xdg_toplevel_show_window_menu(Wayland_Connection *conn, u32 xdg_toplevel, u32 seat, u32 serial, i32 x, i32 y) {
	Writer _w = writer_from_builder(&conn->builder);
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

	wayland_log_infof(LIT("-> xdg_toplevel@%d.show_window_menu: seat=%d serial=%d x=%d y=%d"), xdg_toplevel, seat, serial, x, y);
}

internal void wayland_xdg_toplevel_move(Wayland_Connection *conn, u32 xdg_toplevel, u32 seat, u32 serial) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.move: seat=%d serial=%d"), xdg_toplevel, seat, serial);
}

internal void wayland_xdg_toplevel_resize(Wayland_Connection *conn, u32 xdg_toplevel, u32 seat, u32 serial, Wayland_Xdg_Toplevel_Resize_Edge edges) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);
	u32 edges_value = (u32)edges;
	write_any(w, &edges_value);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.resize: seat=%d serial=%d edges=%S"), xdg_toplevel, seat, serial, enum_to_string(Wayland_Xdg_Toplevel_Resize_Edge, edges));
}

internal void wayland_xdg_toplevel_set_max_size(Wayland_Connection *conn, u32 xdg_toplevel, i32 width, i32 height) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_max_size: width=%d height=%d"), xdg_toplevel, width, height);
}

internal void wayland_xdg_toplevel_set_min_size(Wayland_Connection *conn, u32 xdg_toplevel, i32 width, i32 height) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 8;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_min_size: width=%d height=%d"), xdg_toplevel, width, height);
}

internal void wayland_xdg_toplevel_set_maximized(Wayland_Connection *conn, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_maximized:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_unset_maximized(Wayland_Connection *conn, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 10;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.unset_maximized:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_fullscreen(Wayland_Connection *conn, u32 xdg_toplevel, u32 output) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 11;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &output);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_fullscreen: output=%d"), xdg_toplevel, output);
}

internal void wayland_xdg_toplevel_unset_fullscreen(Wayland_Connection *conn, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 12;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.unset_fullscreen:"), xdg_toplevel);
}

internal void wayland_xdg_toplevel_set_minimized(Wayland_Connection *conn, u32 xdg_toplevel) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_toplevel);
	u16 _opcode = 13;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_toplevel@%d.set_minimized:"), xdg_toplevel);
}

typedef enum {
	Wayland_Xdg_Toplevel_Event_Configure = 0,
	Wayland_Xdg_Toplevel_Event_Close = 1,
	Wayland_Xdg_Toplevel_Event_Configure_Bounds = 2,
	Wayland_Xdg_Toplevel_Event_Wm_Capabilities = 3,
} Wayland_Xdg_Toplevel_Event;

internal isize wayland_parse_event_xdg_toplevel_configure(Wayland_Connection *conn, i32 *width, i32 *height, Byte_Slice *states, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, width), -1);
	or_return(read_any(&r, height), -1);
	u32 states_len;
	or_return(read_any(&r, &states_len), -1);
	slice_init(states, states_len, allocator);
	or_return(read_bytes(&r, *states), -1);
	if (states_len % 4) {
		byte states_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = states_pad_buf, .len = 4 - (states_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- xdg_toplevel@%d.configure: width=%d height=%d"), _object_id, *width, *height);
	return _msg_size;
}

internal isize wayland_parse_event_xdg_toplevel_close(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	wayland_log_infof(LIT("<- xdg_toplevel@%d.close:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_xdg_toplevel_configure_bounds(Wayland_Connection *conn, i32 *width, i32 *height) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, width), -1);
	or_return(read_any(&r, height), -1);
	wayland_log_infof(LIT("<- xdg_toplevel@%d.configure_bounds: width=%d height=%d"), _object_id, *width, *height);
	return _msg_size;
}

internal isize wayland_parse_event_xdg_toplevel_wm_capabilities(Wayland_Connection *conn, Byte_Slice *capabilities, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	u32 capabilities_len;
	or_return(read_any(&r, &capabilities_len), -1);
	slice_init(capabilities, capabilities_len, allocator);
	or_return(read_bytes(&r, *capabilities), -1);
	if (capabilities_len % 4) {
		byte capabilities_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = capabilities_pad_buf, .len = 4 - (capabilities_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- xdg_toplevel@%d.wm_capabilities:"), _object_id);
	return _msg_size;
}

internal void wayland_xdg_popup_destroy(Wayland_Connection *conn, u32 xdg_popup) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> xdg_popup@%d.destroy:"), xdg_popup);
}

internal void wayland_xdg_popup_grab(Wayland_Connection *conn, u32 xdg_popup, u32 seat, u32 serial) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> xdg_popup@%d.grab: seat=%d serial=%d"), xdg_popup, seat, serial);
}

internal void wayland_xdg_popup_reposition(Wayland_Connection *conn, u32 xdg_popup, u32 positioner, u32 token) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &xdg_popup);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &positioner);
	write_any(w, &token);

	wayland_log_infof(LIT("-> xdg_popup@%d.reposition: positioner=%d token=%d"), xdg_popup, positioner, token);
}

typedef enum {
	Wayland_Xdg_Popup_Event_Configure = 0,
	Wayland_Xdg_Popup_Event_Popup_Done = 1,
	Wayland_Xdg_Popup_Event_Repositioned = 2,
} Wayland_Xdg_Popup_Event;

internal isize wayland_parse_event_xdg_popup_configure(Wayland_Connection *conn, i32 *x, i32 *y, i32 *width, i32 *height) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, x), -1);
	or_return(read_any(&r, y), -1);
	or_return(read_any(&r, width), -1);
	or_return(read_any(&r, height), -1);
	wayland_log_infof(LIT("<- xdg_popup@%d.configure: x=%d y=%d width=%d height=%d"), _object_id, *x, *y, *width, *height);
	return _msg_size;
}

internal isize wayland_parse_event_xdg_popup_popup_done(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	wayland_log_infof(LIT("<- xdg_popup@%d.popup_done:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_xdg_popup_repositioned(Wayland_Connection *conn, u32 *token) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, token), -1);
	wayland_log_infof(LIT("<- xdg_popup@%d.repositioned: token=%d"), _object_id, *token);
	return _msg_size;
}

