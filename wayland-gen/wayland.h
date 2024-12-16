// Generated from XML

#include "codin.h"
#include "wayland_gen_common.h"

typedef enum {
	Wayland_Wl_Display_Error_Invalid_Object = 0,
	Wayland_Wl_Display_Error_Invalid_Method = 1,
	Wayland_Wl_Display_Error_No_Memory = 2,
	Wayland_Wl_Display_Error_Implementation = 3,
} Wayland_Wl_Display_Error;

internal String wayland_wl_display_error_string(Wayland_Wl_Display_Error v) {
	switch (v) {
	case Wayland_Wl_Display_Error_Invalid_Object:
		return LIT("Wayland_Wl_Display_Error_Invalid_Object");
	case Wayland_Wl_Display_Error_Invalid_Method:
		return LIT("Wayland_Wl_Display_Error_Invalid_Method");
	case Wayland_Wl_Display_Error_No_Memory:
		return LIT("Wayland_Wl_Display_Error_No_Memory");
	case Wayland_Wl_Display_Error_Implementation:
		return LIT("Wayland_Wl_Display_Error_Implementation");
	}
	return LIT("Wayland_Wl_Display_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shm_Error_Invalid_Format = 0,
	Wayland_Wl_Shm_Error_Invalid_Stride = 1,
	Wayland_Wl_Shm_Error_Invalid_Fd = 2,
} Wayland_Wl_Shm_Error;

internal String wayland_wl_shm_error_string(Wayland_Wl_Shm_Error v) {
	switch (v) {
	case Wayland_Wl_Shm_Error_Invalid_Format:
		return LIT("Wayland_Wl_Shm_Error_Invalid_Format");
	case Wayland_Wl_Shm_Error_Invalid_Stride:
		return LIT("Wayland_Wl_Shm_Error_Invalid_Stride");
	case Wayland_Wl_Shm_Error_Invalid_Fd:
		return LIT("Wayland_Wl_Shm_Error_Invalid_Fd");
	}
	return LIT("Wayland_Wl_Shm_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shm_Format_Argb8888 = 0,
	Wayland_Wl_Shm_Format_Xrgb8888 = 1,
	Wayland_Wl_Shm_Format_C8 = 0x20203843,
	Wayland_Wl_Shm_Format_Rgb332 = 0x38424752,
	Wayland_Wl_Shm_Format_Bgr233 = 0x38524742,
	Wayland_Wl_Shm_Format_Xrgb4444 = 0x32315258,
	Wayland_Wl_Shm_Format_Xbgr4444 = 0x32314258,
	Wayland_Wl_Shm_Format_Rgbx4444 = 0x32315852,
	Wayland_Wl_Shm_Format_Bgrx4444 = 0x32315842,
	Wayland_Wl_Shm_Format_Argb4444 = 0x32315241,
	Wayland_Wl_Shm_Format_Abgr4444 = 0x32314241,
	Wayland_Wl_Shm_Format_Rgba4444 = 0x32314152,
	Wayland_Wl_Shm_Format_Bgra4444 = 0x32314142,
	Wayland_Wl_Shm_Format_Xrgb1555 = 0x35315258,
	Wayland_Wl_Shm_Format_Xbgr1555 = 0x35314258,
	Wayland_Wl_Shm_Format_Rgbx5551 = 0x35315852,
	Wayland_Wl_Shm_Format_Bgrx5551 = 0x35315842,
	Wayland_Wl_Shm_Format_Argb1555 = 0x35315241,
	Wayland_Wl_Shm_Format_Abgr1555 = 0x35314241,
	Wayland_Wl_Shm_Format_Rgba5551 = 0x35314152,
	Wayland_Wl_Shm_Format_Bgra5551 = 0x35314142,
	Wayland_Wl_Shm_Format_Rgb565 = 0x36314752,
	Wayland_Wl_Shm_Format_Bgr565 = 0x36314742,
	Wayland_Wl_Shm_Format_Rgb888 = 0x34324752,
	Wayland_Wl_Shm_Format_Bgr888 = 0x34324742,
	Wayland_Wl_Shm_Format_Xbgr8888 = 0x34324258,
	Wayland_Wl_Shm_Format_Rgbx8888 = 0x34325852,
	Wayland_Wl_Shm_Format_Bgrx8888 = 0x34325842,
	Wayland_Wl_Shm_Format_Abgr8888 = 0x34324241,
	Wayland_Wl_Shm_Format_Rgba8888 = 0x34324152,
	Wayland_Wl_Shm_Format_Bgra8888 = 0x34324142,
	Wayland_Wl_Shm_Format_Xrgb2101010 = 0x30335258,
	Wayland_Wl_Shm_Format_Xbgr2101010 = 0x30334258,
	Wayland_Wl_Shm_Format_Rgbx1010102 = 0x30335852,
	Wayland_Wl_Shm_Format_Bgrx1010102 = 0x30335842,
	Wayland_Wl_Shm_Format_Argb2101010 = 0x30335241,
	Wayland_Wl_Shm_Format_Abgr2101010 = 0x30334241,
	Wayland_Wl_Shm_Format_Rgba1010102 = 0x30334152,
	Wayland_Wl_Shm_Format_Bgra1010102 = 0x30334142,
	Wayland_Wl_Shm_Format_Yuyv = 0x56595559,
	Wayland_Wl_Shm_Format_Yvyu = 0x55595659,
	Wayland_Wl_Shm_Format_Uyvy = 0x59565955,
	Wayland_Wl_Shm_Format_Vyuy = 0x59555956,
	Wayland_Wl_Shm_Format_Ayuv = 0x56555941,
	Wayland_Wl_Shm_Format_Nv12 = 0x3231564e,
	Wayland_Wl_Shm_Format_Nv21 = 0x3132564e,
	Wayland_Wl_Shm_Format_Nv16 = 0x3631564e,
	Wayland_Wl_Shm_Format_Nv61 = 0x3136564e,
	Wayland_Wl_Shm_Format_Yuv410 = 0x39565559,
	Wayland_Wl_Shm_Format_Yvu410 = 0x39555659,
	Wayland_Wl_Shm_Format_Yuv411 = 0x31315559,
	Wayland_Wl_Shm_Format_Yvu411 = 0x31315659,
	Wayland_Wl_Shm_Format_Yuv420 = 0x32315559,
	Wayland_Wl_Shm_Format_Yvu420 = 0x32315659,
	Wayland_Wl_Shm_Format_Yuv422 = 0x36315559,
	Wayland_Wl_Shm_Format_Yvu422 = 0x36315659,
	Wayland_Wl_Shm_Format_Yuv444 = 0x34325559,
	Wayland_Wl_Shm_Format_Yvu444 = 0x34325659,
	Wayland_Wl_Shm_Format_R8 = 0x20203852,
	Wayland_Wl_Shm_Format_R16 = 0x20363152,
	Wayland_Wl_Shm_Format_Rg88 = 0x38384752,
	Wayland_Wl_Shm_Format_Gr88 = 0x38385247,
	Wayland_Wl_Shm_Format_Rg1616 = 0x32334752,
	Wayland_Wl_Shm_Format_Gr1616 = 0x32335247,
	Wayland_Wl_Shm_Format_Xrgb16161616f = 0x48345258,
	Wayland_Wl_Shm_Format_Xbgr16161616f = 0x48344258,
	Wayland_Wl_Shm_Format_Argb16161616f = 0x48345241,
	Wayland_Wl_Shm_Format_Abgr16161616f = 0x48344241,
	Wayland_Wl_Shm_Format_Xyuv8888 = 0x56555958,
	Wayland_Wl_Shm_Format_Vuy888 = 0x34325556,
	Wayland_Wl_Shm_Format_Vuy101010 = 0x30335556,
	Wayland_Wl_Shm_Format_Y210 = 0x30313259,
	Wayland_Wl_Shm_Format_Y212 = 0x32313259,
	Wayland_Wl_Shm_Format_Y216 = 0x36313259,
	Wayland_Wl_Shm_Format_Y410 = 0x30313459,
	Wayland_Wl_Shm_Format_Y412 = 0x32313459,
	Wayland_Wl_Shm_Format_Y416 = 0x36313459,
	Wayland_Wl_Shm_Format_Xvyu2101010 = 0x30335658,
	Wayland_Wl_Shm_Format_Xvyu12_16161616 = 0x36335658,
	Wayland_Wl_Shm_Format_Xvyu16161616 = 0x38345658,
	Wayland_Wl_Shm_Format_Y0l0 = 0x304c3059,
	Wayland_Wl_Shm_Format_X0l0 = 0x304c3058,
	Wayland_Wl_Shm_Format_Y0l2 = 0x324c3059,
	Wayland_Wl_Shm_Format_X0l2 = 0x324c3058,
	Wayland_Wl_Shm_Format_Yuv420_8bit = 0x38305559,
	Wayland_Wl_Shm_Format_Yuv420_10bit = 0x30315559,
	Wayland_Wl_Shm_Format_Xrgb8888_A8 = 0x38415258,
	Wayland_Wl_Shm_Format_Xbgr8888_A8 = 0x38414258,
	Wayland_Wl_Shm_Format_Rgbx8888_A8 = 0x38415852,
	Wayland_Wl_Shm_Format_Bgrx8888_A8 = 0x38415842,
	Wayland_Wl_Shm_Format_Rgb888_A8 = 0x38413852,
	Wayland_Wl_Shm_Format_Bgr888_A8 = 0x38413842,
	Wayland_Wl_Shm_Format_Rgb565_A8 = 0x38413552,
	Wayland_Wl_Shm_Format_Bgr565_A8 = 0x38413542,
	Wayland_Wl_Shm_Format_Nv24 = 0x3432564e,
	Wayland_Wl_Shm_Format_Nv42 = 0x3234564e,
	Wayland_Wl_Shm_Format_P210 = 0x30313250,
	Wayland_Wl_Shm_Format_P010 = 0x30313050,
	Wayland_Wl_Shm_Format_P012 = 0x32313050,
	Wayland_Wl_Shm_Format_P016 = 0x36313050,
	Wayland_Wl_Shm_Format_Axbxgxrx106106106106 = 0x30314241,
	Wayland_Wl_Shm_Format_Nv15 = 0x3531564e,
	Wayland_Wl_Shm_Format_Q410 = 0x30313451,
	Wayland_Wl_Shm_Format_Q401 = 0x31303451,
	Wayland_Wl_Shm_Format_Xrgb16161616 = 0x38345258,
	Wayland_Wl_Shm_Format_Xbgr16161616 = 0x38344258,
	Wayland_Wl_Shm_Format_Argb16161616 = 0x38345241,
	Wayland_Wl_Shm_Format_Abgr16161616 = 0x38344241,
	Wayland_Wl_Shm_Format_C1 = 0x20203143,
	Wayland_Wl_Shm_Format_C2 = 0x20203243,
	Wayland_Wl_Shm_Format_C4 = 0x20203443,
	Wayland_Wl_Shm_Format_D1 = 0x20203144,
	Wayland_Wl_Shm_Format_D2 = 0x20203244,
	Wayland_Wl_Shm_Format_D4 = 0x20203444,
	Wayland_Wl_Shm_Format_D8 = 0x20203844,
	Wayland_Wl_Shm_Format_R1 = 0x20203152,
	Wayland_Wl_Shm_Format_R2 = 0x20203252,
	Wayland_Wl_Shm_Format_R4 = 0x20203452,
	Wayland_Wl_Shm_Format_R10 = 0x20303152,
	Wayland_Wl_Shm_Format_R12 = 0x20323152,
	Wayland_Wl_Shm_Format_Avuy8888 = 0x59555641,
	Wayland_Wl_Shm_Format_Xvuy8888 = 0x59555658,
	Wayland_Wl_Shm_Format_P030 = 0x30333050,
} Wayland_Wl_Shm_Format;

internal String wayland_wl_shm_format_string(Wayland_Wl_Shm_Format v) {
	switch (v) {
	case Wayland_Wl_Shm_Format_Argb8888:
		return LIT("Wayland_Wl_Shm_Format_Argb8888");
	case Wayland_Wl_Shm_Format_Xrgb8888:
		return LIT("Wayland_Wl_Shm_Format_Xrgb8888");
	case Wayland_Wl_Shm_Format_C8:
		return LIT("Wayland_Wl_Shm_Format_C8");
	case Wayland_Wl_Shm_Format_Rgb332:
		return LIT("Wayland_Wl_Shm_Format_Rgb332");
	case Wayland_Wl_Shm_Format_Bgr233:
		return LIT("Wayland_Wl_Shm_Format_Bgr233");
	case Wayland_Wl_Shm_Format_Xrgb4444:
		return LIT("Wayland_Wl_Shm_Format_Xrgb4444");
	case Wayland_Wl_Shm_Format_Xbgr4444:
		return LIT("Wayland_Wl_Shm_Format_Xbgr4444");
	case Wayland_Wl_Shm_Format_Rgbx4444:
		return LIT("Wayland_Wl_Shm_Format_Rgbx4444");
	case Wayland_Wl_Shm_Format_Bgrx4444:
		return LIT("Wayland_Wl_Shm_Format_Bgrx4444");
	case Wayland_Wl_Shm_Format_Argb4444:
		return LIT("Wayland_Wl_Shm_Format_Argb4444");
	case Wayland_Wl_Shm_Format_Abgr4444:
		return LIT("Wayland_Wl_Shm_Format_Abgr4444");
	case Wayland_Wl_Shm_Format_Rgba4444:
		return LIT("Wayland_Wl_Shm_Format_Rgba4444");
	case Wayland_Wl_Shm_Format_Bgra4444:
		return LIT("Wayland_Wl_Shm_Format_Bgra4444");
	case Wayland_Wl_Shm_Format_Xrgb1555:
		return LIT("Wayland_Wl_Shm_Format_Xrgb1555");
	case Wayland_Wl_Shm_Format_Xbgr1555:
		return LIT("Wayland_Wl_Shm_Format_Xbgr1555");
	case Wayland_Wl_Shm_Format_Rgbx5551:
		return LIT("Wayland_Wl_Shm_Format_Rgbx5551");
	case Wayland_Wl_Shm_Format_Bgrx5551:
		return LIT("Wayland_Wl_Shm_Format_Bgrx5551");
	case Wayland_Wl_Shm_Format_Argb1555:
		return LIT("Wayland_Wl_Shm_Format_Argb1555");
	case Wayland_Wl_Shm_Format_Abgr1555:
		return LIT("Wayland_Wl_Shm_Format_Abgr1555");
	case Wayland_Wl_Shm_Format_Rgba5551:
		return LIT("Wayland_Wl_Shm_Format_Rgba5551");
	case Wayland_Wl_Shm_Format_Bgra5551:
		return LIT("Wayland_Wl_Shm_Format_Bgra5551");
	case Wayland_Wl_Shm_Format_Rgb565:
		return LIT("Wayland_Wl_Shm_Format_Rgb565");
	case Wayland_Wl_Shm_Format_Bgr565:
		return LIT("Wayland_Wl_Shm_Format_Bgr565");
	case Wayland_Wl_Shm_Format_Rgb888:
		return LIT("Wayland_Wl_Shm_Format_Rgb888");
	case Wayland_Wl_Shm_Format_Bgr888:
		return LIT("Wayland_Wl_Shm_Format_Bgr888");
	case Wayland_Wl_Shm_Format_Xbgr8888:
		return LIT("Wayland_Wl_Shm_Format_Xbgr8888");
	case Wayland_Wl_Shm_Format_Rgbx8888:
		return LIT("Wayland_Wl_Shm_Format_Rgbx8888");
	case Wayland_Wl_Shm_Format_Bgrx8888:
		return LIT("Wayland_Wl_Shm_Format_Bgrx8888");
	case Wayland_Wl_Shm_Format_Abgr8888:
		return LIT("Wayland_Wl_Shm_Format_Abgr8888");
	case Wayland_Wl_Shm_Format_Rgba8888:
		return LIT("Wayland_Wl_Shm_Format_Rgba8888");
	case Wayland_Wl_Shm_Format_Bgra8888:
		return LIT("Wayland_Wl_Shm_Format_Bgra8888");
	case Wayland_Wl_Shm_Format_Xrgb2101010:
		return LIT("Wayland_Wl_Shm_Format_Xrgb2101010");
	case Wayland_Wl_Shm_Format_Xbgr2101010:
		return LIT("Wayland_Wl_Shm_Format_Xbgr2101010");
	case Wayland_Wl_Shm_Format_Rgbx1010102:
		return LIT("Wayland_Wl_Shm_Format_Rgbx1010102");
	case Wayland_Wl_Shm_Format_Bgrx1010102:
		return LIT("Wayland_Wl_Shm_Format_Bgrx1010102");
	case Wayland_Wl_Shm_Format_Argb2101010:
		return LIT("Wayland_Wl_Shm_Format_Argb2101010");
	case Wayland_Wl_Shm_Format_Abgr2101010:
		return LIT("Wayland_Wl_Shm_Format_Abgr2101010");
	case Wayland_Wl_Shm_Format_Rgba1010102:
		return LIT("Wayland_Wl_Shm_Format_Rgba1010102");
	case Wayland_Wl_Shm_Format_Bgra1010102:
		return LIT("Wayland_Wl_Shm_Format_Bgra1010102");
	case Wayland_Wl_Shm_Format_Yuyv:
		return LIT("Wayland_Wl_Shm_Format_Yuyv");
	case Wayland_Wl_Shm_Format_Yvyu:
		return LIT("Wayland_Wl_Shm_Format_Yvyu");
	case Wayland_Wl_Shm_Format_Uyvy:
		return LIT("Wayland_Wl_Shm_Format_Uyvy");
	case Wayland_Wl_Shm_Format_Vyuy:
		return LIT("Wayland_Wl_Shm_Format_Vyuy");
	case Wayland_Wl_Shm_Format_Ayuv:
		return LIT("Wayland_Wl_Shm_Format_Ayuv");
	case Wayland_Wl_Shm_Format_Nv12:
		return LIT("Wayland_Wl_Shm_Format_Nv12");
	case Wayland_Wl_Shm_Format_Nv21:
		return LIT("Wayland_Wl_Shm_Format_Nv21");
	case Wayland_Wl_Shm_Format_Nv16:
		return LIT("Wayland_Wl_Shm_Format_Nv16");
	case Wayland_Wl_Shm_Format_Nv61:
		return LIT("Wayland_Wl_Shm_Format_Nv61");
	case Wayland_Wl_Shm_Format_Yuv410:
		return LIT("Wayland_Wl_Shm_Format_Yuv410");
	case Wayland_Wl_Shm_Format_Yvu410:
		return LIT("Wayland_Wl_Shm_Format_Yvu410");
	case Wayland_Wl_Shm_Format_Yuv411:
		return LIT("Wayland_Wl_Shm_Format_Yuv411");
	case Wayland_Wl_Shm_Format_Yvu411:
		return LIT("Wayland_Wl_Shm_Format_Yvu411");
	case Wayland_Wl_Shm_Format_Yuv420:
		return LIT("Wayland_Wl_Shm_Format_Yuv420");
	case Wayland_Wl_Shm_Format_Yvu420:
		return LIT("Wayland_Wl_Shm_Format_Yvu420");
	case Wayland_Wl_Shm_Format_Yuv422:
		return LIT("Wayland_Wl_Shm_Format_Yuv422");
	case Wayland_Wl_Shm_Format_Yvu422:
		return LIT("Wayland_Wl_Shm_Format_Yvu422");
	case Wayland_Wl_Shm_Format_Yuv444:
		return LIT("Wayland_Wl_Shm_Format_Yuv444");
	case Wayland_Wl_Shm_Format_Yvu444:
		return LIT("Wayland_Wl_Shm_Format_Yvu444");
	case Wayland_Wl_Shm_Format_R8:
		return LIT("Wayland_Wl_Shm_Format_R8");
	case Wayland_Wl_Shm_Format_R16:
		return LIT("Wayland_Wl_Shm_Format_R16");
	case Wayland_Wl_Shm_Format_Rg88:
		return LIT("Wayland_Wl_Shm_Format_Rg88");
	case Wayland_Wl_Shm_Format_Gr88:
		return LIT("Wayland_Wl_Shm_Format_Gr88");
	case Wayland_Wl_Shm_Format_Rg1616:
		return LIT("Wayland_Wl_Shm_Format_Rg1616");
	case Wayland_Wl_Shm_Format_Gr1616:
		return LIT("Wayland_Wl_Shm_Format_Gr1616");
	case Wayland_Wl_Shm_Format_Xrgb16161616f:
		return LIT("Wayland_Wl_Shm_Format_Xrgb16161616f");
	case Wayland_Wl_Shm_Format_Xbgr16161616f:
		return LIT("Wayland_Wl_Shm_Format_Xbgr16161616f");
	case Wayland_Wl_Shm_Format_Argb16161616f:
		return LIT("Wayland_Wl_Shm_Format_Argb16161616f");
	case Wayland_Wl_Shm_Format_Abgr16161616f:
		return LIT("Wayland_Wl_Shm_Format_Abgr16161616f");
	case Wayland_Wl_Shm_Format_Xyuv8888:
		return LIT("Wayland_Wl_Shm_Format_Xyuv8888");
	case Wayland_Wl_Shm_Format_Vuy888:
		return LIT("Wayland_Wl_Shm_Format_Vuy888");
	case Wayland_Wl_Shm_Format_Vuy101010:
		return LIT("Wayland_Wl_Shm_Format_Vuy101010");
	case Wayland_Wl_Shm_Format_Y210:
		return LIT("Wayland_Wl_Shm_Format_Y210");
	case Wayland_Wl_Shm_Format_Y212:
		return LIT("Wayland_Wl_Shm_Format_Y212");
	case Wayland_Wl_Shm_Format_Y216:
		return LIT("Wayland_Wl_Shm_Format_Y216");
	case Wayland_Wl_Shm_Format_Y410:
		return LIT("Wayland_Wl_Shm_Format_Y410");
	case Wayland_Wl_Shm_Format_Y412:
		return LIT("Wayland_Wl_Shm_Format_Y412");
	case Wayland_Wl_Shm_Format_Y416:
		return LIT("Wayland_Wl_Shm_Format_Y416");
	case Wayland_Wl_Shm_Format_Xvyu2101010:
		return LIT("Wayland_Wl_Shm_Format_Xvyu2101010");
	case Wayland_Wl_Shm_Format_Xvyu12_16161616:
		return LIT("Wayland_Wl_Shm_Format_Xvyu12_16161616");
	case Wayland_Wl_Shm_Format_Xvyu16161616:
		return LIT("Wayland_Wl_Shm_Format_Xvyu16161616");
	case Wayland_Wl_Shm_Format_Y0l0:
		return LIT("Wayland_Wl_Shm_Format_Y0l0");
	case Wayland_Wl_Shm_Format_X0l0:
		return LIT("Wayland_Wl_Shm_Format_X0l0");
	case Wayland_Wl_Shm_Format_Y0l2:
		return LIT("Wayland_Wl_Shm_Format_Y0l2");
	case Wayland_Wl_Shm_Format_X0l2:
		return LIT("Wayland_Wl_Shm_Format_X0l2");
	case Wayland_Wl_Shm_Format_Yuv420_8bit:
		return LIT("Wayland_Wl_Shm_Format_Yuv420_8bit");
	case Wayland_Wl_Shm_Format_Yuv420_10bit:
		return LIT("Wayland_Wl_Shm_Format_Yuv420_10bit");
	case Wayland_Wl_Shm_Format_Xrgb8888_A8:
		return LIT("Wayland_Wl_Shm_Format_Xrgb8888_A8");
	case Wayland_Wl_Shm_Format_Xbgr8888_A8:
		return LIT("Wayland_Wl_Shm_Format_Xbgr8888_A8");
	case Wayland_Wl_Shm_Format_Rgbx8888_A8:
		return LIT("Wayland_Wl_Shm_Format_Rgbx8888_A8");
	case Wayland_Wl_Shm_Format_Bgrx8888_A8:
		return LIT("Wayland_Wl_Shm_Format_Bgrx8888_A8");
	case Wayland_Wl_Shm_Format_Rgb888_A8:
		return LIT("Wayland_Wl_Shm_Format_Rgb888_A8");
	case Wayland_Wl_Shm_Format_Bgr888_A8:
		return LIT("Wayland_Wl_Shm_Format_Bgr888_A8");
	case Wayland_Wl_Shm_Format_Rgb565_A8:
		return LIT("Wayland_Wl_Shm_Format_Rgb565_A8");
	case Wayland_Wl_Shm_Format_Bgr565_A8:
		return LIT("Wayland_Wl_Shm_Format_Bgr565_A8");
	case Wayland_Wl_Shm_Format_Nv24:
		return LIT("Wayland_Wl_Shm_Format_Nv24");
	case Wayland_Wl_Shm_Format_Nv42:
		return LIT("Wayland_Wl_Shm_Format_Nv42");
	case Wayland_Wl_Shm_Format_P210:
		return LIT("Wayland_Wl_Shm_Format_P210");
	case Wayland_Wl_Shm_Format_P010:
		return LIT("Wayland_Wl_Shm_Format_P010");
	case Wayland_Wl_Shm_Format_P012:
		return LIT("Wayland_Wl_Shm_Format_P012");
	case Wayland_Wl_Shm_Format_P016:
		return LIT("Wayland_Wl_Shm_Format_P016");
	case Wayland_Wl_Shm_Format_Axbxgxrx106106106106:
		return LIT("Wayland_Wl_Shm_Format_Axbxgxrx106106106106");
	case Wayland_Wl_Shm_Format_Nv15:
		return LIT("Wayland_Wl_Shm_Format_Nv15");
	case Wayland_Wl_Shm_Format_Q410:
		return LIT("Wayland_Wl_Shm_Format_Q410");
	case Wayland_Wl_Shm_Format_Q401:
		return LIT("Wayland_Wl_Shm_Format_Q401");
	case Wayland_Wl_Shm_Format_Xrgb16161616:
		return LIT("Wayland_Wl_Shm_Format_Xrgb16161616");
	case Wayland_Wl_Shm_Format_Xbgr16161616:
		return LIT("Wayland_Wl_Shm_Format_Xbgr16161616");
	case Wayland_Wl_Shm_Format_Argb16161616:
		return LIT("Wayland_Wl_Shm_Format_Argb16161616");
	case Wayland_Wl_Shm_Format_Abgr16161616:
		return LIT("Wayland_Wl_Shm_Format_Abgr16161616");
	case Wayland_Wl_Shm_Format_C1:
		return LIT("Wayland_Wl_Shm_Format_C1");
	case Wayland_Wl_Shm_Format_C2:
		return LIT("Wayland_Wl_Shm_Format_C2");
	case Wayland_Wl_Shm_Format_C4:
		return LIT("Wayland_Wl_Shm_Format_C4");
	case Wayland_Wl_Shm_Format_D1:
		return LIT("Wayland_Wl_Shm_Format_D1");
	case Wayland_Wl_Shm_Format_D2:
		return LIT("Wayland_Wl_Shm_Format_D2");
	case Wayland_Wl_Shm_Format_D4:
		return LIT("Wayland_Wl_Shm_Format_D4");
	case Wayland_Wl_Shm_Format_D8:
		return LIT("Wayland_Wl_Shm_Format_D8");
	case Wayland_Wl_Shm_Format_R1:
		return LIT("Wayland_Wl_Shm_Format_R1");
	case Wayland_Wl_Shm_Format_R2:
		return LIT("Wayland_Wl_Shm_Format_R2");
	case Wayland_Wl_Shm_Format_R4:
		return LIT("Wayland_Wl_Shm_Format_R4");
	case Wayland_Wl_Shm_Format_R10:
		return LIT("Wayland_Wl_Shm_Format_R10");
	case Wayland_Wl_Shm_Format_R12:
		return LIT("Wayland_Wl_Shm_Format_R12");
	case Wayland_Wl_Shm_Format_Avuy8888:
		return LIT("Wayland_Wl_Shm_Format_Avuy8888");
	case Wayland_Wl_Shm_Format_Xvuy8888:
		return LIT("Wayland_Wl_Shm_Format_Xvuy8888");
	case Wayland_Wl_Shm_Format_P030:
		return LIT("Wayland_Wl_Shm_Format_P030");
	}
	return LIT("Wayland_Wl_Shm_Format_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Data_Offer_Error_Invalid_Finish = 0,
	Wayland_Wl_Data_Offer_Error_Invalid_Action_Mask = 1,
	Wayland_Wl_Data_Offer_Error_Invalid_Action = 2,
	Wayland_Wl_Data_Offer_Error_Invalid_Offer = 3,
} Wayland_Wl_Data_Offer_Error;

internal String wayland_wl_data_offer_error_string(Wayland_Wl_Data_Offer_Error v) {
	switch (v) {
	case Wayland_Wl_Data_Offer_Error_Invalid_Finish:
		return LIT("Wayland_Wl_Data_Offer_Error_Invalid_Finish");
	case Wayland_Wl_Data_Offer_Error_Invalid_Action_Mask:
		return LIT("Wayland_Wl_Data_Offer_Error_Invalid_Action_Mask");
	case Wayland_Wl_Data_Offer_Error_Invalid_Action:
		return LIT("Wayland_Wl_Data_Offer_Error_Invalid_Action");
	case Wayland_Wl_Data_Offer_Error_Invalid_Offer:
		return LIT("Wayland_Wl_Data_Offer_Error_Invalid_Offer");
	}
	return LIT("Wayland_Wl_Data_Offer_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Data_Source_Error_Invalid_Action_Mask = 0,
	Wayland_Wl_Data_Source_Error_Invalid_Source = 1,
} Wayland_Wl_Data_Source_Error;

internal String wayland_wl_data_source_error_string(Wayland_Wl_Data_Source_Error v) {
	switch (v) {
	case Wayland_Wl_Data_Source_Error_Invalid_Action_Mask:
		return LIT("Wayland_Wl_Data_Source_Error_Invalid_Action_Mask");
	case Wayland_Wl_Data_Source_Error_Invalid_Source:
		return LIT("Wayland_Wl_Data_Source_Error_Invalid_Source");
	}
	return LIT("Wayland_Wl_Data_Source_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Data_Device_Error_Role = 0,
	Wayland_Wl_Data_Device_Error_Used_Source = 1,
} Wayland_Wl_Data_Device_Error;

internal String wayland_wl_data_device_error_string(Wayland_Wl_Data_Device_Error v) {
	switch (v) {
	case Wayland_Wl_Data_Device_Error_Role:
		return LIT("Wayland_Wl_Data_Device_Error_Role");
	case Wayland_Wl_Data_Device_Error_Used_Source:
		return LIT("Wayland_Wl_Data_Device_Error_Used_Source");
	}
	return LIT("Wayland_Wl_Data_Device_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Data_Device_Manager_Dnd_Action_None = 0,
	Wayland_Wl_Data_Device_Manager_Dnd_Action_Copy = 1,
	Wayland_Wl_Data_Device_Manager_Dnd_Action_Move = 2,
	Wayland_Wl_Data_Device_Manager_Dnd_Action_Ask = 4,
} Wayland_Wl_Data_Device_Manager_Dnd_Action;

internal String wayland_wl_data_device_manager_dnd_action_string(Wayland_Wl_Data_Device_Manager_Dnd_Action v) {
	switch (v) {
	case Wayland_Wl_Data_Device_Manager_Dnd_Action_None:
		return LIT("Wayland_Wl_Data_Device_Manager_Dnd_Action_None");
	case Wayland_Wl_Data_Device_Manager_Dnd_Action_Copy:
		return LIT("Wayland_Wl_Data_Device_Manager_Dnd_Action_Copy");
	case Wayland_Wl_Data_Device_Manager_Dnd_Action_Move:
		return LIT("Wayland_Wl_Data_Device_Manager_Dnd_Action_Move");
	case Wayland_Wl_Data_Device_Manager_Dnd_Action_Ask:
		return LIT("Wayland_Wl_Data_Device_Manager_Dnd_Action_Ask");
	}
	return LIT("Wayland_Wl_Data_Device_Manager_Dnd_Action_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shell_Error_Role = 0,
} Wayland_Wl_Shell_Error;

internal String wayland_wl_shell_error_string(Wayland_Wl_Shell_Error v) {
	switch (v) {
	case Wayland_Wl_Shell_Error_Role:
		return LIT("Wayland_Wl_Shell_Error_Role");
	}
	return LIT("Wayland_Wl_Shell_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shell_Surface_Resize_None = 0,
	Wayland_Wl_Shell_Surface_Resize_Top = 1,
	Wayland_Wl_Shell_Surface_Resize_Bottom = 2,
	Wayland_Wl_Shell_Surface_Resize_Left = 4,
	Wayland_Wl_Shell_Surface_Resize_Top_Left = 5,
	Wayland_Wl_Shell_Surface_Resize_Bottom_Left = 6,
	Wayland_Wl_Shell_Surface_Resize_Right = 8,
	Wayland_Wl_Shell_Surface_Resize_Top_Right = 9,
	Wayland_Wl_Shell_Surface_Resize_Bottom_Right = 10,
} Wayland_Wl_Shell_Surface_Resize;

internal String wayland_wl_shell_surface_resize_string(Wayland_Wl_Shell_Surface_Resize v) {
	switch (v) {
	case Wayland_Wl_Shell_Surface_Resize_None:
		return LIT("Wayland_Wl_Shell_Surface_Resize_None");
	case Wayland_Wl_Shell_Surface_Resize_Top:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Top");
	case Wayland_Wl_Shell_Surface_Resize_Bottom:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Bottom");
	case Wayland_Wl_Shell_Surface_Resize_Left:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Left");
	case Wayland_Wl_Shell_Surface_Resize_Top_Left:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Top_Left");
	case Wayland_Wl_Shell_Surface_Resize_Bottom_Left:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Bottom_Left");
	case Wayland_Wl_Shell_Surface_Resize_Right:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Right");
	case Wayland_Wl_Shell_Surface_Resize_Top_Right:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Top_Right");
	case Wayland_Wl_Shell_Surface_Resize_Bottom_Right:
		return LIT("Wayland_Wl_Shell_Surface_Resize_Bottom_Right");
	}
	return LIT("Wayland_Wl_Shell_Surface_Resize_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shell_Surface_Transient_Inactive = 0x1,
} Wayland_Wl_Shell_Surface_Transient;

internal String wayland_wl_shell_surface_transient_string(Wayland_Wl_Shell_Surface_Transient v) {
	switch (v) {
	case Wayland_Wl_Shell_Surface_Transient_Inactive:
		return LIT("Wayland_Wl_Shell_Surface_Transient_Inactive");
	}
	return LIT("Wayland_Wl_Shell_Surface_Transient_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Shell_Surface_Fullscreen_Method_Default = 0,
	Wayland_Wl_Shell_Surface_Fullscreen_Method_Scale = 1,
	Wayland_Wl_Shell_Surface_Fullscreen_Method_Driver = 2,
	Wayland_Wl_Shell_Surface_Fullscreen_Method_Fill = 3,
} Wayland_Wl_Shell_Surface_Fullscreen_Method;

internal String wayland_wl_shell_surface_fullscreen_method_string(Wayland_Wl_Shell_Surface_Fullscreen_Method v) {
	switch (v) {
	case Wayland_Wl_Shell_Surface_Fullscreen_Method_Default:
		return LIT("Wayland_Wl_Shell_Surface_Fullscreen_Method_Default");
	case Wayland_Wl_Shell_Surface_Fullscreen_Method_Scale:
		return LIT("Wayland_Wl_Shell_Surface_Fullscreen_Method_Scale");
	case Wayland_Wl_Shell_Surface_Fullscreen_Method_Driver:
		return LIT("Wayland_Wl_Shell_Surface_Fullscreen_Method_Driver");
	case Wayland_Wl_Shell_Surface_Fullscreen_Method_Fill:
		return LIT("Wayland_Wl_Shell_Surface_Fullscreen_Method_Fill");
	}
	return LIT("Wayland_Wl_Shell_Surface_Fullscreen_Method_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Surface_Error_Invalid_Scale = 0,
	Wayland_Wl_Surface_Error_Invalid_Transform = 1,
	Wayland_Wl_Surface_Error_Invalid_Size = 2,
	Wayland_Wl_Surface_Error_Invalid_Offset = 3,
	Wayland_Wl_Surface_Error_Defunct_Role_Object = 4,
} Wayland_Wl_Surface_Error;

internal String wayland_wl_surface_error_string(Wayland_Wl_Surface_Error v) {
	switch (v) {
	case Wayland_Wl_Surface_Error_Invalid_Scale:
		return LIT("Wayland_Wl_Surface_Error_Invalid_Scale");
	case Wayland_Wl_Surface_Error_Invalid_Transform:
		return LIT("Wayland_Wl_Surface_Error_Invalid_Transform");
	case Wayland_Wl_Surface_Error_Invalid_Size:
		return LIT("Wayland_Wl_Surface_Error_Invalid_Size");
	case Wayland_Wl_Surface_Error_Invalid_Offset:
		return LIT("Wayland_Wl_Surface_Error_Invalid_Offset");
	case Wayland_Wl_Surface_Error_Defunct_Role_Object:
		return LIT("Wayland_Wl_Surface_Error_Defunct_Role_Object");
	}
	return LIT("Wayland_Wl_Surface_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Seat_Capability_Pointer = 1,
	Wayland_Wl_Seat_Capability_Keyboard = 2,
	Wayland_Wl_Seat_Capability_Touch = 4,
} Wayland_Wl_Seat_Capability;

internal String wayland_wl_seat_capability_string(Wayland_Wl_Seat_Capability v) {
	switch (v) {
	case Wayland_Wl_Seat_Capability_Pointer:
		return LIT("Wayland_Wl_Seat_Capability_Pointer");
	case Wayland_Wl_Seat_Capability_Keyboard:
		return LIT("Wayland_Wl_Seat_Capability_Keyboard");
	case Wayland_Wl_Seat_Capability_Touch:
		return LIT("Wayland_Wl_Seat_Capability_Touch");
	}
	return LIT("Wayland_Wl_Seat_Capability_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Seat_Error_Missing_Capability = 0,
} Wayland_Wl_Seat_Error;

internal String wayland_wl_seat_error_string(Wayland_Wl_Seat_Error v) {
	switch (v) {
	case Wayland_Wl_Seat_Error_Missing_Capability:
		return LIT("Wayland_Wl_Seat_Error_Missing_Capability");
	}
	return LIT("Wayland_Wl_Seat_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Pointer_Error_Role = 0,
} Wayland_Wl_Pointer_Error;

internal String wayland_wl_pointer_error_string(Wayland_Wl_Pointer_Error v) {
	switch (v) {
	case Wayland_Wl_Pointer_Error_Role:
		return LIT("Wayland_Wl_Pointer_Error_Role");
	}
	return LIT("Wayland_Wl_Pointer_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Pointer_Button_State_Released = 0,
	Wayland_Wl_Pointer_Button_State_Pressed = 1,
} Wayland_Wl_Pointer_Button_State;

internal String wayland_wl_pointer_button_state_string(Wayland_Wl_Pointer_Button_State v) {
	switch (v) {
	case Wayland_Wl_Pointer_Button_State_Released:
		return LIT("Wayland_Wl_Pointer_Button_State_Released");
	case Wayland_Wl_Pointer_Button_State_Pressed:
		return LIT("Wayland_Wl_Pointer_Button_State_Pressed");
	}
	return LIT("Wayland_Wl_Pointer_Button_State_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Pointer_Axis_Vertical_Scroll = 0,
	Wayland_Wl_Pointer_Axis_Horizontal_Scroll = 1,
} Wayland_Wl_Pointer_Axis;

internal String wayland_wl_pointer_axis_string(Wayland_Wl_Pointer_Axis v) {
	switch (v) {
	case Wayland_Wl_Pointer_Axis_Vertical_Scroll:
		return LIT("Wayland_Wl_Pointer_Axis_Vertical_Scroll");
	case Wayland_Wl_Pointer_Axis_Horizontal_Scroll:
		return LIT("Wayland_Wl_Pointer_Axis_Horizontal_Scroll");
	}
	return LIT("Wayland_Wl_Pointer_Axis_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Pointer_Axis_Source_Wheel = 0,
	Wayland_Wl_Pointer_Axis_Source_Finger = 1,
	Wayland_Wl_Pointer_Axis_Source_Continuous = 2,
	Wayland_Wl_Pointer_Axis_Source_Wheel_Tilt = 3,
} Wayland_Wl_Pointer_Axis_Source;

internal String wayland_wl_pointer_axis_source_string(Wayland_Wl_Pointer_Axis_Source v) {
	switch (v) {
	case Wayland_Wl_Pointer_Axis_Source_Wheel:
		return LIT("Wayland_Wl_Pointer_Axis_Source_Wheel");
	case Wayland_Wl_Pointer_Axis_Source_Finger:
		return LIT("Wayland_Wl_Pointer_Axis_Source_Finger");
	case Wayland_Wl_Pointer_Axis_Source_Continuous:
		return LIT("Wayland_Wl_Pointer_Axis_Source_Continuous");
	case Wayland_Wl_Pointer_Axis_Source_Wheel_Tilt:
		return LIT("Wayland_Wl_Pointer_Axis_Source_Wheel_Tilt");
	}
	return LIT("Wayland_Wl_Pointer_Axis_Source_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Pointer_Axis_Relative_Direction_Identical = 0,
	Wayland_Wl_Pointer_Axis_Relative_Direction_Inverted = 1,
} Wayland_Wl_Pointer_Axis_Relative_Direction;

internal String wayland_wl_pointer_axis_relative_direction_string(Wayland_Wl_Pointer_Axis_Relative_Direction v) {
	switch (v) {
	case Wayland_Wl_Pointer_Axis_Relative_Direction_Identical:
		return LIT("Wayland_Wl_Pointer_Axis_Relative_Direction_Identical");
	case Wayland_Wl_Pointer_Axis_Relative_Direction_Inverted:
		return LIT("Wayland_Wl_Pointer_Axis_Relative_Direction_Inverted");
	}
	return LIT("Wayland_Wl_Pointer_Axis_Relative_Direction_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Keyboard_Keymap_Format_No_Keymap = 0,
	Wayland_Wl_Keyboard_Keymap_Format_Xkb_V1 = 1,
} Wayland_Wl_Keyboard_Keymap_Format;

internal String wayland_wl_keyboard_keymap_format_string(Wayland_Wl_Keyboard_Keymap_Format v) {
	switch (v) {
	case Wayland_Wl_Keyboard_Keymap_Format_No_Keymap:
		return LIT("Wayland_Wl_Keyboard_Keymap_Format_No_Keymap");
	case Wayland_Wl_Keyboard_Keymap_Format_Xkb_V1:
		return LIT("Wayland_Wl_Keyboard_Keymap_Format_Xkb_V1");
	}
	return LIT("Wayland_Wl_Keyboard_Keymap_Format_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Keyboard_Key_State_Released = 0,
	Wayland_Wl_Keyboard_Key_State_Pressed = 1,
	Wayland_Wl_Keyboard_Key_State_Repeated = 2,
} Wayland_Wl_Keyboard_Key_State;

internal String wayland_wl_keyboard_key_state_string(Wayland_Wl_Keyboard_Key_State v) {
	switch (v) {
	case Wayland_Wl_Keyboard_Key_State_Released:
		return LIT("Wayland_Wl_Keyboard_Key_State_Released");
	case Wayland_Wl_Keyboard_Key_State_Pressed:
		return LIT("Wayland_Wl_Keyboard_Key_State_Pressed");
	case Wayland_Wl_Keyboard_Key_State_Repeated:
		return LIT("Wayland_Wl_Keyboard_Key_State_Repeated");
	}
	return LIT("Wayland_Wl_Keyboard_Key_State_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Output_Subpixel_Unknown = 0,
	Wayland_Wl_Output_Subpixel_None = 1,
	Wayland_Wl_Output_Subpixel_Horizontal_Rgb = 2,
	Wayland_Wl_Output_Subpixel_Horizontal_Bgr = 3,
	Wayland_Wl_Output_Subpixel_Vertical_Rgb = 4,
	Wayland_Wl_Output_Subpixel_Vertical_Bgr = 5,
} Wayland_Wl_Output_Subpixel;

internal String wayland_wl_output_subpixel_string(Wayland_Wl_Output_Subpixel v) {
	switch (v) {
	case Wayland_Wl_Output_Subpixel_Unknown:
		return LIT("Wayland_Wl_Output_Subpixel_Unknown");
	case Wayland_Wl_Output_Subpixel_None:
		return LIT("Wayland_Wl_Output_Subpixel_None");
	case Wayland_Wl_Output_Subpixel_Horizontal_Rgb:
		return LIT("Wayland_Wl_Output_Subpixel_Horizontal_Rgb");
	case Wayland_Wl_Output_Subpixel_Horizontal_Bgr:
		return LIT("Wayland_Wl_Output_Subpixel_Horizontal_Bgr");
	case Wayland_Wl_Output_Subpixel_Vertical_Rgb:
		return LIT("Wayland_Wl_Output_Subpixel_Vertical_Rgb");
	case Wayland_Wl_Output_Subpixel_Vertical_Bgr:
		return LIT("Wayland_Wl_Output_Subpixel_Vertical_Bgr");
	}
	return LIT("Wayland_Wl_Output_Subpixel_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Output_Transform_Normal = 0,
	Wayland_Wl_Output_Transform_90 = 1,
	Wayland_Wl_Output_Transform_180 = 2,
	Wayland_Wl_Output_Transform_270 = 3,
	Wayland_Wl_Output_Transform_Flipped = 4,
	Wayland_Wl_Output_Transform_Flipped_90 = 5,
	Wayland_Wl_Output_Transform_Flipped_180 = 6,
	Wayland_Wl_Output_Transform_Flipped_270 = 7,
} Wayland_Wl_Output_Transform;

internal String wayland_wl_output_transform_string(Wayland_Wl_Output_Transform v) {
	switch (v) {
	case Wayland_Wl_Output_Transform_Normal:
		return LIT("Wayland_Wl_Output_Transform_Normal");
	case Wayland_Wl_Output_Transform_90:
		return LIT("Wayland_Wl_Output_Transform_90");
	case Wayland_Wl_Output_Transform_180:
		return LIT("Wayland_Wl_Output_Transform_180");
	case Wayland_Wl_Output_Transform_270:
		return LIT("Wayland_Wl_Output_Transform_270");
	case Wayland_Wl_Output_Transform_Flipped:
		return LIT("Wayland_Wl_Output_Transform_Flipped");
	case Wayland_Wl_Output_Transform_Flipped_90:
		return LIT("Wayland_Wl_Output_Transform_Flipped_90");
	case Wayland_Wl_Output_Transform_Flipped_180:
		return LIT("Wayland_Wl_Output_Transform_Flipped_180");
	case Wayland_Wl_Output_Transform_Flipped_270:
		return LIT("Wayland_Wl_Output_Transform_Flipped_270");
	}
	return LIT("Wayland_Wl_Output_Transform_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Output_Mode_Current = 0x1,
	Wayland_Wl_Output_Mode_Preferred = 0x2,
} Wayland_Wl_Output_Mode;

internal String wayland_wl_output_mode_string(Wayland_Wl_Output_Mode v) {
	switch (v) {
	case Wayland_Wl_Output_Mode_Current:
		return LIT("Wayland_Wl_Output_Mode_Current");
	case Wayland_Wl_Output_Mode_Preferred:
		return LIT("Wayland_Wl_Output_Mode_Preferred");
	}
	return LIT("Wayland_Wl_Output_Mode_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Subcompositor_Error_Bad_Surface = 0,
	Wayland_Wl_Subcompositor_Error_Bad_Parent = 1,
} Wayland_Wl_Subcompositor_Error;

internal String wayland_wl_subcompositor_error_string(Wayland_Wl_Subcompositor_Error v) {
	switch (v) {
	case Wayland_Wl_Subcompositor_Error_Bad_Surface:
		return LIT("Wayland_Wl_Subcompositor_Error_Bad_Surface");
	case Wayland_Wl_Subcompositor_Error_Bad_Parent:
		return LIT("Wayland_Wl_Subcompositor_Error_Bad_Parent");
	}
	return LIT("Wayland_Wl_Subcompositor_Error_Invalid_Enum_Value");
}

typedef enum {
	Wayland_Wl_Subsurface_Error_Bad_Surface = 0,
} Wayland_Wl_Subsurface_Error;

internal String wayland_wl_subsurface_error_string(Wayland_Wl_Subsurface_Error v) {
	switch (v) {
	case Wayland_Wl_Subsurface_Error_Bad_Surface:
		return LIT("Wayland_Wl_Subsurface_Error_Bad_Surface");
	}
	return LIT("Wayland_Wl_Subsurface_Error_Invalid_Enum_Value");
}

internal u32 wayland_wl_display_sync(Wayland_Connection *wc, u32 wl_display) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_display);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_display@%d.sync: callback=%d"), wl_display, return_value);
	return return_value;
}

internal u32 wayland_wl_display_get_registry(Wayland_Connection *wc, u32 wl_display) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_display);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_display@%d.get_registry: registry=%d"), wl_display, return_value);
	return return_value;
}

typedef enum {
	Wayland_Wl_Display_Event_Error = 0,
	Wayland_Wl_Display_Event_Delete_Id = 1,
} Wayland_Wl_Display_Event;

internal isize wayland_parse_event_wl_display_error(Wayland_Connection *conn, u32 *object_id, u32 *code, String *message, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, object_id), -1);
	or_return(read_any(&r, code), -1);
	u32 message_len;
	or_return(read_any(&r, &message_len), -1);
	slice_init(message, message_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*message)), -1);
	message->len -= 1;
	if (message_len % 4) {
		byte message_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = message_pad_buf, .len = 4 - (message_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_display@%d.error: object_id=%d code=%d message=%S"), _object_id, *object_id, *code, *message);
	return _msg_size;
}

internal isize wayland_parse_event_wl_display_delete_id(Wayland_Connection *conn, u32 *id) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, id), -1);
	wayland_log_infof(LIT("<- wl_display@%d.delete_id: id=%d"), _object_id, *id);
	return _msg_size;
}

internal u32 wayland_wl_registry_bind(Wayland_Connection *wc, u32 wl_registry, u32 name, String interface, u32 version) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_registry);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 24;
	isize interface_size = roundup_4(interface.len + 1);
	_msg_size += interface_size;

	write_any(w, &_msg_size);
	write_any(w, &name);
	i32 interface_write_len = (i32)interface.len + 1;
	write_any(w, &interface_write_len);
	write_string(w, interface);
	for_range(i, interface.len, interface_size) {
		write_byte(w, 0);
	}
	write_any(w, &version);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_registry@%d.bind: name=%d interface=%S version=%d id=%d"), wl_registry, name, interface, version, return_value);
	return return_value;
}

typedef enum {
	Wayland_Wl_Registry_Event_Global = 0,
	Wayland_Wl_Registry_Event_Global_Remove = 1,
} Wayland_Wl_Registry_Event;

internal isize wayland_parse_event_wl_registry_global(Wayland_Connection *conn, u32 *name, String *interface, u32 *version, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, name), -1);
	u32 interface_len;
	or_return(read_any(&r, &interface_len), -1);
	slice_init(interface, interface_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*interface)), -1);
	interface->len -= 1;
	if (interface_len % 4) {
		byte interface_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = interface_pad_buf, .len = 4 - (interface_len % 4) }), -1);
	}
	or_return(read_any(&r, version), -1);
	wayland_log_infof(LIT("<- wl_registry@%d.global: name=%d interface=%S version=%d"), _object_id, *name, *interface, *version);
	return _msg_size;
}

internal isize wayland_parse_event_wl_registry_global_remove(Wayland_Connection *conn, u32 *name) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, name), -1);
	wayland_log_infof(LIT("<- wl_registry@%d.global_remove: name=%d"), _object_id, *name);
	return _msg_size;
}

typedef enum {
	Wayland_Wl_Callback_Event_Done = 0,
} Wayland_Wl_Callback_Event;

internal isize wayland_parse_event_wl_callback_done(Wayland_Connection *conn, u32 *callback_data) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, callback_data), -1);
	wayland_log_infof(LIT("<- wl_callback@%d.done: callback_data=%d"), _object_id, *callback_data);
	return _msg_size;
}

