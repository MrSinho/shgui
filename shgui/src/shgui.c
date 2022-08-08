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


ShGui* shGuiInit(ShGuiCore core) {
	ShGui* p_gui = (ShGui*)calloc(1, sizeof(ShGui));
	memcpy(&p_gui->core, &core, sizeof(ShGuiCore));
	return p_gui;
}


uint8_t shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, ShGuiKeyEvents key_events, ShGuiMouseEvents mouse_events, ShGuiCursorIcons icons, float* p_delta_time, ShGui* p_gui) {
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
		(int32_t*)icons,
		0,
		p_delta_time
	};
	p_gui->inputs = inputs;
	return 1;
}

uint8_t shGuiUpdateInputs(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	p_gui->inputs.last.last_cursor_pos_x = *p_gui->inputs.p_cursor_pos_x;
	p_gui->inputs.last.last_cursor_pos_y = *p_gui->inputs.p_cursor_pos_y;

	memcpy(p_gui->inputs.last.last_key_events, p_gui->inputs.p_key_events, sizeof(ShGuiKeyEvents));
	memcpy(p_gui->inputs.last.last_mouse_events, p_gui->inputs.p_mouse_events, sizeof(ShGuiMouseEvents));

	return 1;
}

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
			p_gui->core.device,
			p_gui->core.physical_device,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&host_memory_type_index
		);
		VkPhysicalDeviceMemoryBudgetPropertiesEXT heap_budget = { 0 };
		shGetMemoryBudgetProperties(p_gui->core.physical_device, NULL, NULL, &heap_budget);
		host_visible_available_video_memory = (uint32_t)heap_budget.heapBudget[host_memory_type_index];
	}
	uint32_t device_available_video_memory = 0;
	{
		uint32_t device_memory_type_index = 0;
		shGetMemoryType(
			p_gui->core.device,
			p_gui->core.physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&device_memory_type_index
		);
		VkPhysicalDeviceMemoryBudgetPropertiesEXT heap_budget = { 0 };
		shGetMemoryBudgetProperties(p_gui->core.physical_device, NULL, NULL, &heap_budget);
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
		p_gui->core.physical_device,
		p_gui->core.surface,
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
			p_gui->core.device, 
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
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->region_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-region.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->region_infos.graphics_pipeline);
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
		p_gui->region_infos.p_regions_clicked = calloc(1, p_gui->region_infos.regions_data_size / sizeof(ShGuiRegion));
		p_gui->region_infos.p_regions_active = calloc(1, p_gui->region_infos.regions_data_size / sizeof(ShGuiRegion));
		p_gui->region_infos.menus.p_menu_indices = calloc(1, p_gui->region_infos.regions_data_size / sizeof(ShGuiRegion));

		shGuiError(p_gui->region_infos.p_regions_data == NULL, "invalid regions data memory", return 0);
		shGuiError(p_gui->region_infos.p_regions_overwritten_data == NULL, "invalid regions overwrite memory", return 0);
		shGuiError(p_gui->region_infos.p_regions_clicked == NULL, "invalid regions clicked memory", return 0);
		shGuiError(p_gui->region_infos.p_regions_active == NULL, "invalid regions active memory", return 0);
		shGuiError(p_gui->region_infos.menus.p_menu_indices == NULL, "invalid menu indices memory", return 0);

		memset(p_gui->region_infos.p_regions_active, 1, p_gui->region_infos.regions_data_size / sizeof(ShGuiRegion));

		shPipelineCreateDescriptorBuffer(
			p_gui->core.device, 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			0, 
			p_gui->region_infos.regions_data_size, 
			&p_gui->region_infos.graphics_pipeline
		);
		shPipelineAllocateDescriptorBufferMemory(
			p_gui->core.device,
			p_gui->core.physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			&p_gui->region_infos.graphics_pipeline
		);
		shPipelineBindDescriptorBufferMemory(
			p_gui->core.device,
			0,
			0,
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineDescriptorSetLayout(p_gui->core.device, 
			0, 
			0, 
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineCreateDescriptorPool(
			p_gui->core.device, 
			0, 
			&p_gui->region_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorSet(
			p_gui->core.device, 
			0, 
			&p_gui->region_infos.graphics_pipeline
		);

		{
			{
				shCreateBuffer(
					p_gui->core.device,
					p_gui->region_infos.regions_data_size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					&p_gui->region_infos.staging_buffer
				);
				shAllocateMemory(
					p_gui->core.device,
					p_gui->core.physical_device,
					p_gui->region_infos.staging_buffer,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&p_gui->region_infos.staging_memory
				);
				shBindMemory(
					p_gui->core.device,
					p_gui->region_infos.staging_buffer,
					0,
					p_gui->region_infos.staging_memory
				);
			}
		}

		{
			shPipelineCreateDescriptorBuffer(
				p_gui->core.device,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				1,
				16,
				&p_gui->region_infos.graphics_pipeline
			);
			shPipelineAllocateDescriptorBufferMemory(
				p_gui->core.device,
				p_gui->core.physical_device,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				1,
				&p_gui->region_infos.graphics_pipeline
			);
			shPipelineBindDescriptorBufferMemory(p_gui->core.device, 1, 0, &p_gui->region_infos.graphics_pipeline);
			shPipelineDescriptorSetLayout(
				p_gui->core.device, 
				1, 
				0, 
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
				VK_SHADER_STAGE_FRAGMENT_BIT, 
				&p_gui->region_infos.graphics_pipeline
			);
			shPipelineCreateDescriptorPool(p_gui->core.device, 1, &p_gui->region_infos.graphics_pipeline);
			shPipelineAllocateDescriptorSet(p_gui->core.device, 1, &p_gui->region_infos.graphics_pipeline);
		}
	}
	//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->core.device, render_pass, p_gui->region_infos.fixed_states, &p_gui->region_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	return 1;
}

uint8_t SH_GUI_CALL shGuiBuildTextPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items) {
	shGuiError(
		p_gui == NULL,
		"invalid gui memory",
		return 0
	);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		p_gui->core.physical_device,
		p_gui->core.surface,
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
			p_gui->core.device,
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
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-text.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
	}//SHADER STAGES

	{//DESCRIPTORS
	
		VkPhysicalDeviceProperties physical_device_properties = { 0 };
		{
			vkGetPhysicalDeviceProperties(p_gui->core.physical_device, &physical_device_properties);

			p_gui->text_infos.char_info_map = shVkCreateShGuiCharInfoDescriptorStructures(
				physical_device_properties,
				max_gui_items * SH_GUI_TEXT_MAX_CHAR_COUNT
			);
		}

		shPipelineCreateDynamicDescriptorBuffer(
			p_gui->core.device,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,// | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			p_gui->text_infos.char_info_map.structure_size,
			p_gui->text_infos.char_info_map.structure_count,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorBufferMemory(
			p_gui->core.device,
			p_gui->core.physical_device,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineBindDescriptorBufferMemory(
			p_gui->core.device,
			0,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineDescriptorSetLayout(
			p_gui->core.device,
			0,
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineCreateDescriptorPool(
			p_gui->core.device,
			0,
			&p_gui->text_infos.graphics_pipeline
		);

		shPipelineAllocateDescriptorSet(
			p_gui->core.device,
			0,
			&p_gui->text_infos.graphics_pipeline
		);


		shPipelineCreateDescriptorBuffer(
			p_gui->core.device,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			1,
			16,
			&p_gui->text_infos.graphics_pipeline
		);
		shPipelineAllocateDescriptorBufferMemory(
			p_gui->core.device,
			p_gui->core.physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			1,
			&p_gui->text_infos.graphics_pipeline
		);
		shPipelineBindDescriptorBufferMemory(p_gui->core.device, 1, 0, &p_gui->text_infos.graphics_pipeline);
		shPipelineDescriptorSetLayout(
			p_gui->core.device,
			1,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			&p_gui->text_infos.graphics_pipeline
		);
		shPipelineCreateDescriptorPool(p_gui->core.device, 1, &p_gui->text_infos.graphics_pipeline);
		shPipelineAllocateDescriptorSet(p_gui->core.device, 1, &p_gui->text_infos.graphics_pipeline);

	}//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->core.device, render_pass, p_gui->text_infos.fixed_states, &p_gui->text_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	p_gui->text_infos.p_text_data = calloc(max_gui_items, sizeof(ShGuiText));
	shGuiError(p_gui->text_infos.p_text_data == NULL, "invalid text data memory", return 0);

	{//VERTEX BUFFER
		shCreateBuffer(
			p_gui->core.device,
			SH_GUI_MAX_CHAR_VERTEX_SIZE * max_gui_items,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			&p_gui->text_infos.vertex_staging_buffer
		);
		shAllocateMemory(
			p_gui->core.device,
			p_gui->core.physical_device,
			p_gui->text_infos.vertex_staging_buffer,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&p_gui->text_infos.vertex_staging_memory
		);
		shBindMemory(
			p_gui->core.device,
			p_gui->text_infos.vertex_staging_buffer,
			0,
			p_gui->text_infos.vertex_staging_memory
		);


		shCreateBuffer(
			p_gui->core.device, 
			SH_GUI_MAX_CHAR_VERTEX_SIZE * max_gui_items, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			&p_gui->text_infos.vertex_buffer
		);
		shAllocateMemory(
			p_gui->core.device,
			p_gui->core.physical_device,
			p_gui->text_infos.vertex_buffer,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&p_gui->text_infos.vertex_memory
		);
		shBindVertexBufferMemory(p_gui->core.device, p_gui->text_infos.vertex_buffer, 0, p_gui->text_infos.vertex_memory);
	}//VERTEX BUFFER

	p_gui->text_infos.char_distance_offset = 3.0f;

	return 1;
}

uint8_t shGuiSetDefaultValues(ShGui* p_gui, const ShGuiDefaultValues values, const ShGuiInstructions instructions) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	shGuiError(values >= SH_GUI_DEFAULT_VALUES_MAX_ENUM, "invalid gui default values", return 0)

	VkDevice			device			= p_gui->core.device;
	VkCommandBuffer		cmd_buffer		= p_gui->core.cmd_buffer;
	VkFence				fence			= p_gui->core.fence;

	
	float staging_data[8] = { 0 };

	if (instructions & SH_GUI_INITIALIZE) {
		shCreateBuffer(
			device,
			sizeof(staging_data),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			&p_gui->default_infos.staging_buffer
		);
		shAllocateMemory(
			device,
			p_gui->core.physical_device,
			p_gui->default_infos.staging_buffer,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&p_gui->default_infos.staging_memory
		);
		shBindMemory(
			device,
			p_gui->default_infos.staging_buffer,
			0,
			p_gui->default_infos.staging_memory
		);
	}

	if (values & SH_GUI_THEME_DARK) {
		staging_data[0] = 0.05f;//WINDOWS
		staging_data[1] = 0.05f;
		staging_data[2] = 0.05f;

		staging_data[4] = 1.0f;//TEXT
		staging_data[5] = 1.0f;
		staging_data[6] = 1.0f;
	}
	if (values & SH_GUI_THEME_LIGHT) {
		staging_data[0] = 0.8f;//WINDOWS
		staging_data[1] = 0.8f;
		staging_data[2] = 0.8f;

		staging_data[4] = 0.0f;//TEXT
		staging_data[5] = 0.0f;
		staging_data[6] = 0.0f;
	}

	shWriteMemory(device, p_gui->default_infos.staging_memory, 0, sizeof(staging_data), staging_data);

	if (instructions & SH_GUI_RECORD) {
		shWaitForFence(device, &fence);
		shResetFence(device, &fence);
		shBeginCommandBuffer(cmd_buffer);
	}
	
	shCopyBuffer(
		cmd_buffer,
		p_gui->default_infos.staging_buffer,
		0,
		0,
		16,
		p_gui->region_infos.graphics_pipeline.descriptor_buffers[1]
	);

	shCopyBuffer(
		cmd_buffer,
		p_gui->default_infos.staging_buffer,
		16,
		0,
		16,
		p_gui->text_infos.graphics_pipeline.descriptor_buffers[1]
	);

	if (instructions & SH_GUI_RECORD) {
		shEndCommandBuffer(cmd_buffer);
		shQueueSubmit(1, &cmd_buffer, p_gui->core.graphics_queue.queue, fence);
		shWaitForFence(device, &fence);
	}

	return 1;
}

uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice device				= p_gui->core.device;
	VkCommandBuffer cmd_buffer	= p_gui->core.cmd_buffer;

	//WRITE REGIONS DATA
	//
	//
	float null_region[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (uint32_t region_idx = 0; region_idx < p_gui->region_infos.region_count; region_idx++) {
		uint8_t active = p_gui->region_infos.p_regions_active[region_idx];
		if (active) {
			shWriteMemory(
				device,
				p_gui->region_infos.staging_memory,
				16 * region_idx,
				16,
				p_gui->region_infos.p_regions_data[region_idx].raw.size_position
			);
		}
		else {
			shWriteMemory(
				device,
				p_gui->region_infos.staging_memory,
				16 * region_idx,
				16,
				null_region
			);
		}
	}


	for (uint32_t text_idx = 0; text_idx < p_gui->text_infos.text_count; text_idx++) {
		uint32_t text_char_count = (uint32_t)strlen(p_gui->text_infos.p_text_data[text_idx].text);
		for (uint32_t char_idx = 0; char_idx < text_char_count; char_idx++) {
			uint32_t vertex_count = p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count;
			shWriteMemory(
				device,
				p_gui->text_infos.vertex_staging_memory,
				p_gui->text_infos.vertex_count * 4,//total
				vertex_count * 4,//local
				p_gui->text_infos.p_text_data[text_idx].chars[char_idx].p_vertices
			);
			p_gui->text_infos.vertex_count += vertex_count;
		}
	}
	
	//COPY BUFFERS
	//
	//
	{
		if (record) {
			shWaitForFence(device, &p_gui->core.fence);
			shResetFence(device, &p_gui->core.fence);
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
			p_gui->text_infos.vertex_staging_buffer,
			0,
			0,
			p_gui->text_infos.vertex_count * 4,
			p_gui->text_infos.vertex_buffer
		);


		if (record) {
			shEndCommandBuffer(cmd_buffer);
			shQueueSubmit(1, &cmd_buffer, p_gui->core.graphics_queue.queue, p_gui->core.fence);
			shWaitForFence(device, &p_gui->core.fence);
		}
	}
	//
	//
	//

	return 1;
}

uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice			device				= p_gui->core.device;
	VkCommandBuffer		cmd_buffer			= p_gui->core.cmd_buffer;

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
	p_gui->region_infos.menus.menu_count = 0;


	

	shBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_text_pipeline);

	shPipelinePushConstants(cmd_buffer, push_constant_data, p_text_pipeline);

	shPipelineUpdateDescriptorSets(
		device,
		&p_gui->text_infos.graphics_pipeline
	);

	shPipelineBindDescriptorSets(
		cmd_buffer,
		1,
		1,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		&p_gui->text_infos.graphics_pipeline
	);

	//Worst implementation ever seen (comparable to shengine), but i'm in hurry
	uint32_t total_char_idx = 0;
	uint32_t total_vertex_idx = 0;
	for (uint32_t text_idx = 0; text_idx < p_gui->text_infos.text_count; text_idx++) {//to improve: reduce draw calls :(
		for (uint32_t char_idx = 0; char_idx < strlen(p_gui->text_infos.p_text_data[text_idx].text); char_idx++) {
			ShGuiCharInfo* p_char = shVkGetShGuiCharInfoDescriptorStructure(p_gui->text_infos.char_info_map, total_char_idx, 0);
			
			shPipelineWriteDynamicDescriptorBufferMemory(
				device,
				0,
				(void*)p_char,
				&p_gui->text_infos.graphics_pipeline
			);

			shPipelineBindDynamicDescriptorSet(
				p_gui->core.cmd_buffer,
				0,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				&p_gui->text_infos.graphics_pipeline
			);

			shBindVertexBuffer(cmd_buffer, 0, total_vertex_idx * 4, &p_gui->text_infos.vertex_buffer);
			total_vertex_idx += p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count;

			shDraw(cmd_buffer, p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count / 3);
			total_char_idx++;

		}
	}

	//if (p_gui->text_infos.vertex_count > 0) {
	//	shDraw(cmd_buffer, p_gui->text_infos.vertex_count / 3);
	//}

	shEndPipeline(p_text_pipeline);

	p_gui->text_infos.text_count = 0;
	p_gui->text_infos.total_char_count = 0;
	p_gui->text_infos.vertex_count = 0;

	return 1;
}

