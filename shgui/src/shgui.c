#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "shgui/shgui.h"

#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkMemoryInfo.h>
#include <shvulkan/shVkDrawLoop.h>

#include "fonts/consolas.h"

#include <string.h>

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif//_MSC_VER

const char* shGuiReadBinary(const char* path, uint32_t* p_code_size) {

	FILE* stream = fopen(path, "rb");

	if (stream == NULL) { return NULL; }

	fseek(stream, 0, SEEK_END);
	uint32_t code_size = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char* code = (char*)malloc(code_size);
	if (code == NULL) { free(stream); return NULL; }

	fread(code, code_size, 1, stream);
	(p_code_size != NULL) && (*p_code_size = code_size);

	fclose(stream);

	return code;
}



uint8_t SH_GUI_CALL shGuiBuildRegionPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items) {
	shGuiError(
		p_gui == NULL,
		"invalid gui memory",
		return 0
	);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		p_gui->physical_device,
		p_gui->surface,
		&surface_capabilities
	);

	{//PUSH CONSTANT
		shSetPushConstants(
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			80,
			&p_gui->region_infos.graphics_pipeline.push_constant_range
		);
	}//PUSH CONSTANT


	{//FIXED STATES
		shSetFixedStates(
			p_gui->device, 
			surface_capabilities.currentExtent.width, 
			surface_capabilities.currentExtent.height, 
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			VK_POLYGON_MODE_FILL, 
			&p_gui->region_infos.fixed_states
		);
	}//FIXED STATES

	{//SHADER STAGES
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary("../shaders/bin/shgui-region.vert.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->region_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-region.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->region_infos.graphics_pipeline);
		free(src);
	}//SHADER STAGES

	{//DESCRIPTORS
		{
			uint32_t host_visible_available_video_memory = 0;
			{
				uint32_t host_memory_type_index = 0;
				shGetMemoryType(
					p_gui->device,
					p_gui->physical_device,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&host_memory_type_index
				);
				VkPhysicalDeviceMemoryBudgetPropertiesEXT heap_budget = { 0 };
				shGetMemoryBudgetProperties(p_gui->physical_device, NULL, NULL, &heap_budget);
				host_visible_available_video_memory = (uint32_t)heap_budget.heapBudget[host_memory_type_index];
			}
			uint32_t device_available_video_memory = 0;
			{
				uint32_t device_memory_type_index = 0;
				shGetMemoryType(
					p_gui->device,
					p_gui->physical_device,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					&device_memory_type_index
				);
				VkPhysicalDeviceMemoryBudgetPropertiesEXT heap_budget = { 0 };
				shGetMemoryBudgetProperties(p_gui->physical_device, NULL, NULL, &heap_budget);
				device_available_video_memory = (uint32_t)heap_budget.heapBudget[device_memory_type_index];
			}
			uint32_t available_gpu_heap = host_visible_available_video_memory <= device_available_video_memory ? host_visible_available_video_memory : device_available_video_memory;
			available_gpu_heap /= 2;
			uint32_t max_items_size = max_gui_items * sizeof(ShGuiRegion);
			p_gui->region_infos.regions_data_size = available_gpu_heap >= max_items_size ? max_items_size : available_gpu_heap;
		}
		p_gui->region_infos.p_regions_data = calloc(1, p_gui->region_infos.regions_data_size);
		p_gui->region_infos.p_regions_overwritten_data = calloc(1, p_gui->region_infos.regions_data_size / sizeof(ShGuiRegion));

		shPipelineCreateDescriptorBuffer(
			p_gui->device, 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			0, 
			p_gui->region_infos.regions_data_size, 
			&p_gui->region_infos.graphics_pipeline
		);
		shPipelineAllocateDescriptorBufferMemory(
			p_gui->device,
			p_gui->physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			&p_gui->region_infos.graphics_pipeline
		);
		shPipelineBindDescriptorBufferMemory(
			p_gui->device,
			0,
			0,
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineDescriptorSetLayout(p_gui->device, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateDescriptorPool(p_gui->device, 0, &p_gui->region_infos.graphics_pipeline);
		shPipelineAllocateDescriptorSet(p_gui->device, 0, &p_gui->region_infos.graphics_pipeline);

		{
			{
				shCreateBuffer(
					p_gui->device,
					p_gui->region_infos.regions_data_size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					&p_gui->region_infos.staging_buffer
				);
				shAllocateMemory(
					p_gui->device,
					p_gui->physical_device,
					p_gui->region_infos.staging_buffer,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&p_gui->region_infos.staging_buffer_memory
				);
				shBindMemory(
					p_gui->device,
					p_gui->region_infos.staging_buffer,
					0,
					p_gui->region_infos.staging_buffer_memory
				);
			}
		}
	}
	//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->device, render_pass, p_gui->region_infos.fixed_states, &p_gui->region_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	return 1;
}

uint8_t SH_GUI_CALL shGuiBuilTextPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items) {
	shGuiError(
		p_gui == NULL,
		"invalid gui memory",
		return 0
	);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		p_gui->physical_device,
		p_gui->surface,
		&surface_capabilities
	);

	{//PUSH CONSTANT
		shSetPushConstants(
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			80,
			&p_gui->text_infos.graphics_pipeline.push_constant_range
		);
	}//PUSH CONSTANT


	{//FIXED STATES
		shSetFixedStates(
			p_gui->device,
			surface_capabilities.currentExtent.width,
			surface_capabilities.currentExtent.height,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_POLYGON_MODE_FILL,
			&p_gui->text_infos.fixed_states
		);
	}//FIXED STATES

	{//SHADER STAGES
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary("../shaders/bin/shgui-text.vert.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-text.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
	}//SHADER STAGES

	{//DESCRIPTORS
		
	}
	//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->device, render_pass, p_gui->text_infos.fixed_states, &p_gui->text_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	p_gui->text_infos.p_text_data = calloc(max_gui_items, sizeof(ShGuiText));

	return 1;
}

uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	//WRITE REGIONS DATA
	//
	//
	shWriteMemory(
		p_gui->device,
		p_gui->region_infos.staging_buffer_memory,
		0,
		p_gui->region_infos.regions_data_size,
		p_gui->region_infos.p_regions_data
	);

	{
		if (record) {
			shWaitForFence(p_gui->device, &p_gui->fence);
			shResetFence(p_gui->device, &p_gui->fence);
			shBeginCommandBuffer(p_gui->cmd_buffer);
		}

		shCopyBuffer(p_gui->cmd_buffer, p_gui->region_infos.staging_buffer, 0, 0, p_gui->region_infos.regions_data_size, p_gui->region_infos.graphics_pipeline.descriptor_buffers[0]);
		
		if (record) {
			shEndCommandBuffer(p_gui->cmd_buffer);
			shQueueSubmit(1, &p_gui->cmd_buffer, p_gui->graphics_queue.queue, p_gui->fence);
			shWaitForFence(p_gui->device, &p_gui->fence);
		}
	}
	//
	//
	//

	//WRITE TEXT DATA
	//
	//
	

	//
	//
	//

	return 1;
}

uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	shBindPipeline(p_gui->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, &p_gui->region_infos.graphics_pipeline);

	float push_constant_data[32] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,

		(float)p_gui->region_infos.fixed_states.scissor.extent.width, (float)p_gui->region_infos.fixed_states.scissor.extent.height
	};
	shPipelinePushConstants(p_gui->cmd_buffer, push_constant_data, &p_gui->region_infos.graphics_pipeline);

	shPipelineUpdateDescriptorSets(p_gui->device, &p_gui->region_infos.graphics_pipeline);

	shPipelineBindDescriptorSets(
		p_gui->cmd_buffer, 
		0, 
		p_gui->region_infos.graphics_pipeline.descriptor_count, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		&p_gui->region_infos.graphics_pipeline
	);

	shEndPipeline(&p_gui->region_infos.graphics_pipeline);

	shDraw(p_gui->cmd_buffer, p_gui->region_infos.region_count * 6);

	p_gui->region_infos.region_count = 0;

	for (uint32_t text_idx = 0; text_idx < p_gui->text_infos.text_count; text_idx++) {
		free(p_gui->text_infos.p_text_data[text_idx].p_chars);
	}
	p_gui->text_infos.text_count = 0;

	return 1;
}

uint8_t SH_GUI_CALL shGuiGetEvents(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float cursor_x = *p_gui->inputs.p_cursor_pos_x;
	float cursor_y = *p_gui->inputs.p_cursor_pos_y;
	
	for (uint32_t item_idx = 0; item_idx < p_gui->region_infos.region_count; item_idx++) {
		ShGuiRegion* p_item	= &p_gui->region_infos.p_regions_data[item_idx];
		float item_size_x	= p_item->size_position[0];
		float item_size_y	= p_item->size_position[1];
		float item_pos_x	= p_item->size_position[2];
		float item_pos_y	= p_item->size_position[3];
		
		if (
			(cursor_x >= item_pos_x - (item_size_x / 2.0f)) && 
			(cursor_x <= item_pos_x + (item_size_x / 2.0f)) &&
			(cursor_y >= item_pos_y - (item_size_y / 2.0f)) &&
			(cursor_y <= item_pos_y + (item_size_y / 2.0f))
			) {

			if (p_gui->inputs.p_mouse_events[0] == 1) {
				p_item->size_position[2] = cursor_x;
				p_item->size_position[3] = cursor_y;
				p_gui->region_infos.p_regions_overwritten_data[item_idx] = 1;
			}

		}
	}

	return 1;
}