internal u32 wayland_wl_compositor_create_surface(Wayland_Connection *wc, u32 wl_compositor) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_compositor);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_compositor@%d.create_surface: id=%d"), wl_compositor, return_value);
	return return_value;
}

internal u32 wayland_wl_compositor_create_region(Wayland_Connection *wc, u32 wl_compositor) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_compositor);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_compositor@%d.create_region: id=%d"), wl_compositor, return_value);
	return return_value;
}

internal u32 wayland_wl_shm_pool_create_buffer(Wayland_Connection *wc, u32 wl_shm_pool, i32 offset, i32 width, i32 height, i32 stride, Wayland_Wl_Shm_Format format) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shm_pool);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 32;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &offset);
	write_any(w, &width);
	write_any(w, &height);
	write_any(w, &stride);
	u32 format_value = (u32)format;
	write_any(w, &format_value);

	wayland_log_infof(LIT("-> wl_shm_pool@%d.create_buffer: id=%d offset=%d width=%d height=%d stride=%d format=%S"), wl_shm_pool, return_value, offset, width, height, stride, wayland_wl_shm_format_string(format));
	return return_value;
}

internal void wayland_wl_shm_pool_destroy(Wayland_Connection *wc, u32 wl_shm_pool) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shm_pool);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_shm_pool@%d.destroy:"), wl_shm_pool);
}