uint8_t SH_GUI_CALL shGuiGetEvents(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float cursor_x = *p_gui->inputs.p_cursor_pos_x;
	float cursor_y = *p_gui->inputs.p_cursor_pos_y;
	
	//for (uint32_t item_idx = 0; item_idx < p_gui->region_infos.region_count; item_idx++) {
	//	ShGuiRegion* p_region = &p_gui->region_infos.p_regions_data[item_idx];
	//	float item_size_x	= p_region ->size_position[0];
	//	float item_size_y	= p_region ->size_position[1];
	//	float item_pos_x	= p_region ->size_position[2];
	//	float item_pos_y	= p_region ->size_position[3];
	//	
	//	if (
	//		(cursor_x >= item_pos_x - (item_size_x / 2.0f)) && 
	//		(cursor_x <= item_pos_x + (item_size_x / 2.0f)) &&
	//		(cursor_y >= item_pos_y - (item_size_y / 2.0f)) &&
	//		(cursor_y <= item_pos_y + (item_size_y / 2.0f))
	//		) {
	//
	//		if (p_gui->inputs.p_mouse_events[0] == 1) {
	//			p_region ->size_position[2] = cursor_x;
	//			p_region ->size_position[3] = cursor_y;
	//
	//
	//
	//			//p_gui->region_infos.p_regions_overwritten_data[item_idx] = 1;
	//		}
	//
	//	}
	//}

	return 1;
}

