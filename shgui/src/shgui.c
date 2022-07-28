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


uint32_t SH_GUI_CALL shGuiGetAvailableHeap(ShGui* p_gui) {
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
	return host_visible_available_video_memory <= device_available_video_memory ? host_visible_available_video_memory : device_available_video_memory;
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
			uint32_t available_gpu_heap = shGuiGetAvailableHeap(p_gui);
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

		shPipelineDescriptorSetLayout(p_gui->device, 
			0, 
			0, 
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineCreateDescriptorPool(
			p_gui->device, 
			0, 
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorSet(
			p_gui->device, 
			0, 
			&p_gui->region_infos.graphics_pipeline
		);

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
					&p_gui->region_infos.staging_memory
				);
				shBindMemory(
					p_gui->device,
					p_gui->region_infos.staging_buffer,
					0,
					p_gui->region_infos.staging_memory
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
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_POLYGON_MODE_POINT,
			&p_gui->text_infos.fixed_states
		);

		shSetVertexInputAttribute(0, SH_VEC3_SIGNED_FLOAT, 0, 12, &p_gui->text_infos.fixed_states);

		shFixedStatesSetVertexInputRate(VK_VERTEX_INPUT_RATE_VERTEX, 0, &p_gui->text_infos.fixed_states);
		shFixedStatesSetVertexInputState(&p_gui->text_infos.fixed_states);
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
	
		VkPhysicalDeviceProperties physical_device_properties = { 0 };
		{
			vkGetPhysicalDeviceProperties(p_gui->physical_device, &physical_device_properties);

			p_gui->text_infos.char_info_map = shVkCreateShGuiCharInfoDescriptorStructures(
				physical_device_properties,
				max_gui_items * SH_GUI_TEXT_MAX_CHAR_COUNT
			);
		}

		shCreateBuffer(
			p_gui->device,
			p_gui->text_infos.char_info_map.structure_count * p_gui->text_infos.char_info_map.structure_size,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			&p_gui->text_infos.text_descriptor_staging_buffer
		);
		shAllocateMemory(
			p_gui->device,
			p_gui->physical_device,
			p_gui->text_infos.text_descriptor_staging_buffer,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&p_gui->text_infos.text_descriptor_staging_memory
		);
		shBindMemory(
			p_gui->device,
			p_gui->text_infos.text_descriptor_staging_buffer,
			0,
			p_gui->text_infos.text_descriptor_staging_memory
		);


		shPipelineCreateDescriptorBuffer(
			p_gui->device,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			p_gui->text_infos.char_info_map.structure_count * p_gui->text_infos.char_info_map.structure_size,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorBufferMemory(
			p_gui->device,
			p_gui->physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineBindDescriptorBufferMemory(
			p_gui->device,
			0,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineDescriptorSetLayout(
			p_gui->device,
			0,
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineCreateDescriptorPool(
			p_gui->device,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorSet(
			p_gui->device,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

	}//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->device, render_pass, p_gui->text_infos.fixed_states, &p_gui->text_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	p_gui->text_infos.p_text_data = calloc(max_gui_items, sizeof(ShGuiText));
	shGuiError(p_gui->text_infos.p_text_data == NULL, "invalid text data memory", return 0);

	{//VERTEX BUFFER
		shCreateBuffer(
			p_gui->device, 
			SH_GUI_MAX_CHAR_VERTEX_SIZE * max_gui_items, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			&p_gui->text_infos.vertex_buffer
		);
		shAllocateMemory(
			p_gui->device,
			p_gui->physical_device,
			p_gui->text_infos.vertex_buffer,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&p_gui->text_infos.vertex_memory
		);
		shBindVertexBufferMemory(p_gui->device, p_gui->text_infos.vertex_buffer, 0, p_gui->text_infos.vertex_memory);
	}//VERTEX BUFFER

	return 1;
}

uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice device				= p_gui->device;
	VkCommandBuffer cmd_buffer	= p_gui->cmd_buffer;

	//WRITE REGIONS DATA
	//
	//
	shWriteMemory(
		device,
		p_gui->region_infos.staging_memory,
		0,
		p_gui->region_infos.regions_data_size,
		p_gui->region_infos.p_regions_data
	);

	//WRITE TEXT DATA
	//
	//
	for (uint32_t text_idx = 0; text_idx < p_gui->text_infos.text_count; text_idx++) {
		uint32_t char_count = (uint32_t)strlen(p_gui->text_infos.p_text_data[text_idx].text);
		for (uint32_t char_idx = 0; char_idx < char_count; char_idx++) {
			uint32_t vertex_size = p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count * 4;
			shWriteMemory(
				device,
				p_gui->text_infos.vertex_memory,
				p_gui->text_infos.vertex_text_data_size,
				vertex_size,
				p_gui->text_infos.p_text_data[text_idx].chars[char_idx].p_vertices
			);
			p_gui->text_infos.vertex_text_data_size += vertex_size;

			p_gui->text_infos.vertex_count += p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count;
		}
	}
	p_gui->text_infos.vertex_text_data_size = 0;
	shVkMapShGuiCharInfoDecriptorStructures(&p_gui->text_infos.char_info_map);
	shWriteMemory(
		p_gui->device,
		p_gui->text_infos.text_descriptor_staging_memory,
		0,
		p_gui->text_infos.char_info_map.structure_size,
		p_gui->text_infos.char_info_map.p_ShGuiCharInfo_map
	);

	
	//COPY BUFFERS
	//
	//
	{
		if (record) {
			shWaitForFence(device, &p_gui->fence);
			shResetFence(device, &p_gui->fence);
			shBeginCommandBuffer(cmd_buffer);
		}

		shCopyBuffer(
			cmd_buffer, 
			p_gui->region_infos.staging_buffer, 
			0, 
			0, 
			p_gui->region_infos.regions_data_size, 
			p_gui->region_infos.graphics_pipeline.descriptor_buffers[0]
		);

		shCopyBuffer(
			cmd_buffer,
			p_gui->text_infos.text_descriptor_staging_buffer,
			0,
			0,
			p_gui->text_infos.char_info_map.structure_size,
			p_gui->text_infos.graphics_pipeline.descriptor_buffers[0]
		);

		if (record) {
			shEndCommandBuffer(cmd_buffer);
			shQueueSubmit(1, &cmd_buffer, p_gui->graphics_queue.queue, p_gui->fence);
			shWaitForFence(device, &p_gui->fence);
		}
	}
	//
	//
	//

	return 1;
}

uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice			device				= p_gui->device;
	VkCommandBuffer		cmd_buffer			= p_gui->cmd_buffer;

	ShVkPipeline*		p_region_pipeline	= &p_gui->region_infos.graphics_pipeline;
	ShVkPipeline*		p_text_pipeline		= &p_gui->text_infos.graphics_pipeline; 

	shBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_region_pipeline);

	float push_constant_data[32] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,

		(float)p_gui->region_infos.fixed_states.scissor.extent.width, (float)p_gui->region_infos.fixed_states.scissor.extent.height
	};
	shPipelinePushConstants(cmd_buffer, push_constant_data, p_region_pipeline);

	shPipelineUpdateDescriptorSets(device, p_region_pipeline);

	shPipelineBindDescriptorSets(
		cmd_buffer, 
		0, 
		p_region_pipeline->descriptor_count,
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		p_region_pipeline
	);

	shEndPipeline(p_region_pipeline);

	shDraw(cmd_buffer, p_gui->region_infos.region_count * 6);

	p_gui->region_infos.region_count = 0;



	

	shBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_text_pipeline);

	shPipelinePushConstants(cmd_buffer, push_constant_data, p_text_pipeline);

	shPipelineUpdateDescriptorSets(
		p_gui->device,
		&p_gui->text_infos.graphics_pipeline
	);

	shPipelineBindDynamicDescriptorSet(
		p_gui->cmd_buffer,
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		&p_gui->text_infos.graphics_pipeline
	);

	shBindVertexBuffer(cmd_buffer, 0, &p_gui->text_infos.vertex_buffer);

	if (p_gui->text_infos.vertex_count > 0) {
		shDraw(cmd_buffer, p_gui->text_infos.vertex_count / 3);
	}

	shEndPipeline(p_text_pipeline);

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
	_char.p_vertices = (float*)(font ## _ ## char_name ## _vertices);\

uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, const char* s_text, const float scale, const float pos_x, const float pos_y) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	shGuiError(s_text == NULL, "invalid text memory", return 0);

	ShGuiText* p_text = &p_gui->text_infos.p_text_data[p_gui->text_infos.text_count];
	strcpy(p_text->text, s_text);

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		ShGuiCharInfo* p_char_info = shVkGetShGuiCharInfoDescriptorStructure(p_gui->text_infos.char_info_map, char_idx, 0);
		p_char_info->position_scale[0] = pos_x + SH_GUI_CHAR_OFFSET_UNIT * scale * char_idx;
		p_char_info->position_scale[1] = pos_y;
		p_char_info->position_scale[2] = scale;

		switch (s_text[char_idx]) {
		case 'Q':
			SH_GUI_LOAD_CHAR(consolas, bigQ, p_text->chars[char_idx]); 
			break;
		case 'W':
			SH_GUI_LOAD_CHAR(consolas, bigW, p_text->chars[char_idx]); 
			break;
		case 'E':
			SH_GUI_LOAD_CHAR(consolas, bigE, p_text->chars[char_idx]); 
			break;
		case 'R':
			SH_GUI_LOAD_CHAR(consolas, bigR, p_text->chars[char_idx]); 
			break;
		case 'T':
			SH_GUI_LOAD_CHAR(consolas, bigT, p_text->chars[char_idx]); 
			break;
		case 'Y':
			SH_GUI_LOAD_CHAR(consolas, bigY, p_text->chars[char_idx]); 
			break;
		case 'U':
			SH_GUI_LOAD_CHAR(consolas, bigU, p_text->chars[char_idx]); 
			break;
		case 'I':
			SH_GUI_LOAD_CHAR(consolas, bigI, p_text->chars[char_idx]); 
			break;
		case 'O':
			SH_GUI_LOAD_CHAR(consolas, bigO, p_text->chars[char_idx]); 
			break;
		case 'P':
			SH_GUI_LOAD_CHAR(consolas, bigP, p_text->chars[char_idx]); 
			break;
		}
	}

	p_gui->text_infos.text_count++;

	return 1;
}

uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui) {
	shClearBufferMemory(p_gui->device, p_gui->region_infos.staging_buffer, p_gui->region_infos.staging_memory);
	shPipelineClearDescriptorBufferMemory(p_gui->device, 0, &p_gui->region_infos.graphics_pipeline);
	shPipelineRelease(p_gui->device, &p_gui->region_infos.graphics_pipeline);
	
	shPipelineRelease(p_gui->device, &p_gui->text_infos.graphics_pipeline);
	return 1;
}



#ifdef __cplusplus
}
#endif//__cplusplus