uint8_t SH_GUI_CALL shGuiWindow(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* name) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	if (!p_gui->region_infos.p_regions_overwritten_data[p_gui->region_infos.region_count]) {
		float window_size_x = (float)p_gui->region_infos.fixed_states.scissor.extent.width;
		float window_size_y = (float)p_gui->region_infos.fixed_states.scissor.extent.height;

		ShGuiItem item = {
			{//region
				{ width, height, pos_x, -pos_y }
			},
			{//text
				0//load font vertices and indices
			}
		};

		memcpy(&p_gui->region_infos.p_regions_data[p_gui->region_infos.region_count], &item.region, sizeof(ShGuiRegion));
		
		if (name != NULL) {
			for (uint32_t char_idx = 0; char_idx < strlen(name); char_idx++) {
				switch (char_idx) {

				}
			}
		}
	}


	p_gui->region_infos.region_count++;

	return 1;
}

#define SH_GUI_LOAD_CHAR(font, char_name, _char)\
	_char.vertex_count = sizeof(font ## _ ## char_name ## _vertices) / 4;\
	_char.index_count = sizeof(font ## _ ## char_name ## _indices) / 4;\
	_char.p_vertices = (float*)(font ## _ ## char_name ## _vertices);\
	_char.p_indices = (uint32_t*)(font ## _ ## char_name ## _indices);\

uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, const char* s_text, const float scale, const float pos_x, const float pos_y) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	shGuiError(s_text == NULL, "invalid text memory", return 0);

	ShGuiText* p_text = &p_gui->text_infos.p_text_data[p_gui->text_infos.text_count];
	p_text->p_chars = (ShGuiChar*)calloc(strlen(s_text), sizeof(ShGuiChar));
	shGuiError(p_text->p_chars == NULL, "invalid gui text chars", return 0);

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		switch (s_text[char_idx]) {
		case '0':
			SH_GUI_LOAD_CHAR(consolas, 0, p_text->p_chars[char_idx]);
			break;
		case '1':
			SH_GUI_LOAD_CHAR(consolas, 1, p_text->p_chars[char_idx]);
			break;
		case '2':
			SH_GUI_LOAD_CHAR(consolas, 2, p_text->p_chars[char_idx]);
			break;
		case '3':
			SH_GUI_LOAD_CHAR(consolas, 3, p_text->p_chars[char_idx]);
			break;
		case '4':
			SH_GUI_LOAD_CHAR(consolas, 4, p_text->p_chars[char_idx]);
			break;
		case '5':
			SH_GUI_LOAD_CHAR(consolas, 5, p_text->p_chars[char_idx]);
			break;
		case '6':
			SH_GUI_LOAD_CHAR(consolas, 6, p_text->p_chars[char_idx]);
			break;
		case '7':
			SH_GUI_LOAD_CHAR(consolas, 7, p_text->p_chars[char_idx]);
			break;
		case '8':
			SH_GUI_LOAD_CHAR(consolas, 8, p_text->p_chars[char_idx]);
			break;
		case '9':
			SH_GUI_LOAD_CHAR(consolas, 9, p_text->p_chars[char_idx]);
			break;
		case 'a':
			SH_GUI_LOAD_CHAR(consolas, a, p_text->p_chars[char_idx]);
			break;
		case '&':
			SH_GUI_LOAD_CHAR(consolas, and, p_text->p_chars[char_idx]);
			break;
		case '@':
			SH_GUI_LOAD_CHAR(consolas, at, p_text->p_chars[char_idx]);
			break;
		case 'b':
			SH_GUI_LOAD_CHAR(consolas, b, p_text->p_chars[char_idx]);
			break;
		case '\\':
			SH_GUI_LOAD_CHAR(consolas, backslash, p_text->p_chars[char_idx]);
			break;
		case 'A':
			SH_GUI_LOAD_CHAR(consolas, bigA, p_text->p_chars[char_idx]);
			break;
		case 'B':
			SH_GUI_LOAD_CHAR(consolas, bigB, p_text->p_chars[char_idx]);
			break;
		case 'C':
			SH_GUI_LOAD_CHAR(consolas, bigC, p_text->p_chars[char_idx]);
			break;
		case 'D':
			SH_GUI_LOAD_CHAR(consolas, bigD, p_text->p_chars[char_idx]);
			break;
		case 'E':
			SH_GUI_LOAD_CHAR(consolas, bigE, p_text->p_chars[char_idx]);
			break;
		case 'F':
			SH_GUI_LOAD_CHAR(consolas, bigF, p_text->p_chars[char_idx]);
			break;
		case 'G':
			SH_GUI_LOAD_CHAR(consolas, bigG, p_text->p_chars[char_idx]);
			break;
		case '>':
			SH_GUI_LOAD_CHAR(consolas, bigger, p_text->p_chars[char_idx]);
			break;
		case 'H':
			SH_GUI_LOAD_CHAR(consolas, bigH, p_text->p_chars[char_idx]);
			break;
		case 'I':
			SH_GUI_LOAD_CHAR(consolas, bigI, p_text->p_chars[char_idx]);
			break;
		case 'J':
			SH_GUI_LOAD_CHAR(consolas, bigJ, p_text->p_chars[char_idx]);
			break;
		case 'K':
			SH_GUI_LOAD_CHAR(consolas, bigK, p_text->p_chars[char_idx]);
			break;
		case 'L':
			SH_GUI_LOAD_CHAR(consolas, bigL, p_text->p_chars[char_idx]);
			break;
		case 'M':
			SH_GUI_LOAD_CHAR(consolas, bigM, p_text->p_chars[char_idx]);
			break;
		case 'N':
			SH_GUI_LOAD_CHAR(consolas, bigN, p_text->p_chars[char_idx]);
			break;
		case 'O':
			SH_GUI_LOAD_CHAR(consolas, bigO, p_text->p_chars[char_idx]);
			break;
		case 'P':
			SH_GUI_LOAD_CHAR(consolas, bigP, p_text->p_chars[char_idx]);
			break;
		case 'Q':
			SH_GUI_LOAD_CHAR(consolas, bigQ, p_text->p_chars[char_idx]);
			break;
		case 'R':
			SH_GUI_LOAD_CHAR(consolas, bigR, p_text->p_chars[char_idx]);
			break;
		case 'S':
			SH_GUI_LOAD_CHAR(consolas, bigS, p_text->p_chars[char_idx]);
			break;
		case 'T':
			SH_GUI_LOAD_CHAR(consolas, bigT, p_text->p_chars[char_idx]);
			break;
		case 'U':
			SH_GUI_LOAD_CHAR(consolas, bigU, p_text->p_chars[char_idx]);
			break;
		case 'V':
			SH_GUI_LOAD_CHAR(consolas, bigV, p_text->p_chars[char_idx]);
			break;
		case 'W':
			SH_GUI_LOAD_CHAR(consolas, bigW, p_text->p_chars[char_idx]);
			break;
		case 'X':
			SH_GUI_LOAD_CHAR(consolas, bigX, p_text->p_chars[char_idx]);
			break;
		case 'Y':
			SH_GUI_LOAD_CHAR(consolas, bigY, p_text->p_chars[char_idx]);
			break;
		case 'Z':
			SH_GUI_LOAD_CHAR(consolas, bigZ, p_text->p_chars[char_idx]);
			break;
		case 'c':
			SH_GUI_LOAD_CHAR(consolas, c, p_text->p_chars[char_idx]);
			break;
		case '°':
			SH_GUI_LOAD_CHAR(consolas, circle, p_text->p_chars[char_idx]);
			break;
		case '^':
			SH_GUI_LOAD_CHAR(consolas, circumflex, p_text->p_chars[char_idx]);
			break;
		case '}':
			SH_GUI_LOAD_CHAR(consolas, close_curly_bracket, p_text->p_chars[char_idx]);
			break;
		case ')':
			SH_GUI_LOAD_CHAR(consolas, close_round_bracket, p_text->p_chars[char_idx]);
			break;
		case ']':
			SH_GUI_LOAD_CHAR(consolas, close_square_bracket, p_text->p_chars[char_idx]);
			break;
		case ':':
			SH_GUI_LOAD_CHAR(consolas, colon, p_text->p_chars[char_idx]);
			break;
		case ',':
			SH_GUI_LOAD_CHAR(consolas, comma, p_text->p_chars[char_idx]);
			break;
		case 'd':
			SH_GUI_LOAD_CHAR(consolas, d, p_text->p_chars[char_idx]);
			break;
		case '-':
			SH_GUI_LOAD_CHAR(consolas, dash, p_text->p_chars[char_idx]);
			break;
		case '$':
			SH_GUI_LOAD_CHAR(consolas, dollar, p_text->p_chars[char_idx]);
			break;
		case '.':
			SH_GUI_LOAD_CHAR(consolas, dot, p_text->p_chars[char_idx]);
			break;
		case '"':
			SH_GUI_LOAD_CHAR(consolas, double_quote, p_text->p_chars[char_idx]);
			break;
		case 'e':
			SH_GUI_LOAD_CHAR(consolas, e, p_text->p_chars[char_idx]);
			break;
		case '=':
			SH_GUI_LOAD_CHAR(consolas, equal, p_text->p_chars[char_idx]);
			break;
		case '!':
			SH_GUI_LOAD_CHAR(consolas, exclamation, p_text->p_chars[char_idx]);
			break;
		case 'f':
			SH_GUI_LOAD_CHAR(consolas, f, p_text->p_chars[char_idx]);
			break;
		case '/':
			SH_GUI_LOAD_CHAR(consolas, frontslash, p_text->p_chars[char_idx]);
			break;
		case 'g':
			SH_GUI_LOAD_CHAR(consolas, g, p_text->p_chars[char_idx]);
			break;
		case 'h':
			SH_GUI_LOAD_CHAR(consolas, h, p_text->p_chars[char_idx]);
			break;
		case '#':
			SH_GUI_LOAD_CHAR(consolas, hashtag, p_text->p_chars[char_idx]);
			break;
		case 'i':
			SH_GUI_LOAD_CHAR(consolas, i, p_text->p_chars[char_idx]);
			break;
		case 'j':
			SH_GUI_LOAD_CHAR(consolas, j, p_text->p_chars[char_idx]);
			break;
		case 'k':
			SH_GUI_LOAD_CHAR(consolas, k, p_text->p_chars[char_idx]);
			break;
		case 'l':
			SH_GUI_LOAD_CHAR(consolas, l, p_text->p_chars[char_idx]);
			break;
		case 'm':
			SH_GUI_LOAD_CHAR(consolas, m, p_text->p_chars[char_idx]);
			break;
		case 'n':
			SH_GUI_LOAD_CHAR(consolas, n, p_text->p_chars[char_idx]);
			break;
		case 'o':
			SH_GUI_LOAD_CHAR(consolas, o, p_text->p_chars[char_idx]);
			break;
		case '{':
			SH_GUI_LOAD_CHAR(consolas, open_curly_bracket, p_text->p_chars[char_idx]);
			break;
		case '(':
			SH_GUI_LOAD_CHAR(consolas, open_round_bracket, p_text->p_chars[char_idx]);
			break;
		case '[':
			SH_GUI_LOAD_CHAR(consolas, open_square_bracket, p_text->p_chars[char_idx]);
			break;
		case 'p':
			SH_GUI_LOAD_CHAR(consolas, p, p_text->p_chars[char_idx]);
			break;
		case '§':
			SH_GUI_LOAD_CHAR(consolas, paragraph, p_text->p_chars[char_idx]);
			break;
		case '%':
			SH_GUI_LOAD_CHAR(consolas, percent, p_text->p_chars[char_idx]);
			break;
		case 'plus':
			break;
		case 'q':
			SH_GUI_LOAD_CHAR(consolas, q, p_text->p_chars[char_idx]);
			break;
		case '?':
			SH_GUI_LOAD_CHAR(consolas, question_mark, p_text->p_chars[char_idx]);
			break;
		case '\'':
			SH_GUI_LOAD_CHAR(consolas, quote, p_text->p_chars[char_idx]);
			break;
		case 'r':
			SH_GUI_LOAD_CHAR(consolas, r, p_text->p_chars[char_idx]);
			break;
		case 's':
			SH_GUI_LOAD_CHAR(consolas, s, p_text->p_chars[char_idx]);
			break;
		case ';':
			SH_GUI_LOAD_CHAR(consolas, semicolon, p_text->p_chars[char_idx]);
			break;
		case '<':
			SH_GUI_LOAD_CHAR(consolas, smaller, p_text->p_chars[char_idx]);
			break;
		case '*':
			SH_GUI_LOAD_CHAR(consolas, star, p_text->p_chars[char_idx]);
			break;
		case '£':
			SH_GUI_LOAD_CHAR(consolas, sterlin, p_text->p_chars[char_idx]);
			break;
		case '|':
			SH_GUI_LOAD_CHAR(consolas, stick, p_text->p_chars[char_idx]);
			break;
		case 't':
			SH_GUI_LOAD_CHAR(consolas, t, p_text->p_chars[char_idx]);
			break;
		case 'u':
			break;
		case '_':
			SH_GUI_LOAD_CHAR(consolas, underscore, p_text->p_chars[char_idx]);
			break;
		case 'v':
			SH_GUI_LOAD_CHAR(consolas, v, p_text->p_chars[char_idx]);
			break;
		case 'w':
			SH_GUI_LOAD_CHAR(consolas, w, p_text->p_chars[char_idx]);
			break;
		case 'x':
			SH_GUI_LOAD_CHAR(consolas, x, p_text->p_chars[char_idx]);
			break;
		case 'y':
			SH_GUI_LOAD_CHAR(consolas, y, p_text->p_chars[char_idx]);
			break;
		case 'z':
			SH_GUI_LOAD_CHAR(consolas, z, p_text->p_chars[char_idx]);
			break;
		case 'à':
			SH_GUI_LOAD_CHAR(consolas, à, p_text->p_chars[char_idx]);
			break;
		case 'ç':
			SH_GUI_LOAD_CHAR(consolas, ç, p_text->p_chars[char_idx]);
			break;
		case 'è':
			SH_GUI_LOAD_CHAR(consolas, è, p_text->p_chars[char_idx]);
			break;
		case 'é':
			SH_GUI_LOAD_CHAR(consolas, é, p_text->p_chars[char_idx]);
			break;
		case 'ì':
			SH_GUI_LOAD_CHAR(consolas, ì, p_text->p_chars[char_idx]);
			break;
		case 'ò':
			SH_GUI_LOAD_CHAR(consolas, ò, p_text->p_chars[char_idx]);
			break;
		case 'ù':
			SH_GUI_LOAD_CHAR(consolas, ù, p_text->p_chars[char_idx]);
			break;
		}
	}

	p_gui->text_infos.text_count++;

	return 1;
}

uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui) {
	shClearBufferMemory(p_gui->device, p_gui->region_infos.staging_buffer, p_gui->region_infos.staging_buffer_memory);
	shPipelineClearDescriptorBufferMemory(p_gui->device, 0, &p_gui->region_infos.graphics_pipeline);
	shPipelineRelease(p_gui->device, &p_gui->region_infos.graphics_pipeline);
	
	shPipelineRelease(p_gui->device, &p_gui->text_infos.graphics_pipeline);
	return 1;
}



#ifdef __cplusplus
}
#endif//__cplusplus