uint8_t SH_GUI_CALL shGuiRegion(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float cursor_x = *p_gui->inputs.p_cursor_pos_x;
	float cursor_y = *p_gui->inputs.p_cursor_pos_y;

	float window_size_x = (float)p_gui->region_infos.fixed_states.scissor.extent.width;
	float window_size_y = (float)p_gui->region_infos.fixed_states.scissor.extent.height;

	ShGuiItem item = {
			{
				{
					{
						width, height, pos_x, -pos_y
					}//size_position
				},//raw
				flags
			},//region
			&p_gui->text_infos.p_text_data[p_gui->text_infos.text_count]
	};
	
	if (flags & SH_GUI_PIXELS) {
		//fine
	}

	else if (flags & SH_GUI_RELATIVE) {
		item.region.raw.size_position[0] = width / 100.0f * window_size_x;
		item.region.raw.size_position[1] = height / 100.0f * window_size_y;
		item.region.raw.size_position[2] = pos_x / 100.0f * window_size_x / 2.0f;
		item.region.raw.size_position[3] = -pos_y / 100.0f * window_size_y / 2.0f;
	}

	uint32_t		region_count	= p_gui->region_infos.region_count;
	ShGuiRegion*	p_region		= &p_gui->region_infos.p_regions_data[region_count];
	uint8_t			overwritten		= p_gui->region_infos.p_regions_overwritten_data[p_gui->region_infos.region_count];

	//p_region->flags = flags;

	if (!overwritten) {
		memcpy(p_region, &item.region, sizeof(ShGuiRegion));
	}


	//if (flags & SH_GUI_MINIMIZABLE) {
	//	shGuiText(
	//		p_gui, 
	//		text_size, 
	//		p_region->raw.size_position[2] + p_region->raw.size_position[0] / 2.0f - text_size,
	//		-p_region->raw.size_position[3] + p_region->raw.size_position[1] / 2.0f - text_size,
	//		"O"
	//	);
	//}

	if (flags & SH_GUI_RESIZABLE) {
		float limit_left = p_region->raw.size_position[2] - p_region->raw.size_position[0] / 2.0f;
		float limit_right = p_region->raw.size_position[2] + p_region->raw.size_position[0] / 2.0f;
		float limit_top = p_region->raw.size_position[3] - p_region->raw.size_position[1] / 2.0f;
		float limit_bottom = p_region->raw.size_position[3] + p_region->raw.size_position[1] / 2.0f;
		//p_gui->inputs.active_cursor_icon = p_gui->inputs.p_cursor_icons[SH_GUI_CURSOR_NORMAL];

		const float check_size = 6.0f;

		uint8_t horizontal_right	=	(cursor_x >= limit_left - check_size && cursor_x <= limit_left + check_size) &&
										(cursor_y <= limit_bottom && cursor_y >= limit_top);
		uint8_t horizontal_left		=	(cursor_x <= limit_right + check_size && cursor_x >= limit_right - check_size) &&
										(cursor_y <= limit_bottom && cursor_y >= limit_top);
		uint8_t vertical_top		=	(cursor_y >= limit_bottom - check_size && cursor_y <= limit_bottom + check_size) &&
										(cursor_x >= limit_left && cursor_x <= limit_right);
		uint8_t vertical_bottom		=	(cursor_y <= limit_top + check_size && cursor_y >= limit_top - check_size) &&
										(cursor_x >= limit_left && cursor_x <= limit_right);

		float d_cursor_pos_x = (*p_gui->inputs.p_cursor_pos_x) - p_gui->inputs.last.last_cursor_pos_x;
		float d_cursor_pos_y = (*p_gui->inputs.p_cursor_pos_y) - p_gui->inputs.last.last_cursor_pos_y;

		if (horizontal_left || horizontal_right) {
			p_gui->inputs.active_cursor_icon = p_gui->inputs.p_cursor_icons[SH_GUI_CURSOR_HORIZONTAL_RESIZE];
			p_gui->region_infos.p_regions_overwritten_data[p_gui->region_infos.region_count] = 1;
		}
		if (vertical_top || vertical_bottom) {
			p_gui->inputs.active_cursor_icon = p_gui->inputs.p_cursor_icons[SH_GUI_CURSOR_VERTICAL_RESIZE];
			p_gui->region_infos.p_regions_overwritten_data[p_gui->region_infos.region_count] = 1;
		}

		if (p_gui->inputs.p_mouse_events[0]) {
			if (horizontal_left) {
				p_region->raw.size_position[0] += 1000.0f * d_cursor_pos_x * (*p_gui->inputs.p_delta_time);
			}
			if (horizontal_right) {
				p_region->raw.size_position[0] -= 1000.0f * d_cursor_pos_x * (*p_gui->inputs.p_delta_time);
			}
			if (vertical_top) {
				p_region->raw.size_position[1] += 1000.0f * d_cursor_pos_y * (*p_gui->inputs.p_delta_time);
			}
			if (vertical_bottom) {
				p_region->raw.size_position[1] -= 1000.0f * d_cursor_pos_y * (*p_gui->inputs.p_delta_time);
			}
		}
	}

	uint8_t* p_clicked = &p_gui->region_infos.p_regions_clicked[p_gui->region_infos.region_count];
	if ((flags & SH_GUI_SWITCH) == 0) {
		(*p_clicked) = 0;
	}

	if (
	(cursor_x >= p_region->raw.size_position[2] - p_region->raw.size_position[0] / 2.0f + 10.0f) &&
	(cursor_x <= p_region->raw.size_position[2] + p_region->raw.size_position[0] / 2.0f - 10.0f) &&
	(cursor_y >= p_region->raw.size_position[3] - p_region->raw.size_position[1] / 2.0f + 10.0f) &&
	(cursor_y <= p_region->raw.size_position[3] + p_region->raw.size_position[1] / 2.0f - 10.0f)
	) {
		if (p_gui->inputs.p_mouse_events[1] == 1 && (flags & SH_GUI_MOVABLE)) {
			
			float dx = cursor_x - p_gui->inputs.last.last_cursor_pos_x;
			float dy = cursor_y - p_gui->inputs.last.last_cursor_pos_y;

			p_region->raw.size_position[2]		+= dx;
			p_region->raw.size_position[3]		+= dy;
		
			p_gui->region_infos.p_regions_overwritten_data[region_count] = 1;
		}
		if (p_gui->inputs.p_mouse_events[0] == 1 && p_gui->inputs.last.last_mouse_events[0] == 0) {
			uint8_t rtrn = (*p_clicked) == 0;
			(*p_clicked) = 1;
			p_gui->region_infos.region_count++;
			return rtrn;
		}
	}

	(*p_clicked) = 0;
	p_gui->region_infos.region_count++;

	return 0;
}

