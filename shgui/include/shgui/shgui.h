#ifndef SH_GUI_H
#define SH_GUI_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <shgui/shguiShaders.h>

#include <shvulkan/shVulkan.h>

#include <stdio.h>
#include <stdint.h>



#ifndef _WIN32
#define SH_GUI_CALL
#else
#define SH_GUI_CALL __stdcall
#endif//_WIN32



#define shGuiError(\
	condition,\
	error_msg,\
	failure_expression\
)\
	if ((int)(condition)) {\
		printf("shgui error: %s\n", (const char*)(error_msg));\
		failure_expression;\
	}

#define shGuiVkResultError(\
	result,\
	error_msg,\
	failure_expression\
)\
	if ((VkResult)(result) != VK_SUCCESS) {\
		printf("shgui vulkan error: %s, %s\n", error_msg, shTranslateVkResult((VkResult)(result)));\
		failure_expression;\
	}



#define SH_GUI_STRUCTURE_ZERO { 0 }



typedef struct ShGuiCore {
	VkDevice         device;
	VkPhysicalDevice physical_device;
	uint32_t         graphics_queue_family_index;
	VkQueue          graphics_queue;
	VkSurfaceKHR     surface;
	VkRenderPass     render_pass;
	uint32_t         sample_count;
	uint32_t         framebuffer_count;
} ShGuiCore;



#define SH_GUI_KEY_LAST                   348
#define SH_GUI_MOUSE_LAST                 1
				


typedef int8_t  ShGuiKeyEvents            [SH_GUI_KEY_LAST + 1];
#define SH_GUI_NO_KEY                     UINT8_MAX


typedef int8_t  ShGuiMouseEvents          [SH_GUI_MOUSE_LAST + 1];

typedef enum ShGuiMouseButton {
	SH_GUI_LEFT_MOUSE_BUTTON  = 0,
	SH_GUI_RIGHT_MOUSE_BUTTON = 1,
	SH_GUI_NO_MOUSE_BUTTON
} ShGuiMouseButton;

	          
#define SH_GUI_WINDOW_CURSOR_RESIZE_SPEED 1000.0f

typedef enum ShGuiCursor {
	SH_GUI_CURSOR_NORMAL                       = 0,
	SH_GUI_CURSOR_HORIZONTAL_RESIZE            = 1,
	SH_GUI_CURSOR_VERTICAL_RESIZE              = 2
} ShGuiCursor;


#define SH_GUI_MAX_TOLERATED_DELTA_TIME 5.0f

typedef enum ShGuiInputFlags {
	SH_GUI_CLICK_ONCE   = 1 << 0,
	SH_GUI_CLICK_REPEAT = 1 << 1,
	SH_GUI_PRESS_ONCE   = 1 << 2,
	SH_GUI_PRESS_REPEAT = 1 << 3
} ShGuiInputFlags;

typedef struct ShGuiLastInputs {
	float             last_cursor_pos_x;
	float             last_cursor_pos_y;
	ShGuiKeyEvents    last_key_events;
	ShGuiMouseEvents  last_mouse_events;
} ShGuiLastInputs;

typedef struct ShGuiInputs {
	uint32_t*       p_window_width;
	uint32_t*       p_window_height;
	float*          p_cursor_pos_x;
	float*          p_cursor_pos_y;
	int8_t*         p_key_events;
	int8_t*         p_mouse_events;
	ShGuiCursor     active_cursor;
	double*         p_delta_time;
	ShGuiLastInputs last;
} ShGuiInputs;



typedef struct shguivec2 { float x; float y; }                   shguivec2;
typedef struct shguivec3 { float x; float y; float z; }          shguivec3;
typedef struct shguivec4 { float x; float y; float z; float w; } shguivec4;

static  const shguivec2 _SH_GUI_VEC2_ZERO = SH_GUI_STRUCTURE_ZERO;
static  const shguivec3 _SH_GUI_VEC3_ZERO = SH_GUI_STRUCTURE_ZERO;
static  const shguivec4 _SH_GUI_VEC4_ZERO = SH_GUI_STRUCTURE_ZERO;

#define SH_GUI_VEC2_ZERO _SH_GUI_VEC2_ZERO
#define SH_GUI_VEC3_ZERO _SH_GUI_VEC3_ZERO
#define SH_GUI_VEC4_ZERO _SH_GUI_VEC4_ZERO