internal void wayland_wl_shm_pool_resize(Wayland_Connection *wc, u32 wl_shm_pool, i32 size) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shm_pool);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &size);

	wayland_log_infof(LIT("-> wl_shm_pool@%d.resize: size=%d"), wl_shm_pool, size);
}

internal u32 wayland_wl_shm_create_pool(Wayland_Connection *wc, u32 wl_shm, Fd fd, i32 size) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shm);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	vector_append(&wc->fds_out, fd);
	write_any(w, &size);

	wayland_log_infof(LIT("-> wl_shm@%d.create_pool: id=%d fd=%d size=%d"), wl_shm, return_value, fd, size);
	return return_value;
}

internal void wayland_wl_shm_release(Wayland_Connection *wc, u32 wl_shm) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shm);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_shm@%d.release:"), wl_shm);
}

typedef enum {
	Wayland_Wl_Shm_Event_Format = 0,
} Wayland_Wl_Shm_Event;

internal isize wayland_parse_event_wl_shm_format(Wayland_Connection *conn, Wayland_Wl_Shm_Format *format) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	u32 format_value;
	or_return(read_any(&r, &format_value), -1);
	*format = (Wayland_Wl_Shm_Format)format_value;
	wayland_log_infof(LIT("<- wl_shm@%d.format: format=%S"), _object_id, wayland_wl_shm_format_string(*format));
	return _msg_size;
}

