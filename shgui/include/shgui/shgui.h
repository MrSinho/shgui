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


typedef struct ShGuiInputs {
	uint32_t*			p_window_width;
	uint32_t*			p_window_height;
	float*				p_cursor_pos_x;
	float*				p_cursor_pos_y;
	int8_t*				p_key_events;
	int8_t*				p_mouse_events;
	float*				p_delta_time;
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



#define SH_GUI_TEXT_MAX_CHAR_COUNT	1024
#define SH_GUI_CHAR_OFFSET_UNIT		0.5f

typedef struct ShGuiText {
	char		text[SH_GUI_TEXT_MAX_CHAR_COUNT];
	ShGuiChar	chars[SH_GUI_TEXT_MAX_CHAR_COUNT];
} ShGuiText;



typedef struct ShGuiItem {
	ShGuiRegion region;
	ShGuiChar	text;
} ShGuiItem;



typedef struct ShGui {
	VkDevice					device;
	VkPhysicalDevice			physical_device;
	ShGuiQueue					graphics_queue;
	VkCommandBuffer				cmd_buffer;
	VkFence						fence;
	VkSurfaceKHR				surface;
	ShGuiInputs					inputs;

	uint32_t					active_item_idx;
	struct {
		VkBuffer				staging_buffer;
		VkDeviceMemory			staging_memory;
		uint32_t				regions_data_size;

		ShGuiRegion*			p_regions_data;
		uint8_t*				p_regions_overwritten_data;
		uint32_t				region_count;

		ShVkPipeline			graphics_pipeline;
		ShVkFixedStates			fixed_states;
	} region_infos;
	
	struct {
		VkBuffer							text_descriptor_staging_buffer;
		VkDeviceMemory						text_descriptor_staging_memory;

		ShGuiCharInfoDescriptorStructureMap	char_info_map;



		VkBuffer							vertex_staging_buffer;
		VkDeviceMemory						vertex_staging_memory;

		uint32_t							vertex_text_data_size;

		VkBuffer							vertex_buffer;
		VkDeviceMemory						vertex_memory;
		uint32_t							vertex_count;


		ShGuiText*							p_text_data;
		uint32_t							text_count;

		ShVkPipeline						graphics_pipeline;
		ShVkFixedStates						fixed_states;
	} text_infos;


} ShGui;



#ifdef _MSC_VER
#pragma warning (disable: 6011)
#endif//_MSC_VER



static uint8_t SH_GUI_CALL shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, ShGuiKeyEvents key_events, ShGuiMouseEvents mouse_events, float* p_delta_time, ShGui* p_gui) {
	shGuiError(
		(p_window_width && p_window_height && p_cursor_pos_x && p_cursor_pos_y && key_events && mouse_events && p_gui) == 0,
		"invalid arguments",
		return 0;
	);
	ShGuiInputs inputs = {
		p_window_width,
		p_window_height,
		p_cursor_pos_x,
		p_cursor_pos_y,
		(int8_t*)key_events,
		(int8_t*)mouse_events,
		p_delta_time
	};
	p_gui->inputs = inputs;
	return 1;
}

static uint8_t SH_GUI_CALL shGuiSetGraphicsQueue(const uint32_t graphics_queue_family_index, const VkQueue graphics_queue, ShGui* p_gui) {
	p_gui->graphics_queue.queue = graphics_queue;
	p_gui->graphics_queue.queue_family_index = graphics_queue_family_index;
}



extern uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiBuilTextPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items);

extern uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record);

extern uint8_t SH_GUI_CALL shGuiGetEvents(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiWindow(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* name);

extern uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, const char* text, const float scale, const float pos_x, const float pos_y);

extern uint8_t SH_GUI_CALL shGuiButton(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiInputField(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui);

extern uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H