#define SH_GUI_VEC2_ZERO_COMPONENTS _SH_GUI_VEC2_ZERO.x, _SH_GUI_VEC2_ZERO.y
#define SH_GUI_VEC3_ZERO_COMPONENTS _SH_GUI_VEC3_ZERO.x, _SH_GUI_VEC3_ZERO.y, _SH_GUI_VEC3_ZERO.z
#define SH_GUI_VEC4_ZERO_COMPONENTS _SH_GUI_VEC4_ZERO.x, _SH_GUI_VEC4_ZERO.y, _SH_GUI_VEC4_ZERO.z, _SH_GUI_VEC4_ZERO.w

#define SH_GUI_VEC2_COMPARE(first, second) (((first).x == (second).x) && ((first).y == (second).y))
#define SH_GUI_VEC3_COMPARE(first, second) (((first).x == (second).x) && ((first).y == (second).y) && ((first).z == (second).z))
#define SH_GUI_VEC4_COMPARE(first, second) (((first).x == (second).x) && ((first).y == (second).y) && ((first).z == (second).z) && ((first).w == (second).w))

#define SH_GUI_VEC2_IS_ZERO(src) SH_GUI_VEC2_COMPARE(src, SH_GUI_VEC2_ZERO) 
#define SH_GUI_VEC3_IS_ZERO(src) SH_GUI_VEC3_COMPARE(src, SH_GUI_VEC3_ZERO) 
#define SH_GUI_VEC4_IS_ZERO(src) SH_GUI_VEC4_COMPARE(src, SH_GUI_VEC4_ZERO) 

#define SH_GUI_VEC2_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y))
#define SH_GUI_VEC3_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y) && ((dst).z = (src).z))
#define SH_GUI_VEC4_COPY(dst, src) (((dst).x = (src).x) && ((dst).y = (src).y) && ((dst).z = (src).z) && ((dst).w = (src).w))


#define SH_GUI_COLOR_BLACK   (shguivec4){ 0.0f }
#define SH_GUI_COLOR_WHITE   (shguivec4){ 1.0f, 1.0f, 1.0f, 1.0f }
#define SH_GUI_COLOR_RED     (shguivec4){ 1.0f, 0.0f, 0.0f, 1.0f }
#define SH_GUI_COLOR_GREEN   (shguivec4){ 0.0f, 1.0f, 0.0f, 1.0f }
#define SH_GUI_COLOR_BLUE    (shguivec4){ 0.0f, 0.0f, 1.0f, 1.0f }
							 
#define SH_GUI_COLOR_YELLOW  (shguivec4){1.0f, 1.0f, 0.0f }
#define SH_GUI_COLOR_CYAN    (shguivec4){0.0f, 1.0f, 1.0f }
#define SH_GUI_COLOR_MAGENTA (shguivec4){1.0f, 0.0f, 1.0f }

#define SH_GUI_COLOR_GREY    (shguivec4){ 0.5f, 0.5f, 0.5f }


typedef enum ShGuiDefaultValuesFlags {
	SH_GUI_NO_THEME = 0,
	SH_GUI_THEME_DARK,
	SH_GUI_THEME_EXTRA_DARK,
	SH_GUI_THEME_LIGHT,
	SH_GUI_DEFAULT_VALUES_MAX_ENUM 
} ShGuiDefaultValuesFlags;

typedef struct ShGuiDefaultRegionValuesRaw {
	shguivec4 color;
	shguivec4 edge_color;
} ShGuiDefaultRegionValuesRaw;

typedef struct ShGuiDefaultCharValuesRaw {
	shguivec4 color;
} ShGuiDefaultCharValuesRaw;

typedef struct ShGuiDefaultValues {
	ShGuiDefaultRegionValuesRaw default_region_raw_values;
	ShGuiDefaultCharValuesRaw   default_char_raw_values;
	ShGuiDefaultValuesFlags     flags;
} ShGuiDefaultValues;


#define SH_GUI_WINDOW_BAR_HEIGHT         20.0f
#define SH_GUI_WINDOW_TEXT_SIZE          15.0f
#define SH_GUI_WINDOW_TEXT_BORDER_OFFSET 5.0f
#define SH_GUI_WINDOW_USED_HEIGHT(\
	window_pos_y,\
	window_scale_y,\
	used_height,\
	ui_scale\
)\
	((float)(window_pos_y)) + ((float)(window_scale_y)) / 2.0f - SH_GUI_WINDOW_BAR_HEIGHT - ((float)(used_height)) * (ui_scale)