internal void wayland_wl_buffer_destroy(Wayland_Connection *wc, u32 wl_buffer) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_buffer);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_buffer@%d.destroy:"), wl_buffer);
}

typedef enum {
	Wayland_Wl_Buffer_Event_Release = 0,
} Wayland_Wl_Buffer_Event;

internal isize wayland_parse_event_wl_buffer_release(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	wayland_log_infof(LIT("<- wl_buffer@%d.release:"), _object_id);
	return _msg_size;
}

internal void wayland_wl_data_offer_accept(Wayland_Connection *wc, u32 wl_data_offer, u32 serial, String mime_type) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_offer);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 16;
	isize mime_type_size = roundup_4(mime_type.len + 1);
	_msg_size += mime_type_size;

	write_any(w, &_msg_size);
	write_any(w, &serial);
	i32 mime_type_write_len = (i32)mime_type.len + 1;
	write_any(w, &mime_type_write_len);
	write_string(w, mime_type);
	for_range(i, mime_type.len, mime_type_size) {
		write_byte(w, 0);
	}

	wayland_log_infof(LIT("-> wl_data_offer@%d.accept: serial=%d mime_type=%S"), wl_data_offer, serial, mime_type);
}

internal void wayland_wl_data_offer_receive(Wayland_Connection *wc, u32 wl_data_offer, String mime_type, Fd fd) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_offer);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;
	isize mime_type_size = roundup_4(mime_type.len + 1);
	_msg_size += mime_type_size;

	write_any(w, &_msg_size);
	i32 mime_type_write_len = (i32)mime_type.len + 1;
	write_any(w, &mime_type_write_len);
	write_string(w, mime_type);
	for_range(i, mime_type.len, mime_type_size) {
		write_byte(w, 0);
	}
	vector_append(&wc->fds_out, fd);

	wayland_log_infof(LIT("-> wl_data_offer@%d.receive: mime_type=%S fd=%d"), wl_data_offer, mime_type, fd);
}