uint8_t SH_GUI_CALL shGuiRegionWrite(ShGui* p_gui, const uint32_t region_idx, const float width, const float height, const float pos_x, const float pos_y, const char* name, const ShGuiWidgetFlags flags, const ShGuiWriteFlags write_flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	if (write_flags & SH_GUI_WIDTH) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size_position[0] = width;
	}
	if (write_flags & SH_GUI_HEIGHT) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size_position[1] = height;
	}
	if (write_flags & SH_GUI_POSITION_X) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size_position[2] = pos_x;
	}
	if (write_flags & SH_GUI_POSITION_Y) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size_position[3] = pos_y;
	}

	p_gui->region_infos.p_regions_overwritten_data[region_idx] = 1;

	return 1;
}

uint8_t SH_GUI_CALL shGuiItem(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* name, const ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t		region_count	= p_gui->region_infos.region_count;
	ShGuiRegion*	p_region		= &p_gui->region_infos.p_regions_data[region_count];

	uint8_t sig = shGuiRegion(p_gui, width, height, pos_x, pos_y, flags);

	float text_size = 15.0f;
	if (name != NULL) {
		shGuiText(
			p_gui,
			text_size,
			p_region->raw.size_position[2] - p_region->raw.size_position[0] / 2.0f + text_size / 2.0f,
			-p_region->raw.size_position[3] + p_region->raw.size_position[1] / 2.0f - text_size,
			name
		);
	}

	return sig;
}