#define SH_GUI_WINDOW_ITEMS_OFFSET                         15.0f
#define SH_GUI_CURSOR_EDGE_CHECK_SIZE                      6.0f
#define SH_GUI_WINDOW_SEPARATOR_EXTENT(window_abs_scale_x) ((window_abs_scale_x) / 1.2f)
#define SH_GUI_WINDOW_SLIDER_EXTENT(window_abs_scale_x)    ((window_abs_scale_x) / 1.4f)

#define SH_GUI_MIN_REGION_WIDTH  50.0f
#define SH_GUI_MIN_REGION_HEIGHT 50.0f

typedef enum ShGuiRegionRawWriteFlags {
	SH_GUI_REGION_RAW_X_POSITION = 1 << 0,
	SH_GUI_REGION_RAW_Y_POSITION = 1 << 1,
	SH_GUI_REGION_RAW_POSITION   = SH_GUI_REGION_RAW_X_POSITION | SH_GUI_REGION_RAW_Y_POSITION,
	SH_GUI_REGION_RAW_X_SCALE    = 1 << 2,
	SH_GUI_REGION_RAW_Y_SCALE    = 1 << 3,
	SH_GUI_REGION_RAW_SCALE      = SH_GUI_REGION_RAW_X_SCALE | SH_GUI_REGION_RAW_Y_SCALE,
	SH_GUI_REGION_RAW_COLOR      = 1 << 4,
	SH_GUI_REGION_RAW_EDGE_COLOR = 1 << 5,
	SH_GUI_REGION_RAW_PRIORITY   = 1 << 6,
	SH_GUI_REGION_RAW_MAX_ENUM
} ShGuiRegionRawWriteFlags;

typedef struct ShGuiRegionRaw {
	shguivec2 position;
	shguivec2 scale;
	shguivec4 color;
	shguivec3 edge_color;
	float     priority;
} ShGuiRegionRaw;

static  const ShGuiRegionRaw _SH_GUI_NO_REGION = SH_GUI_STRUCTURE_ZERO;
#define SH_GUI_NO_REGION     _SH_GUI_NO_REGION

typedef struct ShGuiRegionInfos {
	float                     region_scale_factor;
	//regions = bars + menus + buttons + windows    
	uint32_t                  max_region_count;
	uint32_t                  max_regions_raw_size;
	ShGuiRegionRaw*           p_regions_raw;
	//menus
	uint32_t                  menu_count;
	uint32_t*                 p_menus_region_indices;
	//windows       
	uint32_t                  window_count;
	uint32_t*                 p_windows_region_indices;
	float*                    p_windows_used_height;
	//region conditions
	uint32_t                  region_count;
	ShGuiRegionRawWriteFlags* p_regions_raw_write_flags;

	uint8_t*                  p_regions_clicked;          
	uint8_t*                  p_cursor_on_regions;        
	uint8_t*                  p_moving_regions;
	uint8_t*                  p_right_resizing_regions;
	uint8_t*                  p_left_resizing_regions;
	uint8_t*                  p_top_resizing_regions;
	uint8_t*                  p_bottom_resizing_regions;
	float*                    p_windows_slider_buttons_offsets;
	uint8_t                   cursor_on_regions;
} ShGuiRegionInfos;



#define SH_GUI_MAX_CHAR_VERTEX_COUNT (7 * 7)
#define SH_GUI_MAX_CHAR_VERTEX_SIZE  (7 * 7 * 3 * 4)

typedef float ShGuiCharVertexRaw[SH_GUI_MAX_CHAR_VERTEX_COUNT * 3];
static  const ShGuiCharVertexRaw _SH_GUI_NO_CHAR = { 0xbf800000 };
#define SH_GUI_NO_CHAR           _SH_GUI_NO_CHAR

#define SH_GUI_LOAD_CHAR_VERTICES(dst, src)\
	memcpy(dst, src, SH_GUI_MAX_CHAR_VERTEX_SIZE)

typedef enum ShGuiCharRawWriteFlags {
	SH_GUI_CHAR_RAW_X_POSITION = 1 << 1,
	SH_GUI_CHAR_RAW_Y_POSITION = 1 << 2,
	SH_GUI_CHAR_RAW_POSITION   = SH_GUI_CHAR_RAW_X_POSITION | SH_GUI_CHAR_RAW_Y_POSITION,
	SH_GUI_CHAR_RAW_SCALE      = 1 << 3,
	SH_GUI_CHAR_RAW_PRIORITY   = 1 << 4,
	SH_GUI_CHAR_RAW_COLOR      = 1 << 5,
	SH_GUI_CHAR_RAW_MAX_ENUM
} ShGuiCharRawWriteFlags;

