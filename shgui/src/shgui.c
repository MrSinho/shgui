#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "shgui/shgui.h"

#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkMemoryInfo.h>
	

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



uint8_t SH_GUI_CALL shGuiBuildPipeline(ShGui* p_gui, VkRenderPass render_pass, const uint32_t max_gui_items) {
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
		//shSetPushConstants(
		//	VK_SHADER_STAGE_VERTEX_BIT, 
		//	0, 
		//	128, 
		//	//
		//);
	}//PUSH CONSTANT


	{//FIXED STATES
		shSetFixedStates(
			p_gui->device, 
			surface_capabilities.currentExtent.width, 
			surface_capabilities.currentExtent.height, 
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			VK_POLYGON_MODE_FILL, 
			&p_gui->fixed_states
		);
	}//FIXED STATES

	{//SHADER STAGES
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary("../shaders/bin/shgui-item.vert.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->graphics_pipeline);
		free(src);
		src = (char*)shGuiReadBinary("../shaders/bin/shgui-item.frag.spv", &src_size);
		shPipelineCreateShaderModule(p_gui->device, src_size, src, &p_gui->graphics_pipeline);
		shPipelineCreateShaderStage(p_gui->device, VK_SHADER_STAGE_FRAGMENT_BIT, &p_gui->graphics_pipeline);
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
			uint32_t max_items_size = max_gui_items * sizeof(ShGuiItem);
			p_gui->items_data_size = available_gpu_heap >= max_items_size ? max_items_size : available_gpu_heap;
		}
		
		shPipelineCreateDescriptorBuffer(
			p_gui->device, 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			0, 
			p_gui->items_data_size, 
			&p_gui->graphics_pipeline
		);
		shPipelineAllocateDescriptorBufferMemory(
			p_gui->device,
			p_gui->physical_device,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			&p_gui->graphics_pipeline
		);
		shPipelineBindDescriptorBufferMemory(
			p_gui->device,
			0,
			0,
			&p_gui->graphics_pipeline
		);

		shPipelineDescriptorSetLayout(p_gui->device, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, &p_gui->graphics_pipeline);
		shPipelineCreateDescriptorPool(p_gui->device, 0, &p_gui->graphics_pipeline);
		shPipelineAllocateDescriptorSet(p_gui->device, 0, &p_gui->graphics_pipeline);

		{
			{
				shCreateBuffer(
					p_gui->device,
					p_gui->items_data_size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					&p_gui->staging_buffer
				);
				shAllocateMemory(
					p_gui->device,
					p_gui->physical_device,
					p_gui->staging_buffer,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&p_gui->staging_buffer_memory
				);
				shBindMemory(
					p_gui->device,
					p_gui->staging_buffer,
					0,
					p_gui->staging_buffer_memory
				);
			}
		}
	}
	//DESCRIPTORS

	{//GRAPHICS PIPELINE
		shSetupGraphicsPipeline(p_gui->device, render_pass, p_gui->fixed_states, &p_gui->graphics_pipeline);
	}//GRAPHICS PIPELINE

	return 1;
}

uint8_t SH_GUI_CALL shGuiWriteMemory(ShGui* p_gui, const uint8_t record) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	p_gui->p_items_data = calloc(1, p_gui->items_data_size);

	shWriteMemory(
		p_gui->device,
		p_gui->staging_buffer_memory,
		0,
		p_gui->items_data_size,
		p_gui->p_items_data
	);

	{
		if (record) {
			shWaitForFence(p_gui->device, &p_gui->fence);
			shResetFence(p_gui->device, &p_gui->fence);
			shBeginCommandBuffer(p_gui->cmd_buffer);
		}

		shCopyBuffer(p_gui->cmd_buffer, p_gui->staging_buffer, 0, 0, p_gui->items_data_size, p_gui->graphics_pipeline.descriptor_buffers[0]);
		
		if (record) {
			shEndCommandBuffer(p_gui->cmd_buffer);
			shQueueSubmit(1, &p_gui->cmd_buffer, p_gui->graphics_queue.queue, p_gui->fence);
			shWaitForFence(p_gui->device, &p_gui->fence);
		}
	}

	return 1;
}

uint8_t SH_GUI_CALL shGuiRender(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	shBindPipeline(p_gui->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, &p_gui->graphics_pipeline);

	shPipelineBindDescriptorSets(
		p_gui->cmd_buffer, 
		0, 
		p_gui->graphics_pipeline.descriptor_count, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		&p_gui->graphics_pipeline
	);

	shEndPipeline(&p_gui->graphics_pipeline);

	p_gui->item_count = 0;

	return 1;
}

uint8_t SH_GUI_CALL shGuiWindow(ShGui* p_gui, const float width, const float height, const float pos_x, const float pos_y, const char* name) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0)

	ShGuiItem item = {
		{ width, height, pos_x, pos_y }
	};

	memcpy(&((char*)p_gui->p_items_data)[p_gui->item_count * sizeof(ShGuiItem)], &item, sizeof(ShGuiItem));

	p_gui->item_count++;

	return 1;
}

uint8_t SH_GUI_CALL shGuiRelease(ShGui* p_gui) {
	shClearBufferMemory(p_gui->device, p_gui->staging_buffer, p_gui->staging_buffer_memory);
	shPipelineClearDescriptorBufferMemory(p_gui->device, 0, &p_gui->graphics_pipeline);
	shPipelineRelease(p_gui->device, &p_gui->graphics_pipeline);
	return 1;
}



#ifdef __cplusplus
}
#endif//__cplusplus