internal void wayland_wl_data_offer_destroy(Wayland_Connection *wc, u32 wl_data_offer) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_offer);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_data_offer@%d.destroy:"), wl_data_offer);
}

internal void wayland_wl_data_offer_finish(Wayland_Connection *wc, u32 wl_data_offer) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_offer);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_data_offer@%d.finish:"), wl_data_offer);
}

internal void wayland_wl_data_offer_set_actions(Wayland_Connection *wc, u32 wl_data_offer, Wayland_Wl_Data_Device_Manager_Dnd_Action dnd_actions, Wayland_Wl_Data_Device_Manager_Dnd_Action preferred_action) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_offer);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	u32 dnd_actions_value = (u32)dnd_actions;
	write_any(w, &dnd_actions_value);
	u32 preferred_action_value = (u32)preferred_action;
	write_any(w, &preferred_action_value);

	wayland_log_infof(LIT("-> wl_data_offer@%d.set_actions: dnd_actions=%S preferred_action=%S"), wl_data_offer, wayland_wl_data_device_manager_dnd_action_string(dnd_actions), wayland_wl_data_device_manager_dnd_action_string(preferred_action));
}

typedef enum {
	Wayland_Wl_Data_Offer_Event_Offer = 0,
	Wayland_Wl_Data_Offer_Event_Source_Actions = 1,
	Wayland_Wl_Data_Offer_Event_Action = 2,
} Wayland_Wl_Data_Offer_Event;

internal isize wayland_parse_event_wl_data_offer_offer(Wayland_Connection *conn, String *mime_type, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	u32 mime_type_len;
	or_return(read_any(&r, &mime_type_len), -1);
	slice_init(mime_type, mime_type_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*mime_type)), -1);
	mime_type->len -= 1;
	if (mime_type_len % 4) {
		byte mime_type_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = mime_type_pad_buf, .len = 4 - (mime_type_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_data_offer@%d.offer: mime_type=%S"), _object_id, *mime_type);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_offer_source_actions(Wayland_Connection *conn, Wayland_Wl_Data_Device_Manager_Dnd_Action *source_actions) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	u32 source_actions_value;
	or_return(read_any(&r, &source_actions_value), -1);
	*source_actions = (Wayland_Wl_Data_Device_Manager_Dnd_Action)source_actions_value;
	wayland_log_infof(LIT("<- wl_data_offer@%d.source_actions: source_actions=%S"), _object_id, wayland_wl_data_device_manager_dnd_action_string(*source_actions));
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_offer_action(Wayland_Connection *conn, Wayland_Wl_Data_Device_Manager_Dnd_Action *dnd_action) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	u32 dnd_action_value;
	or_return(read_any(&r, &dnd_action_value), -1);
	*dnd_action = (Wayland_Wl_Data_Device_Manager_Dnd_Action)dnd_action_value;
	wayland_log_infof(LIT("<- wl_data_offer@%d.action: dnd_action=%S"), _object_id, wayland_wl_data_device_manager_dnd_action_string(*dnd_action));
	return _msg_size;
}

internal void wayland_wl_data_source_offer(Wayland_Connection *wc, u32 wl_data_source, String mime_type) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_source);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;
	isize mime_type_size = roundup_4(mime_type.len + 1);
	_msg_size += mime_type_size;

	write_any(w, &_msg_size);
	i32 mime_type_write_len = (i32)mime_type.len + 1;
	write_any(w, &mime_type_write_len);
	write_string(w, mime_type);
	for_range(i, mime_type.len, mime_type_size) {
		write_byte(w, 0);
	}

	wayland_log_infof(LIT("-> wl_data_source@%d.offer: mime_type=%S"), wl_data_source, mime_type);
}

internal void wayland_wl_data_source_destroy(Wayland_Connection *wc, u32 wl_data_source) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_source);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_data_source@%d.destroy:"), wl_data_source);
}

internal void wayland_wl_data_source_set_actions(Wayland_Connection *wc, u32 wl_data_source, Wayland_Wl_Data_Device_Manager_Dnd_Action dnd_actions) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_source);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 dnd_actions_value = (u32)dnd_actions;
	write_any(w, &dnd_actions_value);

	wayland_log_infof(LIT("-> wl_data_source@%d.set_actions: dnd_actions=%S"), wl_data_source, wayland_wl_data_device_manager_dnd_action_string(dnd_actions));
}

typedef enum {
	Wayland_Wl_Data_Source_Event_Target = 0,
	Wayland_Wl_Data_Source_Event_Send = 1,
	Wayland_Wl_Data_Source_Event_Cancelled = 2,
	Wayland_Wl_Data_Source_Event_Dnd_Drop_Performed = 3,
	Wayland_Wl_Data_Source_Event_Dnd_Finished = 4,
	Wayland_Wl_Data_Source_Event_Action = 5,
} Wayland_Wl_Data_Source_Event;

internal isize wayland_parse_event_wl_data_source_target(Wayland_Connection *conn, String *mime_type, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	u32 mime_type_len;
	or_return(read_any(&r, &mime_type_len), -1);
	slice_init(mime_type, mime_type_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*mime_type)), -1);
	mime_type->len -= 1;
	if (mime_type_len % 4) {
		byte mime_type_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = mime_type_pad_buf, .len = 4 - (mime_type_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_data_source@%d.target: mime_type=%S"), _object_id, *mime_type);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_source_send(Wayland_Connection *conn, String *mime_type, Fd *fd, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	u32 mime_type_len;
	or_return(read_any(&r, &mime_type_len), -1);
	slice_init(mime_type, mime_type_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*mime_type)), -1);
	mime_type->len -= 1;
	if (mime_type_len % 4) {
		byte mime_type_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = mime_type_pad_buf, .len = 4 - (mime_type_len % 4) }), -1);
	}
	*fd = conn->fds_in.data[0];
	vector_remove_ordered(&conn->fds_in, 0);
	wayland_log_infof(LIT("<- wl_data_source@%d.send: mime_type=%S fd=%d"), _object_id, *mime_type, *fd);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_source_cancelled(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	wayland_log_infof(LIT("<- wl_data_source@%d.cancelled:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_source_dnd_drop_performed(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	wayland_log_infof(LIT("<- wl_data_source@%d.dnd_drop_performed:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_source_dnd_finished(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	wayland_log_infof(LIT("<- wl_data_source@%d.dnd_finished:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_source_action(Wayland_Connection *conn, Wayland_Wl_Data_Device_Manager_Dnd_Action *dnd_action) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	u32 dnd_action_value;
	or_return(read_any(&r, &dnd_action_value), -1);
	*dnd_action = (Wayland_Wl_Data_Device_Manager_Dnd_Action)dnd_action_value;
	wayland_log_infof(LIT("<- wl_data_source@%d.action: dnd_action=%S"), _object_id, wayland_wl_data_device_manager_dnd_action_string(*dnd_action));
	return _msg_size;
}

internal void wayland_wl_data_device_start_drag(Wayland_Connection *wc, u32 wl_data_device, u32 source, u32 origin, u32 icon, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_device);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &source);
	write_any(w, &origin);
	write_any(w, &icon);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> wl_data_device@%d.start_drag: source=%d origin=%d icon=%d serial=%d"), wl_data_device, source, origin, icon, serial);
}

internal void wayland_wl_data_device_set_selection(Wayland_Connection *wc, u32 wl_data_device, u32 source, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_device);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &source);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> wl_data_device@%d.set_selection: source=%d serial=%d"), wl_data_device, source, serial);
}

internal void wayland_wl_data_device_release(Wayland_Connection *wc, u32 wl_data_device) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_device);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_data_device@%d.release:"), wl_data_device);
}