typedef struct ShGuiCharRaw {
	shguivec2 position;
	float     scale;
	float     priority;
	shguivec4 color;
} ShGuiCharRaw;

typedef struct ShGuiCharInfos {
	float                   char_scale_factor;
	uint32_t                max_char_count;
	uint32_t                max_chars_raw_size;
	uint32_t                max_chars_vertex_raw_size;
	uint32_t                char_count;
	ShGuiCharRaw*           p_chars_raw;
	ShGuiCharVertexRaw*     p_chars_vertex_raw;
	ShGuiCharRawWriteFlags* p_chars_raw_write_flags;
} ShGuiCharInfos;


typedef struct ShGuiItem {
	uint32_t  region_count;
	uint32_t* p_region_indices;
	uint32_t  char_count;
	uint32_t* p_char_indices;
} ShGuiItem;


#define SH_GUI_POPUP_VAR(id)    uint8_t sh_gui_popup_var_ ## id = 0;
#define SH_GUI_OPEN_POPUP(id)   sh_gui_popup_var_ ## id = 1; 
#define SH_GUI_CLOSE_POPUP(id)  sh_gui_popup_var_ ## id = 0; 
#define SH_GUI_SWITCH_POPUP(id) sh_gui_popup_var_ ## id = !sh_gui_popup_var_ ## id;
#define SH_GUI_CHECK_POPUP(id)  if (sh_gui_popup_var_ ## id) { 
#define SH_GUI_END_POPUP()      }

#define SH_GUI_REGION_VERT_SPV _SH_GUI_REGION_VERT_SPV
#define SH_GUI_REGION_FRAG_SPV _SH_GUI_REGION_FRAG_SPV
#define SH_GUI_CHAR_VERT_SPV   _SH_GUI_CHAR_VERT_SPV
#define SH_GUI_CHAR_FRAG_SPV   _SH_GUI_CHAR_FRAG_SPV

//#define SH_GUI_DEFAULT_REGION_VALUES_STAGING_SIZE          sizeof(ShGuiDefaultRegionValuesRaw)
//#define SH_GUI_DEFAULT_CHAR_VALUES_STAGING_SIZE            sizeof(ShGuiDefaultCharValuesRaw)
#define SH_GUI_REGIONS_RAW_STAGING_SIZE(max_region_count)    ((size_t)(max_region_count) * sizeof(ShGuiRegionRaw))
#define SH_GUI_CHARS_RAW_STAGING_SIZE(max_char_count)        ((size_t)(max_char_count)   * sizeof(ShGuiCharRaw))
#define SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE(max_char_count) ((size_t)(max_char_count)   * sizeof(ShGuiCharVertexRaw))

//#define SH_GUI_DEFAULT_REGION_VALUES_STAGING_OFFSET                              (0)
//#define SH_GUI_DEFAULT_CHAR_VALUES_STAGING_OFFSET                                (SH_GUI_DEFAULT_REGION_VALUES_STAGING_SIZE)
#define SH_GUI_REGIONS_RAW_STAGING_OFFSET                                        (0)
#define SH_GUI_CHARS_RAW_STAGING_OFFSET(max_region_count)                        (SH_GUI_REGIONS_RAW_STAGING_OFFSET + SH_GUI_REGIONS_RAW_STAGING_SIZE(max_region_count)) 
#define SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET(max_region_count, max_char_count) (SH_GUI_CHARS_RAW_STAGING_OFFSET(max_region_count) + SH_GUI_CHARS_RAW_STAGING_SIZE(max_char_count))

#define SH_GUI_STAGING_BUFFER_SIZE(max_region_count, max_char_count)             (SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET(max_region_count, max_char_count) + SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE(max_char_count))

