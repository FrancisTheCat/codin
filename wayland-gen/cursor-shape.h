// Generated from XML

#include "codin.h"
#include "wayland_gen_common.h"

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default, 1)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Context_Menu, 2)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Help, 3)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Pointer, 4)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Progress, 5)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Wait, 6)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Cell, 7)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Crosshair, 8)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Text, 9)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Vertical_Text, 10)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Alias, 11)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Copy, 12)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Move, 13)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_No_Drop, 14)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Not_Allowed, 15)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grab, 16)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grabbing, 17)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_E_Resize, 18)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_N_Resize, 19)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ne_Resize, 20)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nw_Resize, 21)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_S_Resize, 22)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Se_Resize, 23)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Sw_Resize, 24)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_W_Resize, 25)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ew_Resize, 26)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ns_Resize, 27)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nesw_Resize, 28)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nwse_Resize, 29)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Col_Resize, 30)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Row_Resize, 31)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_All_Scroll, 32)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_In, 33)\
	X(Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out, 34)\

X_ENUM_EXPLICIT(Wayland_Wp_Cursor_Shape_Device_V1_Shape, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

#define WAYLAND_X_ENUM_VARIANTS(X) \
	X(Wayland_Wp_Cursor_Shape_Device_V1_Error_Invalid_Shape, 1)\

X_ENUM_EXPLICIT(Wayland_Wp_Cursor_Shape_Device_V1_Error, WAYLAND_X_ENUM_VARIANTS);

#undef WAYLAND_X_ENUM_VARIANTS

internal void wayland_wp_cursor_shape_manager_v1_destroy(Wayland_Connection *conn, u32 wp_cursor_shape_manager_v1) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.destroy:"), wp_cursor_shape_manager_v1);
}

internal u32 wayland_wp_cursor_shape_manager_v1_get_pointer(Wayland_Connection *conn, u32 wp_cursor_shape_manager_v1, u32 pointer) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);
	write_any(w, &pointer);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.get_pointer: cursor_shape_device=%d pointer=%d"), wp_cursor_shape_manager_v1, return_value, pointer);
	return return_value;
}

internal u32 wayland_wp_cursor_shape_manager_v1_get_tablet_tool_v2(Wayland_Connection *conn, u32 wp_cursor_shape_manager_v1, u32 tablet_tool) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	u32 return_value = _wayland_connection_get_next_id(conn);
	write_any(w, &return_value);
	write_any(w, &tablet_tool);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.get_tablet_tool_v2: cursor_shape_device=%d tablet_tool=%d"), wp_cursor_shape_manager_v1, return_value, tablet_tool);
	return return_value;
}

internal void wayland_wp_cursor_shape_device_v1_destroy(Wayland_Connection *conn, u32 wp_cursor_shape_device_v1) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_device_v1);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wp_cursor_shape_device_v1@%d.destroy:"), wp_cursor_shape_device_v1);
}

internal void wayland_wp_cursor_shape_device_v1_set_shape(Wayland_Connection *conn, u32 wp_cursor_shape_device_v1, u32 serial, Wayland_Wp_Cursor_Shape_Device_V1_Shape shape) {
	Writer _w = writer_from_builder(&conn->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_device_v1);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &serial);
	u32 shape_value = (u32)shape;
	write_any(w, &shape_value);

	wayland_log_infof(LIT("-> wp_cursor_shape_device_v1@%d.set_shape: serial=%d shape=%S"), wp_cursor_shape_device_v1, serial, enum_to_string(Wayland_Wp_Cursor_Shape_Device_V1_Shape, shape));
}