uint8_t SH_GUI_CALL shGuiWindow(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* title, const ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	uint32_t region_count = p_gui->region_infos.region_count;
	ShGuiRegion* main_region = &p_gui->region_infos.p_regions_data[region_count];

	uint8_t sig = shGuiItem(p_gui, width, height, pos_x, pos_y, title, flags);
	
	//bar region
		float bar_size = 20.0f;
		float* main_size_position = main_region->raw.size_position;
		shGuiRegion(
			p_gui, 
			main_size_position[0],
			bar_size, 
			main_size_position[2],
			-main_size_position[3] + main_size_position[1] / 2.0f - bar_size / 2.0f,
			SH_GUI_PIXELS
		);

	//close region

	return sig;
}

uint8_t SH_GUI_CALL shGuiMenuBar(ShGui* p_gui, const float extent, const ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float region_width = 100.0f;
	float region_height = extent;
	float region_pos_x = 0.0f;
	float region_pos_y = 100.0f;

	if (flags & SH_GUI_TOP || flags & SH_GUI_BOTTOM) {
		region_width = 100.0f;
		region_height = extent;
		region_pos_x = 0.0f;
		region_pos_y = 100.0f - extent;
		if (flags & SH_GUI_BOTTOM) {
			region_pos_y = -100.0f + extent;
		}
	}
	if (flags & SH_GUI_LEFT || flags & SH_GUI_RIGHT) {
		region_width = extent;
		region_height = 100.0f;
		region_pos_x = -100.0f + extent;
		region_pos_y = 0.0f;
		if (flags & SH_GUI_RIGHT) {
			region_pos_x = 100.0f - extent;
		}
	}

	p_gui->region_infos.menus.p_menu_indices[p_gui->region_infos.menus.menu_count] = p_gui->region_infos.region_count;

	shGuiRegion(
		p_gui, region_width, region_height, region_pos_x, region_pos_y, flags | SH_GUI_RELATIVE
	);

	p_gui->region_infos.menus.menu_count++;

	return 1;
}