//#define SH_GUI_DEFAULT_REGION_VALUES_DST_SIZE   SH_GUI_DEFAULT_REGION_VALUES_STAGING_SIZE
//#define SH_GUI_DEFAULT_CHAR_VALUES_DST_SIZE     SH_GUI_DEFAULT_CHAR_VALUES_STAGING_SIZE
#define SH_GUI_REGIONS_RAW_DST_SIZE             SH_GUI_REGIONS_RAW_STAGING_SIZE
#define SH_GUI_CHARS_RAW_DST_SIZE               SH_GUI_CHARS_RAW_STAGING_SIZE
#define SH_GUI_CHARS_VERTEX_RAW_DST_SIZE        SH_GUI_CHARS_VERTEX_RAW_STAGING_SIZE

//#define SH_GUI_DEFAULT_REGION_VALUES_DST_OFFSET SH_GUI_DEFAULT_REGION_VALUES_STAGING_OFFSET
//#define SH_GUI_DEFAULT_CHAR_VALUES_DST_OFFSET   SH_GUI_DEFAULT_CHAR_VALUES_STAGING_OFFSET 
#define SH_GUI_REGIONS_RAW_DST_OFFSET           SH_GUI_REGIONS_RAW_STAGING_OFFSET
#define SH_GUI_CHARS_RAW_DST_OFFSET             SH_GUI_CHARS_RAW_STAGING_OFFSET
#define SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET      SH_GUI_CHARS_VERTEX_RAW_STAGING_OFFSET

#define SH_GUI_DST_BUFFER_SIZE                  SH_GUI_STAGING_BUFFER_SIZE


typedef struct ShGui {
	ShGuiCore          core;
	ShGuiInputs        inputs;
	VkBuffer           staging_buffer;
	VkDeviceMemory     staging_memory;
	VkBuffer           dst_buffer;
	VkDeviceMemory     dst_memory;
	ShGuiDefaultValues default_values;
	ShGuiRegionInfos   region_infos;
	ShGuiCharInfos     char_infos;
	ShVkPipelinePool   pipeline_pool;
	ShVkPipeline       region_pipeline;
	ShVkPipeline       char_pipeline;
	uint32_t           item_count;
	uint32_t*          p_gui_items;
} ShGui;

#define shAllocateGui() ((ShGui*)calloc(1, sizeof(ShGui)))

extern uint8_t shGuiInit(
	ShGui*    p_gui,
	ShGuiCore core
);

extern uint8_t shGuiSetSurface(
	ShGui*       p_gui,
	VkSurfaceKHR surface
);

extern uint8_t shGuiSetRenderpass(
	ShGui*       p_gui,
	VkRenderPass renderpass
);

extern uint32_t SH_GUI_CALL shGuiGetAvailableHeap(
	ShGui*   p_gui, 
	uint32_t structure_count, 
	uint32_t structure_size
);

extern uint8_t SH_GUI_CALL shGuiAllocateMemory(
	ShGui*   p_gui,
	uint32_t max_region_count,
	uint32_t max_char_count
);

extern char* SH_GUI_CALL shGuiReadBinary(
	const char* path, 
	uint32_t*   p_code_size
);

extern uint8_t SH_GUI_CALL shGuiAllocatePipelineResources(
	ShGui*   p_gui,
	uint32_t framebuffer_count
);

extern uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(
	ShGui*      p_gui, 
	const char* vertex_shader_path, 
	const char* fragment_shader_path
);

extern uint8_t SH_GUI_CALL shGuiBuildCharPipeline(
	ShGui*      p_gui,
	const char* vertex_shader_path,
	const char* fragment_shader_path
);

