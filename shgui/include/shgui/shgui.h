#ifndef SH_GUI_H
#define SH_GUI_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "shgui/shgui-call.h"
#include "shgui/shgui-assert.h"

#include <stdint.h>
#ifndef alignas
#include "../../external/stdalign.in.h"
#endif//alignas

#include <shvulkan/shVkCore.h>
#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkDescriptorStructureMap.h>

typedef struct ShVkQueue ShGuiQueue;



#define SH_GUI_KEY_LAST 348
typedef int8_t ShGuiKeyEvents[SH_GUI_KEY_LAST + 1];
#define SH_GUI_MOUSE_LAST 1
typedef int8_t ShGuiMouseEvents[SH_GUI_MOUSE_LAST + 1];

#define SH_GUI_CURSOR_NORMAL 0
#define SH_GUI_CURSOR_HORIZONTAL_RESIZE 1
#define SH_GUI_CURSOR_VERTICAL_RESIZE 2
typedef int32_t ShGuiCursorIcons[3];//normal, horizontal_resize, vertical_resize

typedef struct ShGuiInputs {
	uint32_t*				p_window_width;
	uint32_t*				p_window_height;
	float*					p_cursor_pos_x;
	float*					p_cursor_pos_y;
	int8_t*					p_key_events;
	int8_t*					p_mouse_events;
	int32_t*				p_cursor_icons;
	int32_t					active_cursor_icon;
	float*					p_delta_time;
	
	struct {
		float				last_cursor_pos_x;
		float				last_cursor_pos_y;
		ShGuiKeyEvents		last_key_events;
		ShGuiMouseEvents	last_mouse_events;
	} last;
} ShGuiInputs;



typedef struct ShGuiRegion {
	alignas(16) float size_position[4];
} ShGuiRegion;



typedef struct ShGuiCharInfo {
	alignas(16) float position_scale[4];
} ShGuiCharInfo;

SH_VULKAN_GENERATE_DESCRIPTOR_STRUCTURE_MAP(ShGuiCharInfo)



typedef struct ShGuiChar {
	float*			p_vertices;
	uint32_t		vertex_count;
} ShGuiChar;



#ifndef SH_GUI_MAX_CHAR_VERTEX_SIZE 
#define SH_GUI_MAX_CHAR_VERTEX_SIZE 49 * 3
#endif//SH_GUI_MAX_CHAR_VERTEX_SIZE


#ifndef SH_GUI_TEXT_MAX_CHAR_COUNT
#define SH_GUI_TEXT_MAX_CHAR_COUNT	1024
#endif//SH_GUI_TEXT_MAX_CHAR_COUNT



typedef struct ShGuiText {
	char		text[SH_GUI_TEXT_MAX_CHAR_COUNT];
	ShGuiChar	chars[SH_GUI_TEXT_MAX_CHAR_COUNT];
} ShGuiText;



typedef struct ShGuiItem {
	ShGuiRegion region;
	ShGuiText*	p_text;
} ShGuiItem;


typedef enum ShGuiDefaultValues{
	SH_GUI_THEME_DARK	= 0b0001,
	SH_GUI_THEME_LIGHT	= 0b0010,
	SH_GUI_DEFAULT_VALUES_MAX_ENUM
} ShGuiDefaultValues;

typedef enum ShGuiInstructions {
	SH_GUI_RECORD		= 0b001,
	SH_GUI_INITIALIZE	= 0b010
} ShGuiInstructions;

typedef enum ShGuiWidgetFlags {
	SH_GUI_TOP			= 0b0000000001,
	SH_GUI_BOTTOM		= 0b0000000010,
	SH_GUI_LEFT			= 0b0000000100,
	SH_GUI_RIGHT		= 0b0000001000,
	SH_GUI_MOVABLE		= 0b0000010000,
	SH_GUI_PIXELS		= 0b0000100000,
	SH_GUI_RELATIVE		= 0b0001000000,
	SH_GUI_MINIMIZABLE	= 0b0010000000,
	SH_GUI_RESIZABLE	= 0b0100000000,
	SH_GUI_SWITCH		= 0b1000000000
} ShGuiWidgetFlags;