typedef enum {
	Wayland_Wl_Data_Device_Event_Data_Offer = 0,
	Wayland_Wl_Data_Device_Event_Enter = 1,
	Wayland_Wl_Data_Device_Event_Leave = 2,
	Wayland_Wl_Data_Device_Event_Motion = 3,
	Wayland_Wl_Data_Device_Event_Drop = 4,
	Wayland_Wl_Data_Device_Event_Selection = 5,
} Wayland_Wl_Data_Device_Event;

internal isize wayland_parse_event_wl_data_device_data_offer(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	wayland_log_infof(LIT("<- wl_data_device@%d.data_offer:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_device_enter(Wayland_Connection *conn, u32 *serial, u32 *surface, f32 *x, f32 *y, u32 *id) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, surface), -1);
	i32 x_fixed;
	or_return(read_any(&r, &x_fixed), -1);
	*x = (f64)x_fixed / 256.0;
	i32 y_fixed;
	or_return(read_any(&r, &y_fixed), -1);
	*y = (f64)y_fixed / 256.0;
	or_return(read_any(&r, id), -1);
	wayland_log_infof(LIT("<- wl_data_device@%d.enter: serial=%d surface=%d x=%f y=%f id=%d"), _object_id, *serial, *surface, *x, *y, *id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_device_leave(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	wayland_log_infof(LIT("<- wl_data_device@%d.leave:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_device_motion(Wayland_Connection *conn, u32 *time, f32 *x, f32 *y) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	or_return(read_any(&r, time), -1);
	i32 x_fixed;
	or_return(read_any(&r, &x_fixed), -1);
	*x = (f64)x_fixed / 256.0;
	i32 y_fixed;
	or_return(read_any(&r, &y_fixed), -1);
	*y = (f64)y_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_data_device@%d.motion: time=%d x=%f y=%f"), _object_id, *time, *x, *y);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_device_drop(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	wayland_log_infof(LIT("<- wl_data_device@%d.drop:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_data_device_selection(Wayland_Connection *conn, u32 *id) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	or_return(read_any(&r, id), -1);
	wayland_log_infof(LIT("<- wl_data_device@%d.selection: id=%d"), _object_id, *id);
	return _msg_size;
}

internal u32 wayland_wl_data_device_manager_create_data_source(Wayland_Connection *wc, u32 wl_data_device_manager) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_device_manager);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_data_device_manager@%d.create_data_source: id=%d"), wl_data_device_manager, return_value);
	return return_value;
}

internal u32 wayland_wl_data_device_manager_get_data_device(Wayland_Connection *wc, u32 wl_data_device_manager, u32 seat) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_data_device_manager);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &seat);

	wayland_log_infof(LIT("-> wl_data_device_manager@%d.get_data_device: id=%d seat=%d"), wl_data_device_manager, return_value, seat);
	return return_value;
}

internal u32 wayland_wl_shell_get_shell_surface(Wayland_Connection *wc, u32 wl_shell, u32 surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &surface);

	wayland_log_infof(LIT("-> wl_shell@%d.get_shell_surface: id=%d surface=%d"), wl_shell, return_value, surface);
	return return_value;
}

internal void wayland_wl_shell_surface_pong(Wayland_Connection *wc, u32 wl_shell_surface, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.pong: serial=%d"), wl_shell_surface, serial);
}

internal void wayland_wl_shell_surface_move(Wayland_Connection *wc, u32 wl_shell_surface, u32 seat, u32 serial) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.move: seat=%d serial=%d"), wl_shell_surface, seat, serial);
}

internal void wayland_wl_shell_surface_resize(Wayland_Connection *wc, u32 wl_shell_surface, u32 seat, u32 serial, Wayland_Wl_Shell_Surface_Resize edges) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);
	u32 edges_value = (u32)edges;
	write_any(w, &edges_value);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.resize: seat=%d serial=%d edges=%S"), wl_shell_surface, seat, serial, wayland_wl_shell_surface_resize_string(edges));
}

internal void wayland_wl_shell_surface_set_toplevel(Wayland_Connection *wc, u32 wl_shell_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_toplevel:"), wl_shell_surface);
}

internal void wayland_wl_shell_surface_set_transient(Wayland_Connection *wc, u32 wl_shell_surface, u32 parent, i32 x, i32 y, Wayland_Wl_Shell_Surface_Transient flags) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &parent);
	write_any(w, &x);
	write_any(w, &y);
	u32 flags_value = (u32)flags;
	write_any(w, &flags_value);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_transient: parent=%d x=%d y=%d flags=%S"), wl_shell_surface, parent, x, y, wayland_wl_shell_surface_transient_string(flags));
}

internal void wayland_wl_shell_surface_set_fullscreen(Wayland_Connection *wc, u32 wl_shell_surface, Wayland_Wl_Shell_Surface_Fullscreen_Method method, u32 framerate, u32 output) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	u32 method_value = (u32)method;
	write_any(w, &method_value);
	write_any(w, &framerate);
	write_any(w, &output);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_fullscreen: method=%S framerate=%d output=%d"), wl_shell_surface, wayland_wl_shell_surface_fullscreen_method_string(method), framerate, output);
}

internal void wayland_wl_shell_surface_set_popup(Wayland_Connection *wc, u32 wl_shell_surface, u32 seat, u32 serial, u32 parent, i32 x, i32 y, Wayland_Wl_Shell_Surface_Transient flags) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 32;

	write_any(w, &_msg_size);
	write_any(w, &seat);
	write_any(w, &serial);
	write_any(w, &parent);
	write_any(w, &x);
	write_any(w, &y);
	u32 flags_value = (u32)flags;
	write_any(w, &flags_value);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_popup: seat=%d serial=%d parent=%d x=%d y=%d flags=%S"), wl_shell_surface, seat, serial, parent, x, y, wayland_wl_shell_surface_transient_string(flags));
}

internal void wayland_wl_shell_surface_set_maximized(Wayland_Connection *wc, u32 wl_shell_surface, u32 output) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &output);

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_maximized: output=%d"), wl_shell_surface, output);
}

internal void wayland_wl_shell_surface_set_title(Wayland_Connection *wc, u32 wl_shell_surface, String title) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 8;
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

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_title: title=%S"), wl_shell_surface, title);
}

internal void wayland_wl_shell_surface_set_class(Wayland_Connection *wc, u32 wl_shell_surface, String class_) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_shell_surface);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 12;
	isize class__size = roundup_4(class_.len + 1);
	_msg_size += class__size;

	write_any(w, &_msg_size);
	i32 class__write_len = (i32)class_.len + 1;
	write_any(w, &class__write_len);
	write_string(w, class_);
	for_range(i, class_.len, class__size) {
		write_byte(w, 0);
	}

	wayland_log_infof(LIT("-> wl_shell_surface@%d.set_class: class_=%S"), wl_shell_surface, class_);
}

typedef enum {
	Wayland_Wl_Shell_Surface_Event_Ping = 0,
	Wayland_Wl_Shell_Surface_Event_Configure = 1,
	Wayland_Wl_Shell_Surface_Event_Popup_Done = 2,
} Wayland_Wl_Shell_Surface_Event;

internal isize wayland_parse_event_wl_shell_surface_ping(Wayland_Connection *conn, u32 *serial) {
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
	wayland_log_infof(LIT("<- wl_shell_surface@%d.ping: serial=%d"), _object_id, *serial);
	return _msg_size;
}

internal isize wayland_parse_event_wl_shell_surface_configure(Wayland_Connection *conn, Wayland_Wl_Shell_Surface_Resize *edges, i32 *width, i32 *height) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	u32 edges_value;
	or_return(read_any(&r, &edges_value), -1);
	*edges = (Wayland_Wl_Shell_Surface_Resize)edges_value;
	or_return(read_any(&r, width), -1);
	or_return(read_any(&r, height), -1);
	wayland_log_infof(LIT("<- wl_shell_surface@%d.configure: edges=%S width=%d height=%d"), _object_id, wayland_wl_shell_surface_resize_string(*edges), *width, *height);
	return _msg_size;
}

internal isize wayland_parse_event_wl_shell_surface_popup_done(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	wayland_log_infof(LIT("<- wl_shell_surface@%d.popup_done:"), _object_id);
	return _msg_size;
}

internal void wayland_wl_surface_destroy(Wayland_Connection *wc, u32 wl_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_surface@%d.destroy:"), wl_surface);
}

internal void wayland_wl_surface_attach(Wayland_Connection *wc, u32 wl_surface, u32 buffer, i32 x, i32 y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	write_any(w, &buffer);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> wl_surface@%d.attach: buffer=%d x=%d y=%d"), wl_surface, buffer, x, y);
}

internal void wayland_wl_surface_damage(Wayland_Connection *wc, u32 wl_surface, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> wl_surface@%d.damage: x=%d y=%d width=%d height=%d"), wl_surface, x, y, width, height);
}

internal u32 wayland_wl_surface_frame(Wayland_Connection *wc, u32 wl_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_surface@%d.frame: callback=%d"), wl_surface, return_value);
	return return_value;
}

internal void wayland_wl_surface_set_opaque_region(Wayland_Connection *wc, u32 wl_surface, u32 region) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &region);

	wayland_log_infof(LIT("-> wl_surface@%d.set_opaque_region: region=%d"), wl_surface, region);
}

internal void wayland_wl_surface_set_input_region(Wayland_Connection *wc, u32 wl_surface, u32 region) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &region);

	wayland_log_infof(LIT("-> wl_surface@%d.set_input_region: region=%d"), wl_surface, region);
}

internal void wayland_wl_surface_commit(Wayland_Connection *wc, u32 wl_surface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 6;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_surface@%d.commit:"), wl_surface);
}

internal void wayland_wl_surface_set_buffer_transform(Wayland_Connection *wc, u32 wl_surface, Wayland_Wl_Output_Transform transform) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 7;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	u32 transform_value = (u32)transform;
	write_any(w, &transform_value);

	wayland_log_infof(LIT("-> wl_surface@%d.set_buffer_transform: transform=%S"), wl_surface, wayland_wl_output_transform_string(transform));
}

internal void wayland_wl_surface_set_buffer_scale(Wayland_Connection *wc, u32 wl_surface, i32 scale) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 8;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &scale);

	wayland_log_infof(LIT("-> wl_surface@%d.set_buffer_scale: scale=%d"), wl_surface, scale);
}

internal void wayland_wl_surface_damage_buffer(Wayland_Connection *wc, u32 wl_surface, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 9;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> wl_surface@%d.damage_buffer: x=%d y=%d width=%d height=%d"), wl_surface, x, y, width, height);
}

internal void wayland_wl_surface_offset(Wayland_Connection *wc, u32 wl_surface, i32 x, i32 y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_surface);
	u16 _opcode = 10;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> wl_surface@%d.offset: x=%d y=%d"), wl_surface, x, y);
}

typedef enum {
	Wayland_Wl_Surface_Event_Enter = 0,
	Wayland_Wl_Surface_Event_Leave = 1,
	Wayland_Wl_Surface_Event_Preferred_Buffer_Scale = 2,
	Wayland_Wl_Surface_Event_Preferred_Buffer_Transform = 3,
} Wayland_Wl_Surface_Event;

internal isize wayland_parse_event_wl_surface_enter(Wayland_Connection *conn, u32 *output) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	or_return(read_any(&r, output), -1);
	wayland_log_infof(LIT("<- wl_surface@%d.enter: output=%d"), _object_id, *output);
	return _msg_size;
}

internal isize wayland_parse_event_wl_surface_leave(Wayland_Connection *conn, u32 *output) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, output), -1);
	wayland_log_infof(LIT("<- wl_surface@%d.leave: output=%d"), _object_id, *output);
	return _msg_size;
}

internal isize wayland_parse_event_wl_surface_preferred_buffer_scale(Wayland_Connection *conn, i32 *factor) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, factor), -1);
	wayland_log_infof(LIT("<- wl_surface@%d.preferred_buffer_scale: factor=%d"), _object_id, *factor);
	return _msg_size;
}