extern uint8_t SH_GUI_CALL shGuiDestroyPipelineResources(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiDestroyPipelineResources(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiDestroyPipelines(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiWriteMemory(
	ShGui*          p_gui,
	VkCommandBuffer transfer_cmd_buffer,
	uint8_t         begin_cmd_buffer
);

extern uint8_t SH_GUI_CALL shGuiResizeInterface(
	ShGui*   p_gui,
	uint32_t last_width,
	uint32_t last_height,
	uint32_t current_width,
	uint32_t current_height
);

extern uint8_t SH_GUI_CALL shGuiReleaseMemory(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiRender(
	ShGui*          p_gui,
	VkCommandBuffer cmd_buffer,
	uint32_t        swapchain_image_idx
);

extern uint8_t SH_GUI_CALL shGuiSubmitInputs(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiRelease(
	ShGui* p_gui
);



typedef enum ShGuiInstructions {
	SH_GUI_BEGIN_COMMAND_BUFFER  = 1 << 0,
	SH_GUI_INITIALIZE            = 1 << 1,
	SH_GUI_INSTRUCTIONS_MAX_ENUM
} ShGuiInstructions;

typedef enum ShGuiWidgetFlags {
	SH_GUI_CENTER_WIDTH          = 1 <<  0,
	SH_GUI_CENTER_HEIGHT         = 1 <<  1,
	SH_GUI_EDGE_LEFT             = 1 <<  2,
	SH_GUI_EDGE_RIGHT            = 1 <<  3,
	SH_GUI_EDGE_TOP              = 1 <<  4,
	SH_GUI_EDGE_BOTTOM           = 1 <<  5,
	SH_GUI_X_MOVABLE             = 1 <<  6,
	SH_GUI_Y_MOVABLE             = 1 <<  7,
	SH_GUI_MOVABLE               = SH_GUI_X_MOVABLE | SH_GUI_Y_MOVABLE,
	SH_GUI_PIXELS                = 1 <<  8,
	SH_GUI_RELATIVE              = 1 <<  9,
	SH_GUI_MINIMIZABLE           = 1 << 10,
	SH_GUI_RESIZABLE             = 1 << 11,
	SH_GUI_POPUP                 = 1 << 12,
	SH_GUI_WIDGET_FLAGS_MAX_ENUM
} ShGuiWidgetFlags;

typedef enum ShGuiWriteFlags {
	SH_GUI_WIDTH                = 1 << 0,
	SH_GUI_HEIGHT               = 1 << 1,
	SH_GUI_POSITION_X           = 1 << 2,
	SH_GUI_POSITION_Y           = 1 << 3,
	SH_GUI_TITLE                = 1 << 4,
	SH_GUI_WRITE_FLAGS_MAX_ENUM
} ShGuiWriteFlags;


#define SH_GUI_SELECTED_ITEM_TEXT_PRIORITY    0.1f
#define SH_GUI_SELECTED_ITEM_REGION_PRIORITY  0.1001f
#define SH_GUI_SELECTED_TEXT_PRIORITY         0.101f
#define SH_GUI_SELECTED_REGION_PRIORITY       0.11f
											  
#define SH_GUI_ITEM_TEXT_PRIORITY             SH_GUI_SELECTED_ITEM_TEXT_PRIORITY   * 1.1f//0.11
#define SH_GUI_ITEM_REGION_PRIORITY           SH_GUI_SELECTED_ITEM_REGION_PRIORITY * 1.1f//0.121
#define SH_GUI_TEXT_PRIORITY                  SH_GUI_SELECTED_TEXT_PRIORITY        * 1.1f//0.1221
#define SH_GUI_REGION_PRIORITY                SH_GUI_SELECTED_REGION_PRIORITY      * 1.1f//0.12221

#define SH_GUI_MINIMUM_PRIORITY_INCREASE_STEP (-0.0001f)

#define SH_GUI_CHAR_DISTANCE_OFFSET           3.0f
#define SH_GUI_SEPARATOR_OFFSET               5.0f
#define SH_GUI_CHAR_X_OFFSET(\
	char_distance_offset,\
	scale\
)\
	(((float)char_distance_offset) / 4.0f * ((float)scale))
#define SH_GUI_CHAR_Y_OFFSET(\
	char_distance_offset,\
	line_count,\
	scale\
)\
	(((float)scale) * ((float)line_count))

#define SH_GUI_CHAR_FINAL_X_OFFSET(\
	char_distance_offset,\
	scale,\
	char_idx\
)\
	((float)char_distance_offset) / 4.0f * ((float)scale) * ((float)char_idx)
#define SH_GUI_CHAR_FINAL_Y_OFFSET\
	SH_GUI_CHAR_Y_OFFSET 

extern uint8_t SH_GUI_CALL shGuiLinkInputs(
	ShGui*           p_gui,
	uint32_t*        p_window_width,
	uint32_t*        p_window_height,
	float*           p_cursor_pos_x,
	float*           p_cursor_pos_y,
	ShGuiKeyEvents   key_events,
	ShGuiMouseEvents mouse_events,
	double*          p_delta_time
);

extern uint8_t SH_GUI_CALL shGuiResetInputs(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiUpdateInputs(
	ShGui* p_gui
);

extern uint8_t SH_GUI_CALL shGuiSetDefaultValues(
	ShGui*                  p_gui,
	shguivec4*              p_default_region_color,
	shguivec4*              p_default_region_edge_color,
	shguivec4*              p_default_char_color,
	ShGuiDefaultValuesFlags flags
);

extern uint8_t SH_GUI_CALL shGuiScaleUI(
	ShGui* p_gui,
	float  region_scaling_factor,
	float  char_scaling_factor
);

extern uint8_t SH_GUI_CALL shGuiRegion(
	ShGui*           p_gui,
	shguivec2        first_position,
	shguivec2        first_scale,
	shguivec4        first_color,
	shguivec4        first_edge_color,
	ShGuiMouseButton move_mouse_button, 
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags flags,
	ShGuiInputFlags  input_flags
);

extern uint8_t SH_GUI_CALL shGuiOverwriteRegion(
	ShGui*                   p_gui,
	uint32_t                 region_idx,
	ShGuiRegionRaw*          p_src_data,
	ShGuiRegionRawWriteFlags flags
);

extern uint8_t SH_GUI_CALL shGuiOverwriteRegions(
	ShGui*                   p_gui,
	uint32_t                 first_region,
	uint32_t                 region_count,
	ShGuiRegionRaw*          p_src_data,
	ShGuiRegionRawWriteFlags flags
);

extern uint8_t SH_GUI_CALL shGuiTextLineCount(
	const char* s_text,
	uint32_t*   p_count
);

extern uint8_t SH_GUI_CALL shGuiText(
	ShGui*           p_gui,
	shguivec2        first_position, 
	shguivec4        first_color,
	float            first_scale,
	char*            s_text,
	ShGuiWidgetFlags flags
);

extern uint8_t SH_GUI_CALL shGuiColorMatrix(
	ShGui* p_gui,
	uint32_t   matrix_width,
	uint32_t   matrix_height,
	shguivec4* p_colors
);

extern uint8_t SH_GUI_CALL shGuiOverwriteChar(
	ShGui*                 p_gui,
	uint32_t               char_idx,
	ShGuiCharRaw*          p_src_data,
	ShGuiCharRawWriteFlags flags
);

extern uint8_t SH_GUI_CALL shGuiOverwriteChars(
	ShGui*                 p_gui,
	uint32_t               first_char,
	uint32_t               char_count,
	ShGuiCharRaw*          p_src_data,
	ShGuiCharRawWriteFlags flags
);


extern uint8_t SH_GUI_CALL shGuiItem(
	ShGui*           p_gui, 
	shguivec2        position,
	shguivec2        scale,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	uint8_t          move_mouse_button,
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags region_flags,
	ShGuiInputFlags  input_flags,
	char*            text,
	float            text_scale,
	shguivec4        text_color
);

extern uint8_t SH_GUI_CALL shGuiWindow(
	ShGui*           p_gui, 
	shguivec2        position,
	shguivec2        scale,
	shguivec4        window_color,
	shguivec4        window_edge_color,
	shguivec4        bar_color,
	shguivec4        bar_edge_color,
	ShGuiMouseButton move_mouse_button,
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags region_flags,
	char*            title,
	shguivec4        title_color
);

extern uint8_t SH_GUI_CALL shGuiWindowText(
	ShGui*           p_gui,
	shguivec4        color,
	float            scale,
	char*            text, 
	ShGuiWidgetFlags flags
);

extern uint8_t SH_GUI_CALL shGuiWindowButton(
	ShGui*           p_gui, 
	float            scale,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	char*            text,
	shguivec4        text_color,
	ShGuiWidgetFlags flags,
	ShGuiInputFlags  input_flags
);

extern uint8_t SH_GUI_CALL shGuiWindowSeparator(
	ShGui*    p_gui,
	shguivec4 color
);

extern uint8_t SH_GUI_CALL shGuiWindowSliderf(
	ShGui*           p_gui, 
	float            scale, 
	float            min, 
	float            max, 
	char*            hint, 
	shguivec4        rail_color,
	shguivec4        rail_edge_color,
	shguivec4        button_color,
	shguivec4        button_edge_color,
	shguivec4        text_color,
	float*           p_dst, 
	ShGuiWidgetFlags flags
);

extern uint8_t SH_GUI_CALL shGuiMenuBar(
	ShGui*           p_gui, 
	float            extent,
	shguivec4        color,
	shguivec4        edge_color,
	ShGuiWidgetFlags flags
);

extern uint8_t SH_GUI_CALL shGuiMenuItem(
	ShGui*           p_gui,
	char*            title,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	float            text_scale,
	shguivec4        text_color,
	ShGuiWidgetFlags anchor_flags
);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H