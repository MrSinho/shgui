#ifndef SH_GUI_H
#define SH_GUI_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <stdint.h>
#ifndef alignas
#include "../../external/stdalign.in.h"
#endif//alignas


#ifndef SH_GUI_DEBUG_SHADERS
#define SH_GUI_DEBUG_SHADERS 0
#else 
#define SH_GUI_DEBUG_SHADERS 1
#endif//SH_GUI_DEBUG

#include "shgui/shgui-call.h"
#include "shgui/shgui-assert.h"
#include "shgui/shgui-shaders.h"

#include <shvulkan/shVkCore.h>
#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkDescriptorStructureMap.h>



//INPUTS
//
//
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
	double*					p_delta_time;
	
	struct {
		float				last_cursor_pos_x;
		float				last_cursor_pos_y;
		ShGuiKeyEvents		last_key_events;
		ShGuiMouseEvents	last_mouse_events;
	} last;
} ShGuiInputs;


//WIDGET MACROS
//
//
#define SH_GUI_WINDOW_BAR_SIZE 20.0f
#define SH_GUI_WINDOW_TEXT_SIZE 15.0f
#define SH_GUI_WINDOW_TEXT_BORDER_OFFSET 5.0f
#define SH_GUI_WINDOW_USED_HEIGHT(window_pos_y, window_height, used_height)\
	((float)(window_pos_y)) + ((float)(window_size_y)) / 2.0f - SH_GUI_WINDOW_BAR_SIZE - ((float)(used_height))
#define SH_GUI_WINDOW_ITEMS_OFFSET 15.0f

#define SH_GUI_CURSOR_EDGE_CHECK_SIZE 6.0f

#define SH_GUI_CHAR_DISTANCE_OFFSET 3.0f
#define SH_GUI_CHAR_FINAL_OFFSET(char_distance_offset, scale, char_idx)\
	((float)char_distance_offset) / 4.0f * ((float)scale) * ((float)char_idx)


#define SH_GUI_SEPARATOR_OFFSET 5.0f


#define SH_GUI_SELECTED_TEXT_PRIORITY 0.0f
#define SH_GUI_SELECTED_ITEMS_PRIORITY 0.1f
#define SH_GUI_SELECTED_EMPTY_REGION_PRIORITY 0.2f

#define SH_GUI_TEXT_PRIORITY			SH_GUI_SELECTED_TEXT_PRIORITY * 1.1f
#define SH_GUI_ITEMS_PRIORITY			SH_GUI_SELECTED_ITEMS_PRIORITY * 1.1f
#define SH_GUI_EMPTY_REGION_PRIORITY	SH_GUI_SELECTED_EMPTY_REGION_PRIORITY * 1.1f

static float SH_GUI_EMPTY_CHAR[1] = { 0x0 };

//WIDGET PROPERTIES
//
//
typedef enum ShGuiDefaultValues {
	SH_GUI_THEME_DARK			= 0b0001,
	SH_GUI_THEME_LIGHT			= 0b0010,
	SH_GUI_DEFAULT_VALUES_MAX_ENUM
} ShGuiDefaultValues;

typedef enum ShGuiInstructions {
	SH_GUI_RECORD				= 0b001,
	SH_GUI_INITIALIZE			= 0b010,
	SH_GUI_INSTRUCTIONS_MAX_ENUM
} ShGuiInstructions;

typedef enum ShGuiWidgetFlags {
	SH_GUI_TOP					= 0b000000000001,
	SH_GUI_BOTTOM				= 0b000000000010,
	SH_GUI_LEFT					= 0b000000000100,
	SH_GUI_RIGHT				= 0b000000001000,
	SH_GUI_CENTER_WIDTH			= 0b000000010000,
	SH_GUI_CENTER_HEIGHT		= 0b000000100000,
	SH_GUI_MOVABLE				= 0b000001000000,
	SH_GUI_PIXELS				= 0b000010000000,
	SH_GUI_RELATIVE				= 0b000100000000,
	SH_GUI_MINIMIZABLE			= 0b001000000000,
	SH_GUI_RESIZABLE			= 0b010000000000,
	SH_GUI_SWITCH				= 0b100000000000,
	SH_GUI_WIDGET_FLAGS_MAX_ENUM
} ShGuiWidgetFlags;

typedef enum ShGuiWriteFlags {
	SH_GUI_WIDTH				= 0b00001,
	SH_GUI_HEIGHT				= 0b00010,
	SH_GUI_POSITION_X			= 0b00100,
	SH_GUI_POSITION_Y			= 0b01000,
	SH_GUI_TITLE				= 0b10000,
	SH_GUI_WRITE_FLAGS_MAX_ENUM
} ShGuiWriteFlags;



//REGION
//
//
typedef struct ShGuiRegionRaw {
	alignas(8)	float			position[2];
	alignas(8)	float			size[2];
	alignas(16) float			priority[4];//only priority[0] is relevant
} ShGuiRegionRaw;