uint8_t SH_GUI_CALL shGuiMenuItem(ShGui* p_gui, const float extent, const char* title, const ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float text_size = 15.0f;

	float width = 10.0f;//%
	ShGuiWidgetFlags additional_flags = SH_GUI_RELATIVE;
	if (title != NULL) {
		additional_flags &= ~SH_GUI_RELATIVE;
		additional_flags |= SH_GUI_PIXELS;
		width = (strlen(title) + 1.0f) * p_gui->text_infos.char_distance_offset * text_size / 4.0f;//in pixels
	}
	
	uint32_t bar_idx = p_gui->region_infos.menus.menu_count - 1;
	ShGuiRegion* bar = &p_gui->region_infos.p_regions_data[p_gui->region_infos.menus.p_menu_indices[bar_idx]];
	
	float size_position[4] = { 0.0f };
	
	if (bar->flags & SH_GUI_TOP || bar->flags & SH_GUI_BOTTOM) {
		size_position[0] = width;
		size_position[1] = bar->raw.size_position[1];
		size_position[2] = -bar->raw.size_position[0] / 2.0f + size_position[0] / 2.0f;
		size_position[3] = bar->raw.size_position[3];
	}
	else if (bar->flags & SH_GUI_LEFT || bar->flags && SH_GUI_RIGHT) {
		size_position[0] = width;
		size_position[1] = text_size + 8.0f;
		size_position[2] = bar->raw.size_position[2];
		size_position[3] = -bar->raw.size_position[1] / 2.0f + size_position[1] / 2.0f;
	}
	else {
		return 0;
	}

	return shGuiItem(p_gui, size_position[0], size_position[1], size_position[2], -size_position[3], title, flags | additional_flags);
}