internal isize wayland_parse_event_wl_surface_preferred_buffer_transform(Wayland_Connection *conn, Wayland_Wl_Output_Transform *transform) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	u32 transform_value;
	or_return(read_any(&r, &transform_value), -1);
	*transform = (Wayland_Wl_Output_Transform)transform_value;
	wayland_log_infof(LIT("<- wl_surface@%d.preferred_buffer_transform: transform=%S"), _object_id, wayland_wl_output_transform_string(*transform));
	return _msg_size;
}

internal u32 wayland_wl_seat_get_pointer(Wayland_Connection *wc, u32 wl_seat) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_seat);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_seat@%d.get_pointer: id=%d"), wl_seat, return_value);
	return return_value;
}

internal u32 wayland_wl_seat_get_keyboard(Wayland_Connection *wc, u32 wl_seat) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_seat);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_seat@%d.get_keyboard: id=%d"), wl_seat, return_value);
	return return_value;
}

internal u32 wayland_wl_seat_get_touch(Wayland_Connection *wc, u32 wl_seat) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_seat);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);

	wayland_log_infof(LIT("-> wl_seat@%d.get_touch: id=%d"), wl_seat, return_value);
	return return_value;
}

internal void wayland_wl_seat_release(Wayland_Connection *wc, u32 wl_seat) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_seat);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_seat@%d.release:"), wl_seat);
}

typedef enum {
	Wayland_Wl_Seat_Event_Capabilities = 0,
	Wayland_Wl_Seat_Event_Name = 1,
} Wayland_Wl_Seat_Event;

internal isize wayland_parse_event_wl_seat_capabilities(Wayland_Connection *conn, Wayland_Wl_Seat_Capability *capabilities) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	u32 capabilities_value;
	or_return(read_any(&r, &capabilities_value), -1);
	*capabilities = (Wayland_Wl_Seat_Capability)capabilities_value;
	wayland_log_infof(LIT("<- wl_seat@%d.capabilities: capabilities=%S"), _object_id, wayland_wl_seat_capability_string(*capabilities));
	return _msg_size;
}

internal isize wayland_parse_event_wl_seat_name(Wayland_Connection *conn, String *name, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	u32 name_len;
	or_return(read_any(&r, &name_len), -1);
	slice_init(name, name_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*name)), -1);
	name->len -= 1;
	if (name_len % 4) {
		byte name_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = name_pad_buf, .len = 4 - (name_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_seat@%d.name: name=%S"), _object_id, *name);
	return _msg_size;
}

internal void wayland_wl_pointer_set_cursor(Wayland_Connection *wc, u32 wl_pointer, u32 serial, u32 surface, i32 hotspot_x, i32 hotspot_y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_pointer);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &serial);
	write_any(w, &surface);
	write_any(w, &hotspot_x);
	write_any(w, &hotspot_y);

	wayland_log_infof(LIT("-> wl_pointer@%d.set_cursor: serial=%d surface=%d hotspot_x=%d hotspot_y=%d"), wl_pointer, serial, surface, hotspot_x, hotspot_y);
}

internal void wayland_wl_pointer_release(Wayland_Connection *wc, u32 wl_pointer) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_pointer);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_pointer@%d.release:"), wl_pointer);
}

typedef enum {
	Wayland_Wl_Pointer_Event_Enter = 0,
	Wayland_Wl_Pointer_Event_Leave = 1,
	Wayland_Wl_Pointer_Event_Motion = 2,
	Wayland_Wl_Pointer_Event_Button = 3,
	Wayland_Wl_Pointer_Event_Axis = 4,
	Wayland_Wl_Pointer_Event_Frame = 5,
	Wayland_Wl_Pointer_Event_Axis_Source = 6,
	Wayland_Wl_Pointer_Event_Axis_Stop = 7,
	Wayland_Wl_Pointer_Event_Axis_Discrete = 8,
	Wayland_Wl_Pointer_Event_Axis_Value120 = 9,
	Wayland_Wl_Pointer_Event_Axis_Relative_Direction = 10,
} Wayland_Wl_Pointer_Event;

internal isize wayland_parse_event_wl_pointer_enter(Wayland_Connection *conn, u32 *serial, u32 *surface, f32 *surface_x, f32 *surface_y) {
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
	or_return(read_any(&r, surface), -1);
	i32 surface_x_fixed;
	or_return(read_any(&r, &surface_x_fixed), -1);
	*surface_x = (f64)surface_x_fixed / 256.0;
	i32 surface_y_fixed;
	or_return(read_any(&r, &surface_y_fixed), -1);
	*surface_y = (f64)surface_y_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_pointer@%d.enter: serial=%d surface=%d surface_x=%f surface_y=%f"), _object_id, *serial, *surface, *surface_x, *surface_y);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_leave(Wayland_Connection *conn, u32 *serial, u32 *surface) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, surface), -1);
	wayland_log_infof(LIT("<- wl_pointer@%d.leave: serial=%d surface=%d"), _object_id, *serial, *surface);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_motion(Wayland_Connection *conn, u32 *time, f32 *surface_x, f32 *surface_y) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, time), -1);
	i32 surface_x_fixed;
	or_return(read_any(&r, &surface_x_fixed), -1);
	*surface_x = (f64)surface_x_fixed / 256.0;
	i32 surface_y_fixed;
	or_return(read_any(&r, &surface_y_fixed), -1);
	*surface_y = (f64)surface_y_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_pointer@%d.motion: time=%d surface_x=%f surface_y=%f"), _object_id, *time, *surface_x, *surface_y);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_button(Wayland_Connection *conn, u32 *serial, u32 *time, u32 *button, Wayland_Wl_Pointer_Button_State *state) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, time), -1);
	or_return(read_any(&r, button), -1);
	u32 state_value;
	or_return(read_any(&r, &state_value), -1);
	*state = (Wayland_Wl_Pointer_Button_State)state_value;
	wayland_log_infof(LIT("<- wl_pointer@%d.button: serial=%d time=%d button=%d state=%S"), _object_id, *serial, *time, *button, wayland_wl_pointer_button_state_string(*state));
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis(Wayland_Connection *conn, u32 *time, Wayland_Wl_Pointer_Axis *axis, f32 *value) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	or_return(read_any(&r, time), -1);
	u32 axis_value;
	or_return(read_any(&r, &axis_value), -1);
	*axis = (Wayland_Wl_Pointer_Axis)axis_value;
	i32 value_fixed;
	or_return(read_any(&r, &value_fixed), -1);
	*value = (f64)value_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_pointer@%d.axis: time=%d axis=%S value=%f"), _object_id, *time, wayland_wl_pointer_axis_string(*axis), *value);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_frame(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	wayland_log_infof(LIT("<- wl_pointer@%d.frame:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis_source(Wayland_Connection *conn, Wayland_Wl_Pointer_Axis_Source *axis_source) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 6);
	u32 axis_source_value;
	or_return(read_any(&r, &axis_source_value), -1);
	*axis_source = (Wayland_Wl_Pointer_Axis_Source)axis_source_value;
	wayland_log_infof(LIT("<- wl_pointer@%d.axis_source: axis_source=%S"), _object_id, wayland_wl_pointer_axis_source_string(*axis_source));
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis_stop(Wayland_Connection *conn, u32 *time, Wayland_Wl_Pointer_Axis *axis) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 7);
	or_return(read_any(&r, time), -1);
	u32 axis_value;
	or_return(read_any(&r, &axis_value), -1);
	*axis = (Wayland_Wl_Pointer_Axis)axis_value;
	wayland_log_infof(LIT("<- wl_pointer@%d.axis_stop: time=%d axis=%S"), _object_id, *time, wayland_wl_pointer_axis_string(*axis));
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis_discrete(Wayland_Connection *conn, Wayland_Wl_Pointer_Axis *axis, i32 *discrete) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 8);
	u32 axis_value;
	or_return(read_any(&r, &axis_value), -1);
	*axis = (Wayland_Wl_Pointer_Axis)axis_value;
	or_return(read_any(&r, discrete), -1);
	wayland_log_infof(LIT("<- wl_pointer@%d.axis_discrete: axis=%S discrete=%d"), _object_id, wayland_wl_pointer_axis_string(*axis), *discrete);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis_value120(Wayland_Connection *conn, Wayland_Wl_Pointer_Axis *axis, i32 *value120) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 9);
	u32 axis_value;
	or_return(read_any(&r, &axis_value), -1);
	*axis = (Wayland_Wl_Pointer_Axis)axis_value;
	or_return(read_any(&r, value120), -1);
	wayland_log_infof(LIT("<- wl_pointer@%d.axis_value120: axis=%S value120=%d"), _object_id, wayland_wl_pointer_axis_string(*axis), *value120);
	return _msg_size;
}

internal isize wayland_parse_event_wl_pointer_axis_relative_direction(Wayland_Connection *conn, Wayland_Wl_Pointer_Axis *axis, Wayland_Wl_Pointer_Axis_Relative_Direction *direction) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 10);
	u32 axis_value;
	or_return(read_any(&r, &axis_value), -1);
	*axis = (Wayland_Wl_Pointer_Axis)axis_value;
	u32 direction_value;
	or_return(read_any(&r, &direction_value), -1);
	*direction = (Wayland_Wl_Pointer_Axis_Relative_Direction)direction_value;
	wayland_log_infof(LIT("<- wl_pointer@%d.axis_relative_direction: axis=%S direction=%S"), _object_id, wayland_wl_pointer_axis_string(*axis), wayland_wl_pointer_axis_relative_direction_string(*direction));
	return _msg_size;
}

internal void wayland_wl_keyboard_release(Wayland_Connection *wc, u32 wl_keyboard) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_keyboard);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_keyboard@%d.release:"), wl_keyboard);
}

typedef enum {
	Wayland_Wl_Keyboard_Event_Keymap = 0,
	Wayland_Wl_Keyboard_Event_Enter = 1,
	Wayland_Wl_Keyboard_Event_Leave = 2,
	Wayland_Wl_Keyboard_Event_Key = 3,
	Wayland_Wl_Keyboard_Event_Modifiers = 4,
	Wayland_Wl_Keyboard_Event_Repeat_Info = 5,
} Wayland_Wl_Keyboard_Event;

internal isize wayland_parse_event_wl_keyboard_keymap(Wayland_Connection *conn, Wayland_Wl_Keyboard_Keymap_Format *format, Fd *fd, u32 *size) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 0);
	u32 format_value;
	or_return(read_any(&r, &format_value), -1);
	*format = (Wayland_Wl_Keyboard_Keymap_Format)format_value;
	*fd = conn->fds_in.data[0];
	vector_remove_ordered(&conn->fds_in, 0);
	or_return(read_any(&r, size), -1);
	wayland_log_infof(LIT("<- wl_keyboard@%d.keymap: format=%S fd=%d size=%d"), _object_id, wayland_wl_keyboard_keymap_format_string(*format), *fd, *size);
	return _msg_size;
}

internal isize wayland_parse_event_wl_keyboard_enter(Wayland_Connection *conn, u32 *serial, u32 *surface, Byte_Slice *keys, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, surface), -1);
	u32 keys_len;
	or_return(read_any(&r, &keys_len), -1);
	slice_init(keys, keys_len, allocator);
	or_return(read_bytes(&r, *keys), -1);
	if (keys_len % 4) {
		byte keys_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = keys_pad_buf, .len = 4 - (keys_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_keyboard@%d.enter: serial=%d surface=%d"), _object_id, *serial, *surface);
	return _msg_size;
}

internal isize wayland_parse_event_wl_keyboard_leave(Wayland_Connection *conn, u32 *serial, u32 *surface) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, surface), -1);
	wayland_log_infof(LIT("<- wl_keyboard@%d.leave: serial=%d surface=%d"), _object_id, *serial, *surface);
	return _msg_size;
}

internal isize wayland_parse_event_wl_keyboard_key(Wayland_Connection *conn, u32 *serial, u32 *time, u32 *key, Wayland_Wl_Keyboard_Key_State *state) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, time), -1);
	or_return(read_any(&r, key), -1);
	u32 state_value;
	or_return(read_any(&r, &state_value), -1);
	*state = (Wayland_Wl_Keyboard_Key_State)state_value;
	wayland_log_infof(LIT("<- wl_keyboard@%d.key: serial=%d time=%d key=%d state=%S"), _object_id, *serial, *time, *key, wayland_wl_keyboard_key_state_string(*state));
	return _msg_size;
}

internal isize wayland_parse_event_wl_keyboard_modifiers(Wayland_Connection *conn, u32 *serial, u32 *mods_depressed, u32 *mods_latched, u32 *mods_locked, u32 *group) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, mods_depressed), -1);
	or_return(read_any(&r, mods_latched), -1);
	or_return(read_any(&r, mods_locked), -1);
	or_return(read_any(&r, group), -1);
	wayland_log_infof(LIT("<- wl_keyboard@%d.modifiers: serial=%d mods_depressed=%d mods_latched=%d mods_locked=%d group=%d"), _object_id, *serial, *mods_depressed, *mods_latched, *mods_locked, *group);
	return _msg_size;
}

