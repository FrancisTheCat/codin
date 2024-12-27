// Generated from XML

#include "codin.h"
#include "wayland_gen_common.h"

typedef enum {
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default = 1,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Context_Menu = 2,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Help = 3,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Pointer = 4,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Progress = 5,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Wait = 6,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Cell = 7,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Crosshair = 8,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Text = 9,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Vertical_Text = 10,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Alias = 11,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Copy = 12,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Move = 13,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_No_Drop = 14,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Not_Allowed = 15,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grab = 16,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grabbing = 17,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_E_Resize = 18,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_N_Resize = 19,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ne_Resize = 20,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nw_Resize = 21,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_S_Resize = 22,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Se_Resize = 23,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Sw_Resize = 24,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_W_Resize = 25,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ew_Resize = 26,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ns_Resize = 27,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nesw_Resize = 28,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nwse_Resize = 29,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Col_Resize = 30,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Row_Resize = 31,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_All_Scroll = 32,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_In = 33,
	Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out = 34,
} Wayland_Wp_Cursor_Shape_Device_V1_Shape;

ENUM_TO_STRING_PROC_DECL(Wayland_Wp_Cursor_Shape_Device_V1_Shape, v) {
	switch (v) {
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Default");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Context_Menu:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Context_Menu");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Help:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Help");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Pointer:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Pointer");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Progress:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Progress");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Wait:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Wait");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Cell:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Cell");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Crosshair:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Crosshair");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Text:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Text");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Vertical_Text:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Vertical_Text");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Alias:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Alias");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Copy:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Copy");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Move:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Move");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_No_Drop:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_No_Drop");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Not_Allowed:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Not_Allowed");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grab:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grab");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grabbing:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Grabbing");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_E_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_E_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_N_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_N_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ne_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ne_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nw_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nw_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_S_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_S_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Se_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Se_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Sw_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Sw_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_W_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_W_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ew_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ew_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ns_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Ns_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nesw_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nesw_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nwse_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Nwse_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Col_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Col_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Row_Resize:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Row_Resize");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_All_Scroll:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_All_Scroll");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_In:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_In");
	case Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_Zoom_Out");
	}
	return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Shape_INVALID");
}

typedef enum {
	Wayland_Wp_Cursor_Shape_Device_V1_Error_Invalid_Shape = 1,
} Wayland_Wp_Cursor_Shape_Device_V1_Error;

ENUM_TO_STRING_PROC_DECL(Wayland_Wp_Cursor_Shape_Device_V1_Error, v) {
	switch (v) {
	case Wayland_Wp_Cursor_Shape_Device_V1_Error_Invalid_Shape:
		return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Error_Invalid_Shape");
	}
	return LIT("Wayland_Wp_Cursor_Shape_Device_V1_Error_INVALID");
}

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

