#ifndef SH_GUI_H
#define SH_GUI_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "shgui/shgui-call.h"
#include "shgui/shgui-assert.h"

#include <stdint.h>

#include <vulkan/vulkan.h>



typedef struct ShGuiQueue {
	uint32_t	queue_family_index;
	VkQueue		queue;
} ShGuiQueue;



#define SH_KEY_LAST 348
typedef int8_t shGuiKeyParameters[SH_KEY_LAST + 1];



typedef struct ShGuiInputs {
	uint32_t*			p_window_width;
	uint32_t*			p_window_height;
	float*				p_cursor_pos_x;
	float*				p_cursor_pos_y;
	shGuiKeyParameters*	p_key_parameters;
} ShGuiInputs;



typedef struct ShGui {
	VkDevice	device;
	ShGuiQueue	graphics_queue;
	VkPipeline	graphics_pipeline;
	ShGuiInputs inputs;
} ShGui;


#ifdef _MSC_VER
#pragma warning (disable: 6011)
#endif//_MSC_VER

static void SH_GUI_CALL shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, shGuiKeyParameters* p_key_parameters, ShGui* p_gui) {
	shGuiAssert(
		p_window_width && p_window_height && p_cursor_pos_x && p_cursor_pos_y && p_key_parameters && p_gui,
		"invalid arguments"
	);
	p_gui->inputs.p_window_width	= p_window_width;
	p_gui->inputs.p_window_height	= p_window_height;
	p_gui->inputs.p_cursor_pos_x	= p_cursor_pos_x;
	p_gui->inputs.p_cursor_pos_y	= p_cursor_pos_y;
	p_gui->inputs.p_key_parameters	= p_key_parameters;
}

static void SH_GUI_CALL shGuiSetGraphicsQueue(const uint32_t graphics_queue_family_index, const VkQueue graphics_queue, ShGui* p_gui) {
	p_gui->graphics_queue.queue = graphics_queue;
	p_gui->graphics_queue.queue_family_index = graphics_queue_family_index;
}



extern void SH_GUI_CALL shGuiBuildGraphicsPipeline(VkDevice device, ShGui* p_gui);

extern void SH_GUI_CALL shGuiGetEvents(ShGui* p_gui);

extern void SH_GUI_CALL shGuiWindow(ShGui* p_gui);

extern void SH_GUI_CALL shGuiText(ShGui* p_gui);

extern void SH_GUI_CALL shGuiButton(ShGui* p_gui);

extern void SH_GUI_CALL shGuiInputField(ShGui* p_gui);

extern void SH_GUI_CALL shGuiInputColorPick(ShGui* p_gui);

extern void SH_GUI_CALL shGuiRender(ShGui* p_gui);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_H