internal isize wayland_parse_event_wl_keyboard_repeat_info(Wayland_Connection *conn, i32 *rate, i32 *delay) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	or_return(read_any(&r, rate), -1);
	or_return(read_any(&r, delay), -1);
	wayland_log_infof(LIT("<- wl_keyboard@%d.repeat_info: rate=%d delay=%d"), _object_id, *rate, *delay);
	return _msg_size;
}

internal void wayland_wl_touch_release(Wayland_Connection *wc, u32 wl_touch) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_touch);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_touch@%d.release:"), wl_touch);
}

typedef enum {
	Wayland_Wl_Touch_Event_Down = 0,
	Wayland_Wl_Touch_Event_Up = 1,
	Wayland_Wl_Touch_Event_Motion = 2,
	Wayland_Wl_Touch_Event_Frame = 3,
	Wayland_Wl_Touch_Event_Cancel = 4,
	Wayland_Wl_Touch_Event_Shape = 5,
	Wayland_Wl_Touch_Event_Orientation = 6,
} Wayland_Wl_Touch_Event;

internal isize wayland_parse_event_wl_touch_down(Wayland_Connection *conn, u32 *serial, u32 *time, u32 *surface, i32 *id, f32 *x, f32 *y) {
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
	or_return(read_any(&r, time), -1);
	or_return(read_any(&r, surface), -1);
	or_return(read_any(&r, id), -1);
	i32 x_fixed;
	or_return(read_any(&r, &x_fixed), -1);
	*x = (f64)x_fixed / 256.0;
	i32 y_fixed;
	or_return(read_any(&r, &y_fixed), -1);
	*y = (f64)y_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_touch@%d.down: serial=%d time=%d surface=%d id=%d x=%f y=%f"), _object_id, *serial, *time, *surface, *id, *x, *y);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_up(Wayland_Connection *conn, u32 *serial, u32 *time, i32 *id) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	or_return(read_any(&r, serial), -1);
	or_return(read_any(&r, time), -1);
	or_return(read_any(&r, id), -1);
	wayland_log_infof(LIT("<- wl_touch@%d.up: serial=%d time=%d id=%d"), _object_id, *serial, *time, *id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_motion(Wayland_Connection *conn, u32 *time, i32 *id, f32 *x, f32 *y) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	or_return(read_any(&r, time), -1);
	or_return(read_any(&r, id), -1);
	i32 x_fixed;
	or_return(read_any(&r, &x_fixed), -1);
	*x = (f64)x_fixed / 256.0;
	i32 y_fixed;
	or_return(read_any(&r, &y_fixed), -1);
	*y = (f64)y_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_touch@%d.motion: time=%d id=%d x=%f y=%f"), _object_id, *time, *id, *x, *y);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_frame(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	wayland_log_infof(LIT("<- wl_touch@%d.frame:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_cancel(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	wayland_log_infof(LIT("<- wl_touch@%d.cancel:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_shape(Wayland_Connection *conn, i32 *id, f32 *major, f32 *minor) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	or_return(read_any(&r, id), -1);
	i32 major_fixed;
	or_return(read_any(&r, &major_fixed), -1);
	*major = (f64)major_fixed / 256.0;
	i32 minor_fixed;
	or_return(read_any(&r, &minor_fixed), -1);
	*minor = (f64)minor_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_touch@%d.shape: id=%d major=%f minor=%f"), _object_id, *id, *major, *minor);
	return _msg_size;
}

internal isize wayland_parse_event_wl_touch_orientation(Wayland_Connection *conn, i32 *id, f32 *orientation) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 6);
	or_return(read_any(&r, id), -1);
	i32 orientation_fixed;
	or_return(read_any(&r, &orientation_fixed), -1);
	*orientation = (f64)orientation_fixed / 256.0;
	wayland_log_infof(LIT("<- wl_touch@%d.orientation: id=%d orientation=%f"), _object_id, *id, *orientation);
	return _msg_size;
}

internal void wayland_wl_output_release(Wayland_Connection *wc, u32 wl_output) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_output);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_output@%d.release:"), wl_output);
}

typedef enum {
	Wayland_Wl_Output_Event_Geometry = 0,
	Wayland_Wl_Output_Event_Mode = 1,
	Wayland_Wl_Output_Event_Done = 2,
	Wayland_Wl_Output_Event_Scale = 3,
	Wayland_Wl_Output_Event_Name = 4,
	Wayland_Wl_Output_Event_Description = 5,
} Wayland_Wl_Output_Event;

internal isize wayland_parse_event_wl_output_geometry(Wayland_Connection *conn, i32 *x, i32 *y, i32 *physical_width, i32 *physical_height, Wayland_Wl_Output_Subpixel *subpixel, String *make, String *model, Wayland_Wl_Output_Transform *transform, Allocator allocator) {
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
	or_return(read_any(&r, physical_width), -1);
	or_return(read_any(&r, physical_height), -1);
	u32 subpixel_value;
	or_return(read_any(&r, &subpixel_value), -1);
	*subpixel = (Wayland_Wl_Output_Subpixel)subpixel_value;
	u32 make_len;
	or_return(read_any(&r, &make_len), -1);
	slice_init(make, make_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*make)), -1);
	make->len -= 1;
	if (make_len % 4) {
		byte make_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = make_pad_buf, .len = 4 - (make_len % 4) }), -1);
	}
	u32 model_len;
	or_return(read_any(&r, &model_len), -1);
	slice_init(model, model_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*model)), -1);
	model->len -= 1;
	if (model_len % 4) {
		byte model_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = model_pad_buf, .len = 4 - (model_len % 4) }), -1);
	}
	u32 transform_value;
	or_return(read_any(&r, &transform_value), -1);
	*transform = (Wayland_Wl_Output_Transform)transform_value;
	wayland_log_infof(LIT("<- wl_output@%d.geometry: x=%d y=%d physical_width=%d physical_height=%d subpixel=%S make=%S model=%S transform=%S"), _object_id, *x, *y, *physical_width, *physical_height, wayland_wl_output_subpixel_string(*subpixel), *make, *model, wayland_wl_output_transform_string(*transform));
	return _msg_size;
}

internal isize wayland_parse_event_wl_output_mode(Wayland_Connection *conn, Wayland_Wl_Output_Mode *flags, i32 *width, i32 *height, i32 *refresh) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 1);
	u32 flags_value;
	or_return(read_any(&r, &flags_value), -1);
	*flags = (Wayland_Wl_Output_Mode)flags_value;
	or_return(read_any(&r, width), -1);
	or_return(read_any(&r, height), -1);
	or_return(read_any(&r, refresh), -1);
	wayland_log_infof(LIT("<- wl_output@%d.mode: flags=%S width=%d height=%d refresh=%d"), _object_id, wayland_wl_output_mode_string(*flags), *width, *height, *refresh);
	return _msg_size;
}

internal isize wayland_parse_event_wl_output_done(Wayland_Connection *conn) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 2);
	wayland_log_infof(LIT("<- wl_output@%d.done:"), _object_id);
	return _msg_size;
}

internal isize wayland_parse_event_wl_output_scale(Wayland_Connection *conn, i32 *factor) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 3);
	or_return(read_any(&r, factor), -1);
	wayland_log_infof(LIT("<- wl_output@%d.scale: factor=%d"), _object_id, *factor);
	return _msg_size;
}

internal isize wayland_parse_event_wl_output_name(Wayland_Connection *conn, String *name, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 4);
	u32 name_len;
	or_return(read_any(&r, &name_len), -1);
	slice_init(name, name_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*name)), -1);
	name->len -= 1;
	if (name_len % 4) {
		byte name_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = name_pad_buf, .len = 4 - (name_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_output@%d.name: name=%S"), _object_id, *name);
	return _msg_size;
}

internal isize wayland_parse_event_wl_output_description(Wayland_Connection *conn, String *description, Allocator allocator) {
	Byte_Slice _data = slice_range(conn->buffer, conn->start, conn->end);
	Reader     r     = buffer_reader(&_data);
	u32 _object_id;
	u16 _opcode, _msg_size;
	or_return(read_any(&r, &_object_id), -1);
	or_return(read_any(&r, &_opcode),    -1);
	or_return(read_any(&r, &_msg_size),  -1);
	conn->start += _msg_size;
	assert(_opcode == 5);
	u32 description_len;
	or_return(read_any(&r, &description_len), -1);
	slice_init(description, description_len, allocator);
	or_return(read_bytes(&r, string_to_bytes(*description)), -1);
	description->len -= 1;
	if (description_len % 4) {
		byte description_pad_buf[4];
		or_return(read_bytes(&r, (Byte_Slice) {.data = description_pad_buf, .len = 4 - (description_len % 4) }), -1);
	}
	wayland_log_infof(LIT("<- wl_output@%d.description: description=%S"), _object_id, *description);
	return _msg_size;
}

internal void wayland_wl_region_destroy(Wayland_Connection *wc, u32 wl_region) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_region);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_region@%d.destroy:"), wl_region);
}

internal void wayland_wl_region_add(Wayland_Connection *wc, u32 wl_region, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_region);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> wl_region@%d.add: x=%d y=%d width=%d height=%d"), wl_region, x, y, width, height);
}

internal void wayland_wl_region_subtract(Wayland_Connection *wc, u32 wl_region, i32 x, i32 y, i32 width, i32 height) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_region);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 24;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);
	write_any(w, &width);
	write_any(w, &height);

	wayland_log_infof(LIT("-> wl_region@%d.subtract: x=%d y=%d width=%d height=%d"), wl_region, x, y, width, height);
}

internal void wayland_wl_subcompositor_destroy(Wayland_Connection *wc, u32 wl_subcompositor) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subcompositor);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_subcompositor@%d.destroy:"), wl_subcompositor);
}

internal u32 wayland_wl_subcompositor_get_subsurface(Wayland_Connection *wc, u32 wl_subcompositor, u32 surface, u32 parent) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subcompositor);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 20;

	write_any(w, &_msg_size);
	wc->current_id  += 1;
	u32 return_value = wc->current_id;
	write_any(w, &return_value);
	write_any(w, &surface);
	write_any(w, &parent);

	wayland_log_infof(LIT("-> wl_subcompositor@%d.get_subsurface: id=%d surface=%d parent=%d"), wl_subcompositor, return_value, surface, parent);
	return return_value;
}

internal void wayland_wl_subsurface_destroy(Wayland_Connection *wc, u32 wl_subsurface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_subsurface@%d.destroy:"), wl_subsurface);
}

internal void wayland_wl_subsurface_set_position(Wayland_Connection *wc, u32 wl_subsurface, i32 x, i32 y) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 16;

	write_any(w, &_msg_size);
	write_any(w, &x);
	write_any(w, &y);

	wayland_log_infof(LIT("-> wl_subsurface@%d.set_position: x=%d y=%d"), wl_subsurface, x, y);
}

internal void wayland_wl_subsurface_place_above(Wayland_Connection *wc, u32 wl_subsurface, u32 sibling) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 2;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &sibling);

	wayland_log_infof(LIT("-> wl_subsurface@%d.place_above: sibling=%d"), wl_subsurface, sibling);
}

internal void wayland_wl_subsurface_place_below(Wayland_Connection *wc, u32 wl_subsurface, u32 sibling) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 3;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &sibling);

	wayland_log_infof(LIT("-> wl_subsurface@%d.place_below: sibling=%d"), wl_subsurface, sibling);
}

internal void wayland_wl_subsurface_set_sync(Wayland_Connection *wc, u32 wl_subsurface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 4;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_subsurface@%d.set_sync:"), wl_subsurface);
}

internal void wayland_wl_subsurface_set_desync(Wayland_Connection *wc, u32 wl_subsurface) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_subsurface);
	u16 _opcode = 5;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_subsurface@%d.set_desync:"), wl_subsurface);
}

internal void wayland_wl_fixes_destroy(Wayland_Connection *wc, u32 wl_fixes) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_fixes);
	u16 _opcode = 0;
	write_any(w, &_opcode);
	u16 _msg_size = 8;

	write_any(w, &_msg_size);

	wayland_log_infof(LIT("-> wl_fixes@%d.destroy:"), wl_fixes);
}

internal void wayland_wl_fixes_destroy_registry(Wayland_Connection *wc, u32 wl_fixes, u32 registry) {
	Writer _w = writer_from_builder(&wc->builder);
	Writer *w = &_w;
	write_any(w, &wl_fixes);
	u16 _opcode = 1;
	write_any(w, &_opcode);
	u16 _msg_size = 12;

	write_any(w, &_msg_size);
	write_any(w, &registry);

	wayland_log_infof(LIT("-> wl_fixes@%d.destroy_registry: registry=%d"), wl_fixes, registry);
}