typedef struct ShGuiRegion {
	ShGuiRegionRaw				raw;
	ShGuiWidgetFlags			flags;
} ShGuiRegion;



//CHAR AND TEXT
//
//
typedef struct ShGuiCharInfo {
	alignas(8) float			position[2];
	alignas(8) float			scale[2];
	alignas(16) float			priority[4];
} ShGuiCharInfo;
SH_VULKAN_GENERATE_DESCRIPTOR_STRUCTURE_MAP(ShGuiCharInfo)

typedef struct ShGuiChar {
	float*						p_vertices;
	uint32_t					vertex_count;
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



//ITEM
//
//
typedef struct ShGuiItem {
	ShGuiRegion region;
	ShGuiText*	p_text;
} ShGuiItem;



//GRAPHICS CORE
//
//
typedef struct ShVkQueue		ShGuiQueue;
typedef struct ShGuiCore {
	VkDevice					device;
	VkPhysicalDevice			physical_device;
	ShGuiQueue					graphics_queue;
	VkCommandBuffer				cmd_buffer;
	VkFence						fence;
	VkSurfaceKHR				surface;
	VkRenderPass				render_pass;
} ShGuiCore;




//GUI
//
//
typedef struct ShGui {
	ShGuiCore					core;
	ShGuiInputs					inputs;

	
	struct {
		VkBuffer			staging_buffer;
		VkDeviceMemory		staging_memory;
	} default_infos;

	struct {
		struct {
			uint32_t menu_count;
			uint32_t* p_menu_indices;
		} menus;

		struct {
			uint32_t	window_count;
			uint32_t*	p_window_indices;
			float*		p_windows_used_height;
		} windows;

		VkBuffer				staging_buffer;
		VkDeviceMemory			staging_memory;
		uint32_t				regions_data_size;

		ShGuiRegion*			p_regions_data;
		uint8_t*				p_regions_overwritten_data;	//merge bits later
		uint8_t*				p_regions_clicked;			//merge bits later
		uint8_t*				p_regions_active;			//merge bits later
		uint8_t*				p_cursor_on_regions;
		uint8_t					cursor_on_regions;
		uint32_t				region_count;

		ShVkPipeline			graphics_pipeline;
		ShVkFixedStates			fixed_states;
	} region_infos;
	
	struct {
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



extern ShGui* SH_GUI_CALL shGuiInit(ShGuiCore core);



extern uint8_t SH_GUI_CALL shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, ShGuiKeyEvents key_events, ShGuiMouseEvents mouse_events, ShGuiCursorIcons icons, double* p_delta_time, ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiUpdateInputs(ShGui* p_gui);



static uint8_t SH_GUI_CALL shGuiSetGraphicsQueue(const uint32_t graphics_queue_family_index, const VkQueue graphics_queue, ShGui* p_gui) {
	p_gui->core.graphics_queue.queue				= graphics_queue;
	p_gui->core.graphics_queue.queue_family_index	= graphics_queue_family_index;
}



extern uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(ShGui* p_gui, uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiBuildTextPipeline(ShGui* p_gui, uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiSetDefaultValues(ShGui* p_gui, ShGuiDefaultValues values, ShGuiInstructions instruction);

extern uint8_t SH_GUI_CALL shGuiRegion(ShGui* p_gui, float width, float height, float pos_x, float pos_y, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiRegionWrite(ShGui* p_gui, uint32_t region_idx, float width, float height, float pos_x, float pos_y, char* name, ShGuiWidgetFlags flags, ShGuiWriteFlags write_flags);

extern uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, float scale, float pos_x, float pos_y, char* text);


extern uint8_t SH_GUI_CALL shGuiSetRegionPriority(ShGui* p_gui, uint32_t region_idx, float priority);


extern uint8_t SH_GUI_CALL shGuiItem(ShGui* p_gui, float width, float height, float pos_x, float pos_y, char* name, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiWindow(ShGui* p_gui, float width, float height, float pos_x, float pos_y, char* title, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiWindowText(ShGui* p_gui, float scale, char* text, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiWindowButton(ShGui* p_gui, float scale, char* text, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiWindowSeparator(ShGui* p_gui);

#if 0
extern uint8_t SH_GUI_CALL shGuiWindowSlideri(ShGui* p_gui, float extent, int min, int max, int step, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiWindowInputFieldi(ShGui* p_gui, float scale, char* text, char* hint, int* p_value, ShGuiWidgetFlags flags);
#endif//0

extern uint8_t SH_GUI_CALL shGuiMenuBar(ShGui* p_gui, float extent, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiMenuItem(ShGui* p_gui, float extent, char* title, ShGuiWidgetFlags flags);

extern uint8_t SH_GUI_CALL shGuiInputField(ShGui* p_gui);



extern uint8_t SH_GUI_CALL shGuiUpdateInputs(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, uint8_t record);

extern uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiDestroyPipelines(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H