typedef enum ShGuiWriteFlags {
	SH_GUI_WIDTH		= 0b00001,
	SH_GUI_HEIGHT		= 0b00010,
	SH_GUI_POSITION_X	= 0b00100,
	SH_GUI_POSITION_Y	= 0b01000,
	SH_GUI_TITLE		= 0b10000
} ShGuiWriteFlags;


typedef struct ShGuiCore {
	VkDevice					device;
	VkPhysicalDevice			physical_device;
	ShGuiQueue					graphics_queue;
	VkCommandBuffer				cmd_buffer;
	VkFence						fence;
	VkSurfaceKHR				surface;
} ShGuiCore;


typedef struct ShGui {
	ShGuiCore					core;
	ShGuiInputs					inputs;


	struct {
		VkBuffer			staging_buffer;
		VkDeviceMemory		staging_memory;
	} default_infos;

	uint32_t					active_item_idx;

	struct {
		VkBuffer				staging_buffer;
		VkDeviceMemory			staging_memory;
		uint32_t				regions_data_size;

		ShGuiRegion*			p_regions_data;
		uint8_t*				p_regions_overwritten_data;	//gonna merge
		uint8_t*				p_regions_clicked;			//gonna merge
		uint8_t*				p_regions_active;			//gonna merge
		uint32_t				region_count;

		ShVkPipeline			graphics_pipeline;
		ShVkFixedStates			fixed_states;
	} region_infos;
	
	struct {
		float								char_distance_offset;
		ShGuiCharInfoDescriptorStructureMap	char_info_map;
		uint32_t							total_char_count;


		VkBuffer							vertex_staging_buffer;
		VkDeviceMemory						vertex_staging_memory;

		VkBuffer							vertex_buffer;
		VkDeviceMemory						vertex_memory;
		uint32_t							vertex_count;


		ShGuiText*							p_text_data;
		uint32_t							text_count;

		ShVkPipeline						graphics_pipeline;
		ShVkFixedStates						fixed_states;
	} text_infos;


} ShGui;

extern ShGui* shGuiInit(ShGuiCore core);

#define SH_GUI_REGION_CONDITION(p_gui, condition, additional_to_condition, additional_to_widget, widget_func_name, dimension_0, ...)\
	if ((uint8_t)(condition)) {\
	((p_gui)->region_infos.p_regions_active[gui.region_infos.region_count] = 1 * (p_gui)->region_infos.p_regions_active[gui.region_infos.region_count] == 0); additional_to_condition; }\
	if ((p_gui)->region_infos.p_regions_active[gui.region_infos.region_count]) { if (widget_func_name(p_gui, dimension_0, __VA_ARGS__)) { (p_gui)->region_infos.p_regions_active[gui.region_infos.region_count - 2] = 0; additional_to_widget; } }\
	else { widget_func_name(p_gui, 0.0f, __VA_ARGS__); (p_gui)->region_infos.p_regions_active[gui.region_infos.region_count] = 0; }


#ifdef _MSC_VER
#pragma warning (disable: 6011)
#endif//_MSC_VER



extern uint8_t SH_GUI_CALL shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, ShGuiKeyEvents key_events, ShGuiMouseEvents mouse_events, ShGuiCursorIcons icons, float* p_delta_time, ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiUpdateInputs(ShGui* p_gui);



static uint8_t SH_GUI_CALL shGuiSetGraphicsQueue(const uint32_t graphics_queue_family_index, const VkQueue graphics_queue, ShGui* p_gui) {
	p_gui->core.graphics_queue.queue = graphics_queue;
	p_gui->core.graphics_queue.queue_family_index = graphics_queue_family_index;
}



extern uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiBuildTextPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiSetDefaultValues(ShGui* p_gui, const ShGuiDefaultValues values, const ShGuiInstructions instruction);

extern uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record);

extern uint8_t SH_GUI_CALL shGuiGetEvents(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRegion(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* name, const ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiRegionWrite(ShGui* p_gui, const uint32_t region_idx, const float width, const float height, const float pos_x, const float pos_y, const char* name, const ShGuiWidgetFlags flags, const ShGuiWriteFlags write_flags);

extern uint8_t SH_GUI_CALL shGuiMenuBar(ShGui* p_gui, const float extent, const char* title, const ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiMenuItem(ShGui* p_gui, const float extent, const char* title, const ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, const float scale, const float pos_x, const float pos_y, const char* text);

extern uint8_t SH_GUI_CALL shGuiInputField(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiDestroyPipelines(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H