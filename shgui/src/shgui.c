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
	shGuiError(p_gui == NULL, "invalid gui memory", return NULL);
	memcpy(&p_gui->core, &core, sizeof(ShGuiCore));
	return p_gui;
}


uint8_t shGuiLinkInputs(uint32_t* p_window_width, uint32_t* p_window_height, float* p_cursor_pos_x, float* p_cursor_pos_y, ShGuiKeyEvents key_events, ShGuiMouseEvents mouse_events, ShGuiCursorIcons icons, double* p_delta_time, ShGui* p_gui) {
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

	p_gui->region_infos.cursor_on_regions = 0;
	for (uint32_t region_idx = 0; region_idx < p_gui->region_infos.region_count; region_idx++) {
		if (p_gui->region_infos.p_cursor_on_regions[region_idx]) {
			p_gui->region_infos.cursor_on_regions = 1;
			break;
		}
	}

	memcpy(p_gui->inputs.last.last_key_events, p_gui->inputs.p_key_events, sizeof(ShGuiKeyEvents));
	memcpy(p_gui->inputs.last.last_mouse_events, p_gui->inputs.p_mouse_events, sizeof(ShGuiMouseEvents));

	return 1;
}

char* shGuiReadBinary(char* path, uint32_t* p_code_size) {

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


uint32_t shGuiGetAvailableHeap(ShGui* p_gui, uint32_t item_count, uint32_t item_size) {
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

	uint32_t available_gpu_heap = host_visible_available_video_memory <= device_available_video_memory ? host_visible_available_video_memory : device_available_video_memory;
	available_gpu_heap /= 2;

	uint32_t max_items_size = item_count * item_size;
	
	return available_gpu_heap >= max_items_size ? max_items_size : available_gpu_heap;
}


uint8_t shGuiBuildRegionPipeline(ShGui* p_gui, uint32_t max_gui_items) {
	shGuiError(
		p_gui == NULL,
		"invalid gui memory",
		return 0
	);

	p_gui->region_infos.max_region_items = max_gui_items;

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
#if SH_GUI_DEBUG_SHADERS
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary("../shaders/bin/shgui-region.vert.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->region_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-region.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->region_infos.graphics_pipeline);
		free(src);
#else 
		shPipelineCreateShaderModule(p_gui->core.device, sizeof(shgui_region_vert_spv), shgui_region_vert_spv, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderModule(p_gui->core.device, sizeof(shgui_region_frag_spv), shgui_region_frag_spv, &p_gui->region_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->region_infos.graphics_pipeline);
#endif//SH_GUI_DEBUG_SHADERS
	}//SHADER STAGES

	{//DESCRIPTORS

		uint32_t regions_size											= shGuiGetAvailableHeap(p_gui, max_gui_items, sizeof(ShGuiRegionRaw));
		uint32_t region_count											= regions_size / sizeof(ShGuiRegionRaw);

		p_gui->region_infos.regions_data_size							= regions_size;

		p_gui->region_infos.p_regions_data								= calloc(1, regions_size);
		p_gui->region_infos.p_regions_overwritten_data					= calloc(1, region_count);
		p_gui->region_infos.p_regions_clicked							= calloc(1, region_count);
		p_gui->region_infos.p_regions_active							= calloc(1, region_count);
		p_gui->region_infos.p_cursor_on_regions							= calloc(1, region_count);
		p_gui->region_infos.menus.p_menu_indices						= calloc(4, region_count);
		p_gui->region_infos.windows.p_window_indices					= calloc(4, region_count);
		p_gui->region_infos.windows.p_windows_used_height				= calloc(4, region_count);

		shGuiError(p_gui->region_infos.p_regions_data					== NULL, "invalid regions data memory",			return 0);
		shGuiError(p_gui->region_infos.p_regions_overwritten_data		== NULL, "invalid regions overwrite memory",	return 0);
		shGuiError(p_gui->region_infos.p_regions_clicked				== NULL, "invalid regions clicked memory",		return 0);
		shGuiError(p_gui->region_infos.p_regions_active					== NULL, "invalid regions active memory",		return 0);
		shGuiError(p_gui->region_infos.p_cursor_on_regions				== NULL, "invalid cursor on regions memory",	return 0);
		shGuiError(p_gui->region_infos.menus.p_menu_indices				== NULL, "invalid menu indices memory",			return 0);
		shGuiError(p_gui->region_infos.windows.p_window_indices			== NULL, "invalid menu indices memory",			return 0);
		shGuiError(p_gui->region_infos.windows.p_windows_used_height	== NULL, "invalid menu indices memory",			return 0);

		memset(p_gui->region_infos.p_regions_active, 1, region_count);

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
				sizeof(ShGuiRegionRaw),
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
		shSetupGraphicsPipeline(p_gui->core.device, p_gui->core.render_pass, p_gui->region_infos.fixed_states, &p_gui->region_infos.graphics_pipeline);
	}//GRAPHICS PIPELINE

	return 1;
}

uint8_t shGuiBuildTextPipeline(ShGui* p_gui, uint32_t max_gui_items) {
	shGuiError(
		p_gui == NULL,
		"invalid gui memory",
		return 0
	);

	p_gui->text_infos.max_text_items = max_gui_items;

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
#if SH_GUI_DEBUG_SHADERS
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary("../shaders/bin/shgui-text.vert.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-text.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->core.device, src_size, src, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->text_infos.graphics_pipeline);
		free(src);
#else
		shPipelineCreateShaderModule(p_gui->core.device, sizeof(shgui_text_vert_spv), shgui_text_vert_spv, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderModule(p_gui->core.device, sizeof(shgui_text_frag_spv), shgui_text_frag_spv, &p_gui->text_infos.graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->core.device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->text_infos.graphics_pipeline);
#endif//SH_GUI_DEBUG_SHADERS
	}//SHADER STAGES

	{//DESCRIPTORS
	
		uint32_t text_total_size = shGuiGetAvailableHeap(
			p_gui,
			max_gui_items * SH_GUI_TEXT_MAX_CHAR_COUNT,
			sizeof(ShGuiCharInfo)
		);
		uint32_t char_count = text_total_size / sizeof(ShGuiCharInfo);

		VkPhysicalDeviceProperties physical_device_properties = { 0 };
		vkGetPhysicalDeviceProperties(p_gui->core.physical_device, &physical_device_properties);

		p_gui->text_infos.char_info_map = shVkCreateShGuiCharInfoDescriptorStructures(
			physical_device_properties,
			char_count
		);

		

		shPipelineCreateDynamicDescriptorBuffer(
			p_gui->core.device,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,// | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			sizeof(ShGuiCharInfo),
			char_count,
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
			sizeof(ShGuiCharInfo),
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
		shSetupGraphicsPipeline(p_gui->core.device, p_gui->core.render_pass, p_gui->text_infos.fixed_states, &p_gui->text_infos.graphics_pipeline);
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

	return 1;
}

uint8_t shGuiSetDefaultValues(ShGui* p_gui, ShGuiDefaultValues values, ShGuiInstructions instructions) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	shGuiError(values >= SH_GUI_DEFAULT_VALUES_MAX_ENUM, "invalid gui default values", return 0)

	VkDevice			device			= p_gui->core.device;
	VkCommandBuffer		cmd_buffer		= p_gui->core.cmd_buffer;
	VkFence				fence			= p_gui->core.fence;

	p_gui->default_infos.default_values = values;

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

	switch (values) {
	case SH_GUI_THEME_DARK:
		staging_data[0] = 0.05f;//WINDOWS
		staging_data[1] = 0.05f;
		staging_data[2] = 0.05f;

		staging_data[4] = 1.0f;//TEXT
		staging_data[5] = 1.0f;
		staging_data[6] = 1.0f;
		break;
	case SH_GUI_THEME_EXTRA_DARK:
		staging_data[0] = 0.01f;//WINDOWS
		staging_data[1] = 0.01f;
		staging_data[2] = 0.01f;

		staging_data[4] = 1.0f;//TEXT
		staging_data[5] = 1.0f;
		staging_data[6] = 1.0f;
		break;
	case SH_GUI_THEME_LIGHT:
		staging_data[0] = 0.8f;//WINDOWS
		staging_data[1] = 0.8f;
		staging_data[2] = 0.8f;

		staging_data[4] = 0.0f;//TEXT
		staging_data[5] = 0.0f;
		staging_data[6] = 0.0f;
		break;
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

uint8_t shGuiWriteMemory(ShGui* p_gui, uint8_t record) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice device				= p_gui->core.device;
	VkCommandBuffer cmd_buffer	= p_gui->core.cmd_buffer;

	//WRITE REGIONS DATA
	//
	//
	float null_region[6] = { 0.0f };
	for (uint32_t region_idx = 0; region_idx < p_gui->region_infos.region_count; region_idx++) {
		uint8_t active = p_gui->region_infos.p_regions_active[region_idx];
		ShGuiRegion* p_region = &p_gui->region_infos.p_regions_data[region_idx];
		if (active) {
			shWriteMemory(
				device,
				p_gui->region_infos.staging_memory,
				sizeof(ShGuiRegionRaw) * region_idx,
				sizeof(ShGuiRegionRaw),
				&p_gui->region_infos.p_regions_data[region_idx].raw
			);
		}
		else {
			shWriteMemory(
				device,
				p_gui->region_infos.staging_memory,
				sizeof(ShGuiRegionRaw) * region_idx,
				sizeof(ShGuiRegionRaw),
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

		if (p_gui->region_infos.regions_data_size > 0) {
			shCopyBuffer(
				cmd_buffer,
				p_gui->region_infos.staging_buffer,
				0,
				0,
				p_gui->region_infos.regions_data_size,
				p_gui->region_infos.graphics_pipeline.descriptor_buffers[0]
			);
		}

		if (p_gui->text_infos.vertex_count > 0) {
			shCopyBuffer(
				cmd_buffer,
				p_gui->text_infos.vertex_staging_buffer,
				0,
				0,
				p_gui->text_infos.vertex_count * 4,
				p_gui->text_infos.vertex_buffer
			);
		}


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

uint8_t shGuiRender(ShGui* p_gui) {
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

	//REGION PIPELINE
	//
	//
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

	

	
	//TEXT PIPELINE
	//
	//
	shBindPipeline(
		cmd_buffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		p_text_pipeline
	);

	shPipelinePushConstants(
		cmd_buffer, 
		push_constant_data, 
		p_text_pipeline
	);

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

	uint32_t total_char_idx			= 0;
	uint32_t total_vertex_idx		= 0;
	for (uint32_t text_idx			= 0;	text_idx < p_gui->text_infos.text_count; text_idx++) {//to improve: reduce draw calls :(
		for (uint32_t char_idx		= 0;	char_idx < strlen(p_gui->text_infos.p_text_data[text_idx].text); char_idx++) {
			ShGuiCharInfo* p_char	=		shVkGetShGuiCharInfoDescriptorStructure(p_gui->text_infos.char_info_map, total_char_idx, 0);
			
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

			shBindVertexBuffer(
				cmd_buffer, 
				0, 
				total_vertex_idx * 4, 
				&p_gui->text_infos.vertex_buffer
			);
			total_vertex_idx += p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count;

			shDraw(
				cmd_buffer, 
				p_gui->text_infos.p_text_data[text_idx].chars[char_idx].vertex_count / 3
			);
			total_char_idx++;
		}
	}


	shEndPipeline(p_text_pipeline);

	//RESET TO DEFAULT
	//
	//
	memset(
		p_gui->region_infos.windows.p_windows_used_height,
		0,
		p_gui->region_infos.windows.window_count * 4
	);
	p_gui->region_infos.windows.window_count = 0;

	p_gui->region_infos.region_count = 0;
	p_gui->region_infos.menus.menu_count = 0;

	p_gui->text_infos.text_count = 0;
	p_gui->text_infos.total_char_count = 0;
	p_gui->text_infos.vertex_count = 0;

	return 1;
}

uint8_t shGuiRegion(ShGui* p_gui, float width, float height, float pos_x, float pos_y, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float cursor_x		= *p_gui->inputs.p_cursor_pos_x;
	float cursor_y		= *p_gui->inputs.p_cursor_pos_y;

	float window_size_x = (float)p_gui->region_infos.fixed_states.scissor.extent.width;
	float window_size_y = (float)p_gui->region_infos.fixed_states.scissor.extent.height;

	ShGuiItem item = {
			{
				{
					{
						pos_x, -pos_y
					},
					{
						width, height
					}
				},//raw
				flags
			},//region
			&p_gui->text_infos.p_text_data[p_gui->text_infos.text_count]
	};
	
	if (flags & SH_GUI_PIXELS) {
		//fine
	}

	else if (flags & SH_GUI_RELATIVE) {
		item.region.raw.size[0]		= width / 100.0f * window_size_x;
		item.region.raw.size[1]		= height / 100.0f * window_size_y;
		item.region.raw.position[0]	= pos_x / 100.0f * window_size_x / 2.0f;
		item.region.raw.position[1]	= -pos_y / 100.0f * window_size_y / 2.0f;
	}

	uint32_t		region_count			= p_gui->region_infos.region_count;
	ShGuiRegion*	p_region				= &p_gui->region_infos.p_regions_data[region_count];
	uint8_t			overwritten				= p_gui->region_infos.p_regions_overwritten_data[p_gui->region_infos.region_count];

	if (!overwritten) {
		memcpy(p_region, &item.region, sizeof(ShGuiRegion));
	}

	if (flags & SH_GUI_RESIZABLE) {
		float limit_left					= p_region->raw.position[0] - p_region->raw.size[0] / 2.0f;
		float limit_right					= p_region->raw.position[0] + p_region->raw.size[0] / 2.0f;
		float limit_top						= p_region->raw.position[1] - p_region->raw.size[1] / 2.0f;
		float limit_bottom					= p_region->raw.position[1] + p_region->raw.size[1] / 2.0f;

		uint8_t horizontal_right			=	(cursor_x >= limit_left		- SH_GUI_CURSOR_EDGE_CHECK_SIZE		&& cursor_x			<= limit_left		+ SH_GUI_CURSOR_EDGE_CHECK_SIZE)	&&
												(cursor_y <= limit_bottom	&& cursor_y							>= limit_top																);
		uint8_t horizontal_left				=	(cursor_x <= limit_right	+ SH_GUI_CURSOR_EDGE_CHECK_SIZE		&& cursor_x			>= limit_right		- SH_GUI_CURSOR_EDGE_CHECK_SIZE)	&&
												(cursor_y <= limit_bottom	&& cursor_y							>= limit_top																);
		uint8_t vertical_top				=	(cursor_y >= limit_bottom	- SH_GUI_CURSOR_EDGE_CHECK_SIZE		&& cursor_y			<= limit_bottom		+ SH_GUI_CURSOR_EDGE_CHECK_SIZE)	&&
												(cursor_x >= limit_left		&& cursor_x							<= limit_right																);
		uint8_t vertical_bottom				=	(cursor_y <= limit_top		+ SH_GUI_CURSOR_EDGE_CHECK_SIZE		&& cursor_y			>= limit_top		- SH_GUI_CURSOR_EDGE_CHECK_SIZE)	&&
												(cursor_x >= limit_left		&& cursor_x							<= limit_right																);

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
				p_region->raw.size[0] += 1000.0f * d_cursor_pos_x * ((float)*p_gui->inputs.p_delta_time);
			}
			if (horizontal_right) {
				p_region->raw.size[0] -= 1000.0f * d_cursor_pos_x * ((float)*p_gui->inputs.p_delta_time);
			}
			if (vertical_top) {
				p_region->raw.size[1] += 1000.0f * d_cursor_pos_y * ((float)*p_gui->inputs.p_delta_time);
			}
			if (vertical_bottom) {
				p_region->raw.size[1] -= 1000.0f * d_cursor_pos_y * ((float)*p_gui->inputs.p_delta_time);
			}
		}
	}

	uint8_t* p_clicked = &p_gui->region_infos.p_regions_clicked[p_gui->region_infos.region_count];
	if ((flags & SH_GUI_SWITCH) == 0) {
		(*p_clicked) = 0;
	}

	if (
	(cursor_x >= p_region->raw.position[0] - p_region->raw.size[0] / 2.0f) &&
	(cursor_x <= p_region->raw.position[0] + p_region->raw.size[0] / 2.0f) &&
	(cursor_y >= p_region->raw.position[1] - p_region->raw.size[1] / 2.0f) &&
	(cursor_y <= p_region->raw.position[1] + p_region->raw.size[1] / 2.0f)
	) {
		p_gui->inputs.active_cursor_icon = p_gui->inputs.p_cursor_icons[SH_GUI_CURSOR_NORMAL];

		p_gui->region_infos.p_cursor_on_regions[region_count] = 1;

		if (p_gui->inputs.p_mouse_events[1] == 1 && (flags & SH_GUI_MOVABLE)) {
			
			float dx = cursor_x - p_gui->inputs.last.last_cursor_pos_x;
			float dy = cursor_y - p_gui->inputs.last.last_cursor_pos_y;

			p_region->raw.position[0]		+= dx;
			p_region->raw.position[1]		+= dy;
		
			p_gui->region_infos.p_regions_overwritten_data[region_count] = 1;
		}
		if (p_gui->inputs.p_mouse_events[0] == 1 && p_gui->inputs.last.last_mouse_events[0] == 0) {
			uint8_t rtrn = (*p_clicked) == 0;
			(*p_clicked) = 1;
			p_gui->region_infos.region_count++;
			return rtrn;
		}
	}
	else {
		p_gui->region_infos.p_cursor_on_regions[region_count] = 0;
	}

	(*p_clicked) = 0;
	p_gui->region_infos.region_count++;

	return 0;
}

uint8_t shGuiRegionWrite(ShGui* p_gui, uint32_t region_idx, float width, float height, float pos_x, float pos_y, char* name, ShGuiWidgetFlags flags, ShGuiWriteFlags write_flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	if (write_flags & SH_GUI_WIDTH) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size[0] = width;
	}
	if (write_flags & SH_GUI_HEIGHT) {
		p_gui->region_infos.p_regions_data[region_idx].raw.size[1] = height;
	}
	if (write_flags & SH_GUI_POSITION_X) {
		p_gui->region_infos.p_regions_data[region_idx].raw.position[0] = pos_x;
	}
	if (write_flags & SH_GUI_POSITION_Y) {
		p_gui->region_infos.p_regions_data[region_idx].raw.position[1] = pos_y;
	}

	p_gui->region_infos.p_regions_overwritten_data[region_idx] = 1;

	return 1;
}

uint8_t shGuiSetRegionPriority(ShGui* p_gui, uint32_t region_idx, float priority) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	ShGuiRegion* p_region = &p_gui->region_infos.p_regions_data[region_idx];
	p_region->raw.priority[0] = priority;

	return 1;
}

uint8_t shGuiItem(ShGui* p_gui, float width, float height, float pos_x, float pos_y, char* name, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t		region_count	= p_gui->region_infos.region_count;
	ShGuiRegion*	p_region		= &p_gui->region_infos.p_regions_data[region_count];

	uint8_t sig						= shGuiRegion(p_gui, width, height, pos_x, pos_y, flags);

	if (name != NULL) {
		shGuiText(
			p_gui,
			SH_GUI_WINDOW_TEXT_SIZE,
			p_region->raw.position[0] - p_region->raw.size[0] / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET,
			-p_region->raw.position[1] - SH_GUI_WINDOW_TEXT_BORDER_OFFSET,
			name
		);
	}

	return sig;
}

uint8_t shGuiWindow(ShGui* p_gui, float width, float height, float pos_x, float pos_y, char* title, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	uint32_t main_region_idx		= p_gui->region_infos.region_count;
	ShGuiRegion* main_region		= &p_gui->region_infos.p_regions_data[main_region_idx];

	p_gui->region_infos.windows.p_window_indices[p_gui->region_infos.windows.window_count] = p_gui->region_infos.region_count;

	uint8_t sig						= shGuiRegion(p_gui, width, height, pos_x, pos_y, flags);
	shGuiSetRegionPriority(p_gui, main_region_idx, SH_GUI_EMPTY_REGION_PRIORITY);

	float* main_position		= main_region->raw.position;
	float* main_size			= main_region->raw.size;

	uint32_t bar_region_idx = p_gui->region_infos.region_count;

	shGuiItem(
		p_gui, 
		main_size[0],
		SH_GUI_WINDOW_BAR_SIZE, 
		main_position[0],
		-main_position[1] + main_size[1] / 2.0f - SH_GUI_WINDOW_BAR_SIZE / 2.0f,
		title,
		SH_GUI_PIXELS
	);
	shGuiSetRegionPriority(p_gui, bar_region_idx, SH_GUI_ITEMS_PRIORITY);

	p_gui->region_infos.windows.window_count++;

	return sig;
}

uint8_t shGuiWindowText(ShGui* p_gui, float scale, char* text, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t window_count			= p_gui->region_infos.windows.window_count;
	uint32_t window_idx				= p_gui->region_infos.windows.p_window_indices[window_count - 1];
	uint32_t last_region			= p_gui->region_infos.region_count - 1;

	ShGuiRegion window_region		= p_gui->region_infos.p_regions_data[window_idx];
	float* p_window_position		= window_region.raw.position;
	float* p_window_size			= window_region.raw.size;

	float window_pos_x				= p_window_position[0];
	float window_pos_y				= -p_window_position[1];
	float window_size_x				= p_window_size[0];
	float window_size_y				= p_window_size[1];

	float* p_used_height			= &p_gui->region_infos.windows.p_windows_used_height[window_count - 1];

	float text_pos_x				= window_pos_x - window_size_x / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET;
	float text_pos_y				= window_pos_y + window_size_y / 2.0f - SH_GUI_WINDOW_BAR_SIZE - SH_GUI_WINDOW_TEXT_BORDER_OFFSET - (*p_used_height) - scale;

	if (flags & SH_GUI_CENTER_WIDTH) {
		float chars_offset			= text != NULL 
									? strlen(text) * SH_GUI_CHAR_DISTANCE_OFFSET * scale / 4.0f 
									: 0.0f;
		text_pos_x					= window_pos_x - chars_offset / 2.0f;
	}

	(*p_used_height)				+= scale + SH_GUI_WINDOW_ITEMS_OFFSET;

	shGuiText(
		p_gui,
		scale,
		text_pos_x,
		text_pos_y,
		text
	);

	return 1;
}

uint8_t shGuiWindowButton(ShGui* p_gui, float scale, char* text, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memmory", return 0);

	uint32_t window_count			= p_gui->region_infos.windows.window_count;
	uint32_t window_idx				= p_gui->region_infos.windows.p_window_indices[window_count - 1];
	uint32_t last_region			= p_gui->region_infos.region_count - 1;

	ShGuiRegion window_region		= p_gui->region_infos.p_regions_data[window_idx];
	float* p_window_position		= window_region.raw.position;
	float* p_window_size			= window_region.raw.size;

	float window_pos_x				= p_window_position[0];
	float window_pos_y				= -p_window_position[1];
	float window_size_x				= p_window_size[0];
	float window_size_y				= p_window_size[1];

	float* p_used_height			= &p_gui->region_infos.windows.p_windows_used_height[window_count - 1];

	float width						= text != NULL ? SH_GUI_CHAR_FINAL_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, scale, strlen(text)) + SH_GUI_WINDOW_TEXT_BORDER_OFFSET : 50.0f;
	float height					= scale + SH_GUI_WINDOW_TEXT_BORDER_OFFSET;

	float item_pos_x				= window_pos_x - window_size_x / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET + width / 2.0f;
	float item_pos_y				= SH_GUI_WINDOW_USED_HEIGHT(window_pos_y, window_size_y, *p_used_height) - height / 2.0f;

	if (flags & SH_GUI_CENTER_WIDTH) {
		item_pos_x					= window_pos_x;
	}
	(*p_used_height)				+= scale + SH_GUI_WINDOW_ITEMS_OFFSET;

	uint32_t button_region_idx = p_gui->region_infos.region_count;
	uint8_t pressed = shGuiItem(
		p_gui,
		width,
		scale,
		item_pos_x,
		item_pos_y,
		text,
		SH_GUI_PIXELS
	);
	shGuiSetRegionPriority(p_gui, button_region_idx, SH_GUI_ITEMS_PRIORITY);

	return pressed;
}

uint8_t shGuiWindowSeparator(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t window_count			= p_gui->region_infos.windows.window_count;
	uint32_t window_idx				= p_gui->region_infos.windows.p_window_indices[window_count - 1];
	uint32_t last_region			= p_gui->region_infos.region_count - 1;

	ShGuiRegion window_region		= p_gui->region_infos.p_regions_data[window_idx];
	float* p_window_position		= window_region.raw.position;
	float* p_window_size			= window_region.raw.size;

	float window_pos_x				= p_window_position[0];
	float window_pos_y				= -p_window_position[1];
	float window_size_x				= p_window_size[0];
	float window_size_y				= p_window_size[1];

	float* p_used_height = &p_gui->region_infos.windows.p_windows_used_height[window_count - 1];

	float item_pos_x = window_pos_x;
	float item_pos_y = SH_GUI_WINDOW_USED_HEIGHT(window_pos_y, window_size_y, *p_used_height) - SH_GUI_SEPARATOR_OFFSET;
	float item_height = 2.0f;

	(*p_used_height) += item_height + SH_GUI_WINDOW_ITEMS_OFFSET;

	uint32_t separator_region_idx = p_gui->region_infos.region_count;
	shGuiRegion(
		p_gui,
		window_size_x / 1.2f,
		item_height,
		item_pos_x,
		item_pos_y,
		SH_GUI_PIXELS
	);
	shGuiSetRegionPriority(p_gui, separator_region_idx, SH_GUI_ITEMS_PRIORITY);

	return 1;
}

uint8_t shGuiMenuBar(ShGui* p_gui, float extent, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float region_width = 100.0f;
	float region_height = extent;
	float region_pos_x = 0.0f;
	float region_pos_y = 100.0f;

	if (flags & SH_GUI_RELATIVE) {
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
	}
	else {
		float full_window_width = (float)p_gui->region_infos.fixed_states.scissor.extent.width;
		float full_window_height = (float)p_gui->region_infos.fixed_states.scissor.extent.height;

		if (flags & SH_GUI_TOP || flags & SH_GUI_BOTTOM) {
			region_width = full_window_width;
			region_height = extent;
			region_pos_x = 0.0f;
			region_pos_y = (full_window_height - extent) / 2.0f;
			if (flags & SH_GUI_BOTTOM) {
				region_pos_y = (-full_window_height + extent) / 2.0f;
			}
		}
		if (flags & SH_GUI_LEFT || flags & SH_GUI_RIGHT) {
			region_width = extent;
			region_height = full_window_height;
			region_pos_x = (-full_window_width + extent) / 2.0f;
			region_pos_y = 0.0f;
			if (flags & SH_GUI_RIGHT) {
				region_pos_x = (full_window_width - extent) / 2.0f;
			}
		}
		flags |= SH_GUI_PIXELS;
	}

	p_gui->region_infos.menus.p_menu_indices[p_gui->region_infos.menus.menu_count] = p_gui->region_infos.region_count;


	uint32_t bar_region_idx = p_gui->region_infos.region_count;
	shGuiRegion(
		p_gui, region_width, region_height, region_pos_x, region_pos_y, flags
	);
	shGuiSetRegionPriority(p_gui, bar_region_idx, SH_GUI_EMPTY_REGION_PRIORITY);

	p_gui->region_infos.menus.menu_count++;

	return 1;
}

uint8_t shGuiMenuItem(ShGui* p_gui, float extent, char* title, ShGuiWidgetFlags flags) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float width							= 10.0f;//%
	ShGuiWidgetFlags additional_flags	= SH_GUI_RELATIVE;
	if (title != NULL) {
		additional_flags				&= ~SH_GUI_RELATIVE;
		additional_flags				|= SH_GUI_PIXELS;
		width							= (strlen(title) + 1.0f) * SH_GUI_CHAR_DISTANCE_OFFSET * SH_GUI_WINDOW_TEXT_SIZE / 4.0f;//in pixels
	}
	
	uint32_t bar_count					= p_gui->region_infos.menus.menu_count - 1;
	uint32_t bar_idx					= p_gui->region_infos.menus.p_menu_indices[bar_count];
	ShGuiRegion* bar					= &p_gui->region_infos.p_regions_data[bar_idx];
	
	uint32_t last_region_idx			= p_gui->region_infos.region_count - 1;

	float position[2]					= { 0.0f };
	float size[2]						= { 0.0f };
	
	if (bar->flags & SH_GUI_TOP || bar->flags & SH_GUI_BOTTOM) {

		float bar_corner_left			= -bar->raw.size[0] / 2.0f;
		float last_region_corner_left	= p_gui->region_infos.p_regions_data[last_region_idx].raw.position[0] - p_gui->region_infos.p_regions_data[last_region_idx].raw.size[0] / 2.0f;
		float last_region_corner_right	= p_gui->region_infos.p_regions_data[last_region_idx].raw.position[0] + p_gui->region_infos.p_regions_data[last_region_idx].raw.size[0] / 2.0f;
		size[0]							= width;
		size[1]							= bar->raw.size[1];
		if (last_region_idx				== bar_idx) {
			position[0]					= bar_corner_left + size[0] / 2.0f;
		}
		else {
			position[0]					= last_region_corner_right + size[0] / 2.0f;
		}
		position[1]						= bar->raw.position[1];
	}
	else if (bar->flags & SH_GUI_LEFT || bar->flags && SH_GUI_RIGHT) {
		size[0]				= width;
		size[1]				= SH_GUI_WINDOW_TEXT_SIZE;
		position[0]				= bar->raw.position[0];
		position[1]				= (-bar->raw.size[1] + size[1]) / 2.0f;
	}
	else {
		return 0;
	}

	uint32_t item_region_idx = p_gui->region_infos.region_count;
	uint8_t pressed = shGuiItem(
		p_gui, 
		size[0], 
		size[1], 
		position[0], 
		-position[1], 
		title, 
		flags | additional_flags
	);
	shGuiSetRegionPriority(p_gui, item_region_idx, SH_GUI_ITEMS_PRIORITY);
	return pressed;
}

#define SH_GUI_LOAD_CHAR(font, char_name, _char)\
	_char.vertex_count = sizeof(font ## _ ## char_name ## _vertices) / 4;\
	_char.p_vertices = (float*)(font ## _ ## char_name ## _vertices);\

uint8_t shGuiText(ShGui* p_gui, float scale, float pos_x, float pos_y, char* s_text) {
	shGuiError(p_gui					== NULL, "invalid gui memory", return 0);
	shGuiError(s_text					== NULL, "invalid text memory", return 0);

	ShGuiText* p_text					= &p_gui->text_infos.p_text_data[p_gui->text_infos.text_count];
	strcpy(p_text->text,				s_text);

	float window_size_x					= (float)p_gui->region_infos.fixed_states.scissor.extent.width;
	float window_size_y					= (float)p_gui->region_infos.fixed_states.scissor.extent.height;

	for (uint32_t char_idx				= 0; char_idx < strlen(s_text); char_idx++) {
		ShGuiCharInfo* p_char_info		= shVkGetShGuiCharInfoDescriptorStructure(p_gui->text_infos.char_info_map, p_gui->text_infos.total_char_count, 0);
		
		p_char_info->position[0]	= pos_x + SH_GUI_CHAR_FINAL_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, scale, char_idx);
		p_char_info->position[1]	= -pos_y;
		p_char_info->scale[0]		= scale;
		p_char_info->priority[0]	= SH_GUI_TEXT_PRIORITY;
		
		p_gui->text_infos.total_char_count++;

		switch (s_text[char_idx]) {
		case 'q':
		case 'Q':
			SH_GUI_LOAD_CHAR(consolas, bigQ, p_text->chars[char_idx]); 
			break;
		case 'w':
		case 'W':
			SH_GUI_LOAD_CHAR(consolas, bigW, p_text->chars[char_idx]); 
			break;
		case 'e':
		case 'E':
			SH_GUI_LOAD_CHAR(consolas, bigE, p_text->chars[char_idx]); 
			break;
		case 'r':
		case 'R':
			SH_GUI_LOAD_CHAR(consolas, bigR, p_text->chars[char_idx]);
			break;
		case 't':
		case 'T':
			SH_GUI_LOAD_CHAR(consolas, bigT, p_text->chars[char_idx]);
			break;
		case 'y':
		case 'Y':
			SH_GUI_LOAD_CHAR(consolas, bigY, p_text->chars[char_idx]);
			break;
		case 'u':
		case 'U':
			SH_GUI_LOAD_CHAR(consolas, bigU, p_text->chars[char_idx]);
			break;
		case 'i':
		case 'I':
			SH_GUI_LOAD_CHAR(consolas, bigI, p_text->chars[char_idx]);
			break;
		case 'o':
		case 'O':
			SH_GUI_LOAD_CHAR(consolas, bigO, p_text->chars[char_idx]);
			break;
		case 'p':
		case 'P':
			SH_GUI_LOAD_CHAR(consolas, bigP, p_text->chars[char_idx]);
			break;
		case 'a':
		case 'A':
			SH_GUI_LOAD_CHAR(consolas, bigA, p_text->chars[char_idx]);
			break;
		case 's':
		case 'S':
			SH_GUI_LOAD_CHAR(consolas, bigS, p_text->chars[char_idx]);
			break;
		case 'd':
		case 'D':
			SH_GUI_LOAD_CHAR(consolas, bigD, p_text->chars[char_idx]);
			break;
		case 'f':
		case 'F':
			SH_GUI_LOAD_CHAR(consolas, bigF, p_text->chars[char_idx]);
			break;
		case 'g':
		case 'G':
			SH_GUI_LOAD_CHAR(consolas, bigG, p_text->chars[char_idx]);
			break;
		case 'h':
		case 'H':
			SH_GUI_LOAD_CHAR(consolas, bigH, p_text->chars[char_idx]);
			break;
		case 'j':
		case 'J':
			SH_GUI_LOAD_CHAR(consolas, bigJ, p_text->chars[char_idx]);
			break;
		case 'k':
		case 'K':
			SH_GUI_LOAD_CHAR(consolas, bigK, p_text->chars[char_idx]);
			break;
		case 'l':
		case 'L':
			SH_GUI_LOAD_CHAR(consolas, bigL, p_text->chars[char_idx]);
			break;
		case 'z':
		case 'Z':
			SH_GUI_LOAD_CHAR(consolas, bigZ, p_text->chars[char_idx]);
			break;
		case 'x':
		case 'X':
			SH_GUI_LOAD_CHAR(consolas, bigX, p_text->chars[char_idx]);
			break;
		case 'c':
		case 'C':
			SH_GUI_LOAD_CHAR(consolas, bigC, p_text->chars[char_idx]);
			break;
		case 'v':
		case 'V':
			SH_GUI_LOAD_CHAR(consolas, bigV, p_text->chars[char_idx]);
			break;
		case 'b':
		case 'B':
			SH_GUI_LOAD_CHAR(consolas, bigB, p_text->chars[char_idx]);
			break;
		case 'n':
		case 'N':
			SH_GUI_LOAD_CHAR(consolas, bigN, p_text->chars[char_idx]);
			break;
		case 'm':
		case 'M':
			SH_GUI_LOAD_CHAR(consolas, bigM, p_text->chars[char_idx]);
			break;
		case '\\':
			SH_GUI_LOAD_CHAR(consolas, backslash, p_text->chars[char_idx]);
			break;
		case '1':
			SH_GUI_LOAD_CHAR(consolas, 1, p_text->chars[char_idx]);
			break;
		case '2':
			SH_GUI_LOAD_CHAR(consolas, 2, p_text->chars[char_idx]);
			break;
		case '3':
			SH_GUI_LOAD_CHAR(consolas, 3, p_text->chars[char_idx]);
			break;
		case '4':
			SH_GUI_LOAD_CHAR(consolas, 4, p_text->chars[char_idx]);
			break;
		case '5':
			SH_GUI_LOAD_CHAR(consolas, 5, p_text->chars[char_idx]);
			break;
		case '6':
			SH_GUI_LOAD_CHAR(consolas, 6, p_text->chars[char_idx]);
			break;
		case '7':
			SH_GUI_LOAD_CHAR(consolas, 7, p_text->chars[char_idx]);
			break;
		case '8':
			SH_GUI_LOAD_CHAR(consolas, 8, p_text->chars[char_idx]);
			break;
		case '9':
			SH_GUI_LOAD_CHAR(consolas, 9, p_text->chars[char_idx]);
			break;
		case '0':
			SH_GUI_LOAD_CHAR(consolas, 0, p_text->chars[char_idx]);
			break;
		case '\'':
			SH_GUI_LOAD_CHAR(consolas, quote, p_text->chars[char_idx]); 
				break;
		case '|':
			SH_GUI_LOAD_CHAR(consolas, stick, p_text->chars[char_idx]);
			break;
		case '!':
			SH_GUI_LOAD_CHAR(consolas, exclamation, p_text->chars[char_idx]);
			break;
		case '"':
			SH_GUI_LOAD_CHAR(consolas, double_quote, p_text->chars[char_idx]); 
			break;
		case '£':
			SH_GUI_LOAD_CHAR(consolas, pound, p_text->chars[char_idx]);
			break;
		case '$':
			SH_GUI_LOAD_CHAR(consolas, dollar, p_text->chars[char_idx]);
			break;
		case '%':
			SH_GUI_LOAD_CHAR(consolas, percent, p_text->chars[char_idx]);
			break;
		case '&':
			SH_GUI_LOAD_CHAR(consolas, and, p_text->chars[char_idx]);
			break;
		case '/':
			SH_GUI_LOAD_CHAR(consolas, frontslash, p_text->chars[char_idx]);
			break;
		//case '(':
		//	SH_GUI_LOAD_CHAR(consolas, open_round_bracket, p_text->chars[char_idx]);
		//	break;
		//case ')':
		//	SH_GUI_LOAD_CHAR(consolas, close_round_bracket, p_text->chars[char_idx]);
		//	break;
		case '=':
			SH_GUI_LOAD_CHAR(consolas, equal, p_text->chars[char_idx]);
			break;
		case '?':
			SH_GUI_LOAD_CHAR(consolas, question_mark, p_text->chars[char_idx]);
			break;
		case '+':
			SH_GUI_LOAD_CHAR(consolas, plus, p_text->chars[char_idx]);
			break;
		case ',':
			SH_GUI_LOAD_CHAR(consolas, comma, p_text->chars[char_idx]);
			break;
		case '.':
			SH_GUI_LOAD_CHAR(consolas, dot, p_text->chars[char_idx]);
			break;
		case '-':
			SH_GUI_LOAD_CHAR(consolas, dash, p_text->chars[char_idx]);
			break;
		case '*':
			SH_GUI_LOAD_CHAR(consolas, star, p_text->chars[char_idx]);
			break;
		case ';':
			SH_GUI_LOAD_CHAR(consolas, semicolon, p_text->chars[char_idx]);
			break;
		case ':':
			SH_GUI_LOAD_CHAR(consolas, colon, p_text->chars[char_idx]);
			break;
		case '_':
			SH_GUI_LOAD_CHAR(consolas, underscore, p_text->chars[char_idx]);
			break;
		//case '[':
		//	SH_GUI_LOAD_CHAR(consolas, open_round_bracket, p_text->chars[char_idx]);
		//	break;
		//case ']':
		//	SH_GUI_LOAD_CHAR(consolas, close_round_bracket, p_text->chars[char_idx]);
		//	break;
		case '@':
			SH_GUI_LOAD_CHAR(consolas, at, p_text->chars[char_idx]);
			break;
		case '#':
			SH_GUI_LOAD_CHAR(consolas, hash, p_text->chars[char_idx]);
			break;
		//case '{':
		//	SH_GUI_LOAD_CHAR(consolas, open_curly_bracket, p_text->chars[char_idx]);
		//	break;
		//case '}':
		//	SH_GUI_LOAD_CHAR(consolas, close_curly_bracket, p_text->chars[char_idx]);
		//	break;
		default:
			p_text->chars[char_idx].vertex_count = 1;
			p_text->chars[char_idx].p_vertices = SH_GUI_EMPTY_CHAR;
			break;
		}
	}

	p_gui->text_infos.text_count++;

	return 1;
}


uint8_t shGuiDestroyPipelines(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	shFixedStatesRelease(&p_gui->region_infos.fixed_states);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 0, &p_gui->region_infos.graphics_pipeline);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 1, &p_gui->region_infos.graphics_pipeline);
	shPipelineRelease(p_gui->core.device, &p_gui->region_infos.graphics_pipeline);
	
	shClearBufferMemory(p_gui->core.device, p_gui->region_infos.staging_buffer, p_gui->region_infos.staging_memory);

	shFixedStatesRelease(&p_gui->text_infos.fixed_states);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 0, &p_gui->text_infos.graphics_pipeline);
	shPipelineClearDescriptorBufferMemory(p_gui->core.device, 1, &p_gui->text_infos.graphics_pipeline);
	shPipelineRelease(p_gui->core.device, &p_gui->text_infos.graphics_pipeline);

	shClearBufferMemory(p_gui->core.device, p_gui->text_infos.vertex_staging_buffer, p_gui->text_infos.vertex_staging_memory);
	shClearBufferMemory(p_gui->core.device, p_gui->text_infos.vertex_buffer, p_gui->text_infos.vertex_memory);

	return 1;
}

uint8_t shGuiRelease(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	shClearBufferMemory(p_gui->core.device, p_gui->default_infos.staging_buffer, p_gui->default_infos.staging_memory);
	
	shGuiDestroyPipelines(p_gui);

	{
		ShGuiRegion* p_regions_data				= p_gui->region_infos.p_regions_data;
		uint8_t* p_regions_overwritten_data		= p_gui->region_infos.p_regions_overwritten_data;
		uint8_t* p_regions_clicked				= p_gui->region_infos.p_regions_clicked;
		uint8_t* p_regions_active				= p_gui->region_infos.p_regions_active;
		uint32_t* p_menu_indices				= p_gui->region_infos.menus.p_menu_indices;
		uint32_t* p_window_indices				= p_gui->region_infos.windows.p_window_indices;
		float* p_windows_used_height			= p_gui->region_infos.windows.p_windows_used_height;
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
		if (p_menu_indices != NULL) {
			free(p_menu_indices);
		}
		if (p_window_indices != NULL) {
			free(p_window_indices);
		}
		if (p_windows_used_height != NULL) {
			free(p_windows_used_height);
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