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

typedef enum {
	Wayland_Wp_Cursor_Shape_Device_V1_Error_Invalid_Shape = 1,
} Wayland_Wp_Cursor_Shape_Device_V1_Error;

internal void wayland_wp_cursor_shape_manager_v1_destroy(Wayland_Connection *wc, u32 wp_cursor_shape_manager_v1) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.destroy:"), wp_cursor_shape_manager_v1);
}

internal u32 wayland_wp_cursor_shape_manager_v1_get_pointer(Wayland_Connection *wc, u32 wp_cursor_shape_manager_v1, u32 pointer) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &pointer);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.get_pointer:"), wp_cursor_shape_manager_v1);
	return return_value;
}

internal u32 wayland_wp_cursor_shape_manager_v1_get_tablet_tool_v2(Wayland_Connection *wc, u32 wp_cursor_shape_manager_v1, u32 tablet_tool) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_manager_v1);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &tablet_tool);

	wayland_log_infof(LIT("-> wp_cursor_shape_manager_v1@%d.get_tablet_tool_v2:"), wp_cursor_shape_manager_v1);
	return return_value;
}

internal void wayland_wp_cursor_shape_device_v1_destroy(Wayland_Connection *wc, u32 wp_cursor_shape_device_v1) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_device_v1);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wp_cursor_shape_device_v1@%d.destroy:"), wp_cursor_shape_device_v1);
}

internal void wayland_wp_cursor_shape_device_v1_set_shape(Wayland_Connection *wc, u32 wp_cursor_shape_device_v1, u32 serial, Wayland_Wp_Cursor_Shape_Device_V1_Shape shape) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wp_cursor_shape_device_v1);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &serial);
	i32 shape_value = (i32)shape;
	write_any(w, &shape_value);

	wayland_log_infof(LIT("-> wp_cursor_shape_device_v1@%d.set_shape:"), wp_cursor_shape_device_v1);
}