#define SH_GUI_LOAD_CHAR(font, char_name, _char)\
	_char.vertex_count = sizeof(font ## _ ## char_name ## _vertices) / 4;\
	_char.p_vertices = (float*)(font ## _ ## char_name ## _vertices);\

uint8_t SH_GUI_CALL shGuiText(ShGui* p_gui, const float scale, const float pos_x, const float pos_y, const char* s_text) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	shGuiError(s_text == NULL, "invalid text memory", return 0);

	ShGuiText* p_text = &p_gui->text_infos.p_text_data[p_gui->text_infos.text_count];
	strcpy(p_text->text, s_text);

	float window_size_x = (float)p_gui->region_infos.fixed_states.scissor.extent.width;
	float window_size_y = (float)p_gui->region_infos.fixed_states.scissor.extent.height;

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		ShGuiCharInfo* p_char_info = shVkGetShGuiCharInfoDescriptorStructure(p_gui->text_infos.char_info_map, p_gui->text_infos.total_char_count, 0);
		
		float char_distance_offset = p_gui->text_infos.char_distance_offset;

		p_char_info->position_scale[0] = pos_x + char_distance_offset / 4.0f * scale * char_idx;
		p_char_info->position_scale[1] = -pos_y;
		p_char_info->position_scale[2] = scale;
		
		p_gui->text_infos.total_char_count++;

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

uint8_t SH_GUI_CALL shGuiDestroyPipelines(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	shFixedStatesRelease(&p_gui->region_infos.fixed_states);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 0, &p_gui->region_infos.graphics_pipeline);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 1, &p_gui->region_infos.graphics_pipeline);
	shPipelineRelease(p_gui->core.device, &p_gui->region_infos.graphics_pipeline);
	

	shFixedStatesRelease(&p_gui->text_infos.fixed_states);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 0, &p_gui->text_infos.graphics_pipeline);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 1, &p_gui->text_infos.graphics_pipeline);
	shPipelineRelease(p_gui->core.device, &p_gui->text_infos.graphics_pipeline);

	return 1;
}

uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	shClearBufferMemory(p_gui->core.device, p_gui->default_infos.staging_buffer, p_gui->default_infos.staging_memory);

	shClearBufferMemory(p_gui->core.device, p_gui->region_infos.staging_buffer, p_gui->region_infos.staging_memory);
	
	shClearBufferMemory(p_gui->core.device, p_gui->text_infos.vertex_staging_buffer, p_gui->text_infos.vertex_staging_memory);
	shClearBufferMemory(p_gui->core.device, p_gui->text_infos.vertex_buffer, p_gui->text_infos.vertex_memory);
	
	shGuiDestroyPipelines(p_gui);

	{
		ShGuiRegion* p_regions_data = p_gui->region_infos.p_regions_data;
		uint8_t* p_regions_overwritten_data = p_gui->region_infos.p_regions_overwritten_data;
		uint8_t* p_regions_clicked = p_gui->region_infos.p_regions_clicked;
		uint8_t* p_regions_active = p_gui->region_infos.p_regions_active;
		if (p_regions_data != NULL) {
			free(p_regions_data);
		}
		if (p_regions_overwritten_data != NULL) {
			free(p_regions_overwritten_data);
		}
		if (p_regions_clicked != NULL) {
			free(p_regions_clicked);
		}
		if (p_regions_active != NULL) {
			free(p_regions_active);
		}


		ShGuiText* p_text_data = p_gui->text_infos.p_text_data;
		if (p_text_data != NULL) {
			free(p_gui->text_infos.p_text_data);
		}

		shVkReleaseShGuiCharInfoDescriptorStructureMap(&p_gui->text_infos.char_info_map);
	}

	free(p_gui);

	return 1;
}



#ifdef __cplusplus
}
#endif//__cplusplus