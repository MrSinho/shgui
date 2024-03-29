#ifdef __cplusplus
"C" {
#endif//__cplusplus


#include "shgui/shgui.h"
#include "shgui/shgui-font.h"

#include <shvulkan/shVulkan.h>
#include <string.h>



#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif//_MSC_VER



uint8_t shGuiInit(
	ShGui*    p_gui,
	ShGuiCore core
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	p_gui->core = core;
	
	return 1;
}

uint8_t shGuiSetSurface(
	ShGui*       p_gui,
	VkSurfaceKHR surface
) {
	shGuiError(p_gui   == NULL,           "invalid gui memory",     return 0);
	shGuiError(surface == VK_NULL_HANDLE, "invalid surface memory", return 0);

	p_gui->core.surface = surface;

	return 1;
}

uint8_t shGuiSetRenderpass(
	ShGui*       p_gui,
	VkRenderPass renderpass
) {
	shGuiError(p_gui      == NULL,           "invalid gui memory",     return 0);
	shGuiError(renderpass == VK_NULL_HANDLE, "invalid surface memory", return 0);
		
	p_gui->core.render_pass = renderpass;

	return 1;
}

uint32_t shGuiGetAvailableHeap(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice         device          = p_gui->core.device;
	VkPhysicalDevice physical_device = p_gui->core.physical_device;

	shGuiError(device          == VK_NULL_HANDLE, "invalid device memory",          return 0);
	shGuiError(physical_device == VK_NULL_HANDLE, "invalid physical device memory", return 0);

	uint32_t                                  host_memory_type_index      =   0;
	uint32_t                                  host_available_video_memory =   0;
	VkPhysicalDeviceMemoryBudgetPropertiesEXT host_memory_budget          = { 0 };

	uint8_t r = 1;

	r = r && shGetMemoryType(
		device, physical_device,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&host_memory_type_index
	);

	r = r && shGetMemoryBudgetProperties(
		physical_device, &host_memory_budget
	);
	
	shGuiError(r == 0, "failed reading host memory budget properties", return 0);

	uint32_t                                  device_memory_type_index      =   0;
	uint32_t                                  device_available_video_memory =   0;
	VkPhysicalDeviceMemoryBudgetPropertiesEXT device_memory_budget          = { 0 };

	r = r && shGetMemoryType(
		device, physical_device,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&device_memory_type_index
	);

	r = r && shGetMemoryBudgetProperties(
		physical_device, &device_memory_budget
	);

	shGuiError(r == 0, "failed reading host memory budget properties", return 0);

	host_available_video_memory   = (uint32_t)host_memory_budget.heapBudget  [host_memory_type_index];
	device_available_video_memory = (uint32_t)device_memory_budget.heapBudget[device_memory_type_index];

	uint32_t available_heap          = 0;
	uint32_t max_structures_size     = 0;

	available_heap = host_available_video_memory <= device_available_video_memory ? host_available_video_memory : device_available_video_memory;

	return available_heap;
}

uint8_t shGuiAllocateMemory(
	ShGui* p_gui
) {
	shGuiError(p_gui            == NULL, "invalid gui memory",   return 0);
	
	VkDevice device                  = p_gui->core.device;
	VkPhysicalDevice physical_device = p_gui->core.physical_device;


	shGuiError(device          == VK_NULL_HANDLE, "invalid device memory",          return 0);
	shGuiError(physical_device == VK_NULL_HANDLE, "invalid physical device memory", return 0);


	uint8_t r = 1;

	r = r && shCreateBuffer(
		device, SH_GUI_STAGING_BUFFER_SIZE,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		&p_gui->staging_buffer
	);

	r = r && shAllocateBufferMemory(
		device, physical_device,
		p_gui->staging_buffer, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&p_gui->staging_memory
	);

	r = r && shBindBufferMemory(device, p_gui->staging_buffer, 0, p_gui->staging_memory);



	r = r && shCreateBuffer(
		device, SH_GUI_DST_BUFFER_SIZE,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		&p_gui->dst_buffer
	);

	r = r && shAllocateBufferMemory(
		device, physical_device,
		p_gui->dst_buffer,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&p_gui->dst_memory
	);

	r = r && shBindBufferMemory(device, p_gui->dst_buffer, 0, p_gui->dst_memory);

	shGuiError(r == 0, "failed allocating memory", return 0);

	return 1;
}

char* shGuiReadBinary(
	const char* path, 
	uint32_t*   p_code_size
) {
	shGuiError(path == NULL, "invalid file path", return 0);

	FILE*    stream    = NULL;
	char*    code      = NULL;
	uint32_t code_size = 0;

	stream = fopen(path, "rb");

	shGuiError(stream == NULL, "invalid file stream memory", return NULL);

	fseek(stream, 0, SEEK_END);
	code_size = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	shGuiError(code_size == 0, "file is empty", return NULL);

	code = (char*)malloc(code_size);
	shGuiError(code == NULL, "invalid code memory", return NULL);

	fread(code, code_size, 1, stream);
	
	(p_code_size != NULL) && (*p_code_size = code_size);

	fclose(stream);

	return code;
}

uint8_t shGuiAllocatePipelineResources(
	ShGui*   p_gui,
	uint32_t framebuffer_count
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice          device          = p_gui->core.device;
	ShVkPipelinePool* p_pipeline_pool = &p_gui->pipeline_pool;

	ShGuiRegionInfos* p_region_infos  = &p_gui->region_infos;
	ShGuiCharInfos*   p_char_infos    = &p_gui->char_infos;

	uint8_t r = 1;


	r = r && shPipelinePoolCreateDescriptorSetLayoutBinding(
		0,                                 //binding
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, //descriptor_type
		1,                                 //descriptor_set_count
		VK_SHADER_STAGE_VERTEX_BIT,        //shader_stage
		p_pipeline_pool                    //p_pipeline_pool
	);

	r = r && shPipelinePoolCreateDescriptorSetLayout(
		device,         //device
		0,              //first_binding_idx
		1,              //binding_count
		0,              //set_layout_idx
		p_pipeline_pool //p_pipeline_pool
	);

	r = r && shPipelinePoolCopyDescriptorSetLayout(
		0,                     //src_set_layout_idx
		0,                     //first_dst_set_layout_idx
		framebuffer_count * 2, //dst_set_layout_count       /*me from the future: times 2 because it's shared with */
		p_pipeline_pool        //p_pipeline_pool
	);

	r = r && shPipelinePoolCreateDescriptorPool(
		device,                                                                //device
		0,                                                                     //pool_idx
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER | VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, //descriptor_type
		framebuffer_count * 2,                                                 //descriptor_count /*me from the future: times 2 because it's shared with */
		p_pipeline_pool                                                        //p_pipeline_pool
	);

	r = r && shPipelinePoolAllocateDescriptorSetUnits(
		device,                            //device
		0,                                 //binding
		0,                                 //pool_idx
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, //descriptor_type
		0,                                 //first_descriptor_set_unit
		framebuffer_count * 2,             //descriptor_set_unit_count
		p_pipeline_pool                    //p_pipeline_pool
	);
	
	r = r && shPipelinePoolSetDescriptorBufferInfos(
		0,                             //first_descriptor
		framebuffer_count,             //descriptor_count
		p_gui->dst_buffer,             //buffer
		SH_GUI_REGIONS_RAW_DST_OFFSET, //buffer_offset
		SH_GUI_REGIONS_RAW_DST_SIZE,   //buffer_size
		p_pipeline_pool                //p_pipeline_pool
	);
	
	r = r && shPipelinePoolSetDescriptorBufferInfos(
		framebuffer_count,           //first_descriptor
		framebuffer_count,           //descriptor_count
		p_gui->dst_buffer,           //buffer
		SH_GUI_CHARS_RAW_DST_OFFSET, //buffer_offset
		SH_GUI_CHARS_RAW_DST_SIZE,   //buffer_size
		p_pipeline_pool              //p_pipeline_pool
	);
	
	r = r && shPipelinePoolUpdateDescriptorSetUnits(
		device,                //device
		0,                     //first_descriptor_set_unit
		framebuffer_count * 2, //descriptor_set_unit_count
		p_pipeline_pool        //p_pipeline_pool
	);

	//r = r && shPipelinePoolAllocateDescriptorSets(
	//	device, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	//	0, framebuffer_count * 2, p_pipeline_pool
	//);
	//
	//r = r && shPipelinePoolSetDescriptorSetBufferInfos(
	//	0, framebuffer_count,
	//	p_gui->dst_buffer,
	//	SH_GUI_REGIONS_RAW_DST_OFFSET,
	//	SH_GUI_REGIONS_RAW_DST_SIZE,
	//	p_pipeline_pool
	//);
	//
	//r = r && shPipelinePoolSetDescriptorSetBufferInfos(
	//	framebuffer_count, framebuffer_count,
	//	p_gui->dst_buffer,
	//	SH_GUI_CHARS_RAW_DST_OFFSET,
	//	SH_GUI_CHARS_RAW_DST_SIZE,
	//	p_pipeline_pool
	//);
	//
	//r = r && shPipelinePoolUpdateDescriptorSets(
	//	device, 0, framebuffer_count * 2, p_pipeline_pool
	//);

	shGuiError(
		r == 0,
		"failed allocating pipeline resources",
		return 0
	);

	return r;
}

uint8_t shGuiBuildRegionPipeline(
	ShGui*      p_gui, 
	const char* vertex_shader_path, 
	const char* fragment_shader_path
) {
	shGuiError(p_gui == NULL, "invalid gui memory",   return 0);

	VkDevice          device                = p_gui->core.device;
	VkPhysicalDevice  physical_device       = p_gui->core.physical_device;
	VkSurfaceKHR      surface               = p_gui->core.surface;
	VkRenderPass      renderpass            = p_gui->core.render_pass;
	uint32_t          swapchain_image_count = p_gui->core.swapchain_image_count;
	ShGuiRegionInfos* p_region_infos        = &p_gui->region_infos;
	ShVkPipeline* p_region_pipeline         = &p_gui->region_pipeline;

	shGuiError(device                == VK_NULL_HANDLE, "invalid device memory",          return 0);
	shGuiError(physical_device       == VK_NULL_HANDLE, "invalid physical device memory", return 0);
	shGuiError(surface               == VK_NULL_HANDLE, "invalid surface memory",         return 0);
	shGuiError(renderpass            == VK_NULL_HANDLE, "invalid renderpass memory",      return 0);
	shGuiError(swapchain_image_count == 0,              "invalid swapchain image count",  return 0);

	uint8_t r = 1;

	VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };
	r = r && shGetPhysicalDeviceSurfaceCapabilities(
		physical_device, surface,
		&surface_capabilities
	);

	r = r && shPipelineCreateRasterizer(
		VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
		p_region_pipeline
	);

	r = r && shPipelineSetMultisampleState(
		(VkSampleCountFlagBits)p_gui->core.sample_count, 0.0f,
		p_region_pipeline
	);

	r = r && shPipelineSetViewport(
		0, 0,
		surface_capabilities.currentExtent.width,
		surface_capabilities.currentExtent.height,
		0, 0,
		surface_capabilities.currentExtent.width,
		surface_capabilities.currentExtent.height,
		p_region_pipeline
	);

	r = r && shPipelineColorBlendSettings(
		0,
		0,
		1,
		p_region_pipeline
	);

	r = r && shPipelineCreateInputAssembly(
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE, p_region_pipeline
	);

	r = r && shPipelineSetVertexInputState(
		p_region_pipeline
	);

	r = r && shPipelineSetPushConstants(
		VK_SHADER_STAGE_VERTEX_BIT,
		0, 80,
		p_region_pipeline
	);

	if (vertex_shader_path != NULL && fragment_shader_path != NULL) {
		uint32_t src_size = 0;
		char* src = (char*)shGuiReadBinary(vertex_shader_path, &src_size);

		shGuiError(src == VK_NULL_HANDLE, "invalid shader source memory", return 0);

		r = r && shPipelineCreateShaderModule(
			device, src_size, src,
			p_region_pipeline
		);

		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_VERTEX_BIT,
			p_region_pipeline
		);

		free(src);
		src = (char*)shGuiReadBinary(fragment_shader_path, &src_size);

		shGuiError(src == VK_NULL_HANDLE, "invalid shader source memory", return 0);

		r = r && shPipelineCreateShaderModule(
			device, src_size, src,
			p_region_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_FRAGMENT_BIT,
			p_region_pipeline
		);

		free(src);
	}
	else {
		r = r && shPipelineCreateShaderModule(
			device, sizeof(SH_GUI_REGION_VERT_SPV),
			(char*)SH_GUI_REGION_VERT_SPV, p_region_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_VERTEX_BIT, p_region_pipeline
		);
		r = r && shPipelineCreateShaderModule(
			device, sizeof(SH_GUI_REGION_FRAG_SPV),
			(char*)SH_GUI_REGION_FRAG_SPV, p_region_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_FRAGMENT_BIT,
			p_region_pipeline
		);
	}

	r = r && shPipelineCreateLayout(
		device, 0, swapchain_image_count, &p_gui->pipeline_pool, p_region_pipeline
	);

	r = r && shSetupGraphicsPipeline(
		device, renderpass, p_region_pipeline
	);

	shGuiError(r == 0, "failed setting up region pipeline", return 0);

	return 1;
}

uint8_t shGuiBuildCharPipeline(
	ShGui*      p_gui,
	const char* vertex_shader_path,
	const char* fragment_shader_path
) {
	shGuiError(p_gui          == NULL, "invalid gui memory", return 0);

	VkDevice          device                = p_gui->core.device;
	VkPhysicalDevice  physical_device       = p_gui->core.physical_device;
	VkSurfaceKHR      surface               = p_gui->core.surface;
	VkRenderPass      renderpass            = p_gui->core.render_pass;
	uint32_t          swapchain_image_count = p_gui->core.swapchain_image_count;
	ShGuiCharInfos*   p_char_infos          = &p_gui->char_infos;
	ShVkPipeline*     p_char_pipeline       = &p_gui->char_pipeline;

	shGuiError(device                == VK_NULL_HANDLE, "invalid device memory",                     return 0);
	shGuiError(physical_device       == VK_NULL_HANDLE, "invalid physical device memory",            return 0);
	shGuiError(surface               == VK_NULL_HANDLE, "invalid surface memory",                    return 0);
	shGuiError(renderpass            == VK_NULL_HANDLE, "invalid renderpass memory",                 return 0);
	shGuiError(swapchain_image_count == 0,              "invalid swapchain image framebuffer count", return 0);

	uint8_t r = 1;

	VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };
	r = r && shGetPhysicalDeviceSurfaceCapabilities(
		physical_device, surface,
		&surface_capabilities
	);

	r = r && shPipelineCreateRasterizer(
		VK_POLYGON_MODE_POINT, VK_CULL_MODE_NONE,
		p_char_pipeline
	);

	r = r && shPipelineSetMultisampleState(
		(VkSampleCountFlagBits)p_gui->core.sample_count, 0.0f,
		p_char_pipeline
	);

	r = r && shPipelineSetViewport(
		0, 0,
		surface_capabilities.currentExtent.width,
		surface_capabilities.currentExtent.height,
		0, 0,
		surface_capabilities.currentExtent.width,
		surface_capabilities.currentExtent.height,
		p_char_pipeline
	);

	r = r && shPipelineColorBlendSettings(
		0,
		0,
		1,
		p_char_pipeline
	);

	r = r && shPipelineSetVertexBinding(
		0, 12, VK_VERTEX_INPUT_RATE_VERTEX,
		p_char_pipeline
	);

	r = r && shPipelineSetVertexAttribute(
		0, 0, VK_FORMAT_R32G32B32_SFLOAT,
		0, p_char_pipeline
	);

	r = r && shPipelineSetVertexInputState(p_char_pipeline);

	r = r && shPipelineCreateInputAssembly(
		VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
		VK_FALSE, p_char_pipeline
	);

	r = r && shPipelineSetPushConstants(
		VK_SHADER_STAGE_VERTEX_BIT, 
		0, 80, 
		p_char_pipeline
	);

	if (vertex_shader_path != NULL && fragment_shader_path != NULL) {
		uint32_t src_size = 0;
		char*    src      = (char*)shGuiReadBinary(vertex_shader_path, &src_size);
		
		shGuiError(src == NULL, "invalid shader source memory", return 0);

		r = r && shPipelineCreateShaderModule(
			device, src_size, src,
			p_char_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_VERTEX_BIT,
			p_char_pipeline
		);
		
		free(src);
		src = (char*)shGuiReadBinary(fragment_shader_path, &src_size);
		
		shGuiError(src == NULL, "invalid shader source memory", return 0);

		r = r && shPipelineCreateShaderModule(
			device, src_size, src, p_char_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_FRAGMENT_BIT,
			p_char_pipeline
		);
		
		free(src);
	}
	else {
		r = r && shPipelineCreateShaderModule(
			device, sizeof(SH_GUI_CHAR_VERT_SPV), 
			(char*)SH_GUI_CHAR_VERT_SPV, p_char_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_VERTEX_BIT,
			p_char_pipeline
		);
		r = r && shPipelineCreateShaderModule(
			device, sizeof(SH_GUI_CHAR_FRAG_SPV), 
			(char*)SH_GUI_CHAR_FRAG_SPV,
			p_char_pipeline
		);
		r = r && shPipelineCreateShaderStage(
			VK_SHADER_STAGE_FRAGMENT_BIT,
			p_char_pipeline
		);
	}

	r = r && shPipelineCreateLayout(
		device, swapchain_image_count, swapchain_image_count, &p_gui->pipeline_pool, p_char_pipeline//?
	);

	r = r && shSetupGraphicsPipeline(device, renderpass, p_char_pipeline);

	shGuiError(r == 0, "failed setting up region pipeline", return 0);

	return 1;
}

uint8_t shGuiDestroyPipelineResources(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice          device          =  p_gui->core.device;
	ShVkPipelinePool* p_pipeline_pool = &p_gui->pipeline_pool;

	uint8_t r = 1;

	r = r && shPipelinePoolDestroyDescriptorSetLayouts(device, 0, 1, p_pipeline_pool);
	r = r && shPipelinePoolDestroyDescriptorPools     (device, 0, 1, p_pipeline_pool);

	shGuiError(r == 0, "failed destroying pipeline resources", return 0);

	return 1;
}

uint8_t shGuiDestroyPipelines(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice          device                 =  p_gui->core.device;
	ShVkPipeline*     p_region_pipeline      = &p_gui->region_pipeline;
	ShVkPipeline*     p_char_pipeline        = &p_gui->char_pipeline;

	shGuiError(device == VK_NULL_HANDLE, "invalid device memory",         return 0);

	uint8_t r = 1;

	r = r && shPipelineDestroyShaderModules           (device, 0, 2, p_region_pipeline);
	r = r && shPipelineDestroyLayout                  (device, p_region_pipeline);
	r = r && shDestroyPipeline                        (device, p_region_pipeline->pipeline);
	r = r && shClearPipeline                          (p_region_pipeline);

	shGuiError(r == 0, "failed destroying region pipeline", return 0);

	r = r && shPipelineDestroyShaderModules           (device, 0, 2, p_char_pipeline);
	r = r && shPipelineDestroyLayout                  (device, p_char_pipeline);
	r = r && shDestroyPipeline                        (device, p_char_pipeline->pipeline);
	r = r && shClearPipeline                          (p_char_pipeline);

	shGuiError(r == 0, "failed destroying char pipeline", return 0);

	return 1;
}

uint8_t shGuiWriteMemory(
	ShGui*          p_gui, 
	VkCommandBuffer transfer_cmd_buffer,
	uint8_t         begin_cmd_buffer
) {
	shGuiError(p_gui               == NULL,           "invalid gui memory",            return 0);
	shGuiError(transfer_cmd_buffer == VK_NULL_HANDLE, "invalid command buffer memory", return 0);

	VkDevice        device         = p_gui->core.device;
	VkQueue         queue          = p_gui->core.graphics_queue;
	VkBuffer        staging_buffer = p_gui->staging_buffer;
	VkDeviceMemory  staging_memory = p_gui->staging_memory;
	VkBuffer        dst_buffer     = p_gui->dst_buffer;
	VkDeviceMemory  dst_memory     = p_gui->dst_memory;

	shGuiError(device         == VK_NULL_HANDLE, "invalid device memory",             return 0);
	shGuiError(queue          == VK_NULL_HANDLE, "invalid queue memory",              return 0);
	shGuiError(staging_buffer == VK_NULL_HANDLE, "invalid staging buffer memory",     return 0);
	shGuiError(staging_memory == VK_NULL_HANDLE, "invalid staging device memory",     return 0);
	shGuiError(dst_buffer     == VK_NULL_HANDLE, "invalid destination buffer memory", return 0);
	shGuiError(dst_memory     == VK_NULL_HANDLE, "invalid destination device memory", return 0);

	uint32_t regions_write_offset          = SH_GUI_REGIONS_RAW_DST_OFFSET;
	uint32_t regions_write_size            = SH_GUI_REGIONS_RAW_DST_SIZE;
	uint32_t chars_raw_write_offset        = SH_GUI_CHARS_RAW_DST_OFFSET;
	uint32_t chars_raw_write_size          = SH_GUI_CHARS_RAW_DST_SIZE;
	uint32_t chars_vertex_raw_write_offset = SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET;
	uint32_t chars_vertex_raw_write_size   = SH_GUI_CHARS_VERTEX_RAW_DST_SIZE;
	
	uint32_t buffer_write_size             = SH_GUI_DST_BUFFER_SIZE;

	shGuiError(
		buffer_write_size == 0,
		"invalid write size",
		return 0
	);

	uint8_t r = 1;

	VkFence fence = VK_NULL_HANDLE;
	r = r && shCreateFences(device, 1, 1, &fence);

	shVkError(fence == VK_NULL_HANDLE, "invalid fence memory", return 0);

	r = r && shWriteMemory(
		device, staging_memory, 
		regions_write_offset, regions_write_size,
		p_gui->region_infos.regions_raw
	);

	r = r && shWriteMemory(
		device, staging_memory,
		chars_raw_write_offset, chars_raw_write_size,
		p_gui->char_infos.chars_raw
	);

	r = r && shWriteMemory(
		device, staging_memory,
		chars_vertex_raw_write_offset, chars_vertex_raw_write_size,
		p_gui->char_infos.chars_vertex_raw
	);

	if (begin_cmd_buffer) {
		r = r && shWaitForFences(
			device, 1, 
			&fence, 
			1, UINT64_MAX
		);
		r = r && shResetFences(device, 1, &fence);
		r = r && shBeginCommandBuffer(transfer_cmd_buffer);
	}

	r = r && shCopyBufferRegions(
		transfer_cmd_buffer, staging_buffer,
		1, &regions_write_offset, &regions_write_offset,
		&buffer_write_size, dst_buffer
	);

	if (begin_cmd_buffer) {
		r = r && shEndCommandBuffer(transfer_cmd_buffer);
		r = r && shQueueSubmit(
			1, &transfer_cmd_buffer, queue,
			fence, 0, VK_NULL_HANDLE, 
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, VK_NULL_HANDLE
		);
		r = r && shWaitForFences(
			device, 1, &fence, 1, UINT64_MAX
		);
	}

	r = r && shDestroyFences(device, 1, &fence);

	shGuiError(r == 0, "failed writing memory", return 0);

	return 1;
}

uint8_t shGuiResizeInterface(
	ShGui*   p_gui, 
	uint32_t last_width, 
	uint32_t last_height, 
	uint32_t current_width, 
	uint32_t current_height
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	for (uint32_t region_idx = 0; region_idx < SH_GUI_MAX_REGION_COUNT; region_idx++) {
	
		ShGuiRegionRaw* p_region_raw = &p_gui->region_infos.regions_raw[region_idx];
		
		float pos_x   = p_region_raw->position.x / (float)last_width  * (float)current_width;
		float pos_y   = p_region_raw->position.y / (float)last_height * (float)current_height;

		float scale_x = p_region_raw->scale.x    / (float)last_width  * (float)current_width;
		float scale_y = p_region_raw->scale.y    / (float)last_height * (float)current_height;

		p_region_raw->position.x = pos_x;
		p_region_raw->position.y = pos_y;

		p_region_raw->scale.x    = scale_x;
		p_region_raw->scale.y    = scale_y;
	}

	return 1;
}

uint8_t shGuiReleaseMemory(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice       device         = p_gui->core.device;
	VkBuffer       staging_buffer = p_gui->staging_buffer;
	VkDeviceMemory staging_memory = p_gui->staging_memory;
	VkBuffer       dst_buffer     = p_gui->dst_buffer;
	VkDeviceMemory dst_memory     = p_gui->dst_memory;

	shGuiError(device         == VK_NULL_HANDLE, "invalid device memory",             return 0);
	shGuiError(staging_buffer == VK_NULL_HANDLE, "invalid staging buffer memory",     return 0);
	shGuiError(staging_memory == VK_NULL_HANDLE, "invalid staging device memory",     return 0);
	shGuiError(dst_buffer     == VK_NULL_HANDLE, "invalid destination buffer memory", return 0);
	shGuiError(dst_memory     == VK_NULL_HANDLE, "invalid destination device memory", return 0);

	uint8_t r = 1;

	r = r && shClearBufferMemory(device, staging_buffer, staging_memory);
	r = r && shClearBufferMemory(device, dst_buffer,     dst_memory);

	shGuiError(r == 0, "failed releasing memory", return 0);

	return 1;
}

uint8_t shGuiResetWidgetCount(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	p_gui->region_infos.region_count = 0;
	p_gui->char_infos.char_count     = 0;

	return 1;
}

uint8_t shGuiRender(
	ShGui*          p_gui,
	VkCommandBuffer cmd_buffer,
	uint32_t        swapchain_image_idx
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	VkDevice      device            = p_gui->core.device;
	VkBuffer      dst_buffer        = p_gui->dst_buffer;
	ShVkPipeline* p_region_pipeline = &p_gui->region_pipeline;
	ShVkPipeline* p_char_pipeline   = &p_gui->char_pipeline; 

	shGuiError(device     == VK_NULL_HANDLE, "invalid device memory",             return 0);
	shGuiError(dst_buffer == VK_NULL_HANDLE, "invalid destination buffer memory", return 0);

	float push_constant_data[18] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		(float)(*p_gui->inputs.p_window_width),
		(float)(*p_gui->inputs.p_window_height)
	};

	uint8_t r            = 1;
	uint8_t set_bindings = 0;

	r = r && shBindPipeline(
		cmd_buffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		p_region_pipeline
	);

	r = r && shPipelinePushConstants(cmd_buffer, push_constant_data, p_region_pipeline);

	r = r && shPipelineBindDescriptorSetUnits(
		cmd_buffer,                      //cmd_buffer
		0,                               //first_descriptor_set
		swapchain_image_idx,             //first_descriptor_set_unit_idx
		1,                               //descriptor_set_unit_count
		VK_PIPELINE_BIND_POINT_GRAPHICS, //bind_point
		0,                               //dynamic_descriptors_count
		VK_NULL_HANDLE,                  //p_dynamic_offsets
		&p_gui->pipeline_pool,           //p_pipeline_pool
		p_region_pipeline                //p_pipeline
	);

	//shPipelineBindDescriptorSets(
	//	cmd_buffer,
	//	swapchain_image_idx,
	//	1, VK_PIPELINE_BIND_POINT_GRAPHICS,
	//	0, NULL,
	//	&p_gui->pipeline_pool,
	//	p_region_pipeline
	//);

	set_bindings++;

	r = r && shDraw(
		cmd_buffer, 
		p_gui->region_infos.region_count * 6, 0, 
		1, 0
	);


	r = r && shBindPipeline(
		cmd_buffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		p_char_pipeline
	);

	r = r && shPipelinePushConstants(
		cmd_buffer, 
		push_constant_data, 
		p_char_pipeline
	);

	r = r && shPipelineBindDescriptorSetUnits(
		cmd_buffer,                                                               //cmd_buffer
		0,                                                                        //first_descriptor_set
		(p_gui->core.swapchain_image_count * set_bindings) + swapchain_image_idx, //first_descriptor_set_unit_idx
		1,                                                                        //descriptor_set_unit_count
		VK_PIPELINE_BIND_POINT_GRAPHICS,                                          //bind_point
		0,                                                                        //dynamic_descriptors_count
		VK_NULL_HANDLE,                                                           //p_dynamic_offsets
		&p_gui->pipeline_pool,                                                    //p_pipeline_pool
		p_char_pipeline                                                           //p_pipeline
	);

	set_bindings++;

	//shPipelineBindDescriptorSets(
	//	cmd_buffer,
	//	(p_gui->core.swapchain_image_count * set_bindings) + swapchain_image_idx,
	//	1, VK_PIPELINE_BIND_POINT_GRAPHICS,
	//	0, NULL,
	//	&p_gui->pipeline_pool, p_char_pipeline
	//);

	VkDeviceSize vertex_raw_offset = SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET;
	r = r && shBindVertexBuffers(
		cmd_buffer, 0, 1, &dst_buffer, &vertex_raw_offset
	);
	
	r = r && shDraw(
		cmd_buffer,
		p_gui->char_infos.char_count * SH_GUI_MAX_CHAR_VERTEX_COUNT, 0,
		1, 0
	);

	shGuiError(r == 0, "failed rendering gui", return 0);

	return 1;
}

uint8_t shGuiSubmitInputs(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	memcpy(
		p_gui->inputs.last_key_events, 
		p_gui->inputs.key_events, 
		sizeof(ShGuiKeyEvents)
	);

	//?????????? missing char/text events

	return 1;
}

uint8_t shGuiRelease(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	shGuiReleaseMemory(p_gui);
	shGuiDestroyPipelineResources(p_gui);
	shGuiDestroyPipelines(p_gui);

	return 1;
}

uint8_t shGuiLinkInputs(
	ShGui*           p_gui,
	uint32_t*        p_window_width, 
	uint32_t*        p_window_height, 
	float*           p_cursor_pos_x, 
	float*           p_cursor_pos_y, 
	ShGuiKeyEvents   key_events,
	ShGuiKeyEvents   last_key_events,
	double*          p_delta_time
) {
	shGuiError(p_gui           == NULL, "invalid gui memory",           return 0);
	shGuiError(p_window_width  == NULL, "invalid window width memory",  return 0);
	shGuiError(p_window_height == NULL, "invalid window height memory", return 0);
	shGuiError(p_cursor_pos_x  == NULL, "invalid cursor pos x memory",  return 0);
	shGuiError(p_cursor_pos_y  == NULL, "invalid cursor pos y memory",  return 0);
	shGuiError(key_events      == NULL, "invalid key events memory",    return 0);
	shGuiError(p_delta_time    == NULL, "invalid delta time memory",    return 0);

	ShGuiInputs inputs = {
		p_window_width,        //p_window_width
		p_window_height,       //p_window_height
		key_events,            //key_events
		last_key_events,       //last_key_events
		0                      //system_submitted_char
	};

	p_gui->inputs = inputs;

	return 1;
}

uint8_t shGuiUpdateInputs(ShGui* p_gui) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	memcpy(p_gui->inputs.last_key_events,   p_gui->inputs.key_events, sizeof(ShGuiKeyEvents));

	return 1;
}

uint8_t shGuiScaleUI(
	ShGui* p_gui,
	float  region_scale_factor,
	float  char_scale_factor
) {
	shGuiError(p_gui               == NULL, "invalid gui memory",                  return 0);
	shGuiError(region_scale_factor == 0.0f, "invalid region scaling factor value", return 0);
	shGuiError(char_scale_factor   == 0.0f, "invalid char scaling factor value",   return 0);

	p_gui->region_infos.region_scale_factor = region_scale_factor;
	p_gui->char_infos.char_scale_factor     = char_scale_factor;

	return 1;
}

uint8_t shGuiRegion(
	ShGui*           p_gui,
	shguivec2        position_zero,
	shguivec2        scale_zero,
	shguivec4        fill_color_zero,
	shguivec4        edge_color_zero,
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float           window_size_x = (float)(*p_gui->inputs.p_window_width);
	float           window_size_y = (float)(*p_gui->inputs.p_window_height);

	uint32_t        region_count  =  p_gui->region_infos.region_count;
	ShGuiRegionRaw* p_region_raw  = &p_gui->region_infos.regions_raw[region_count];

	ShGuiRegionRaw region_raw = {
		position_zero,
		scale_zero,
		fill_color_zero,
		(shguivec3) { edge_color_zero.x, edge_color_zero.y, edge_color_zero.z },
		SH_GUI_REGION_Z_PRIORITY
	};

	if (flags & SH_GUI_PIXELS) {
		//fine
	}

	else if (flags & SH_GUI_RELATIVE) {
		region_raw.scale.x    = scale_zero.x    / 100.0f * window_size_x * p_gui->region_infos.region_scale_factor;
		region_raw.scale.y    = scale_zero.y    / 100.0f * window_size_y * p_gui->region_infos.region_scale_factor;
		region_raw.position.x = position_zero.x / 100.0f * window_size_x / 2.0f;
		region_raw.position.y = position_zero.y / 100.0f * window_size_y / 2.0f;
	}

	if (flags & SH_GUI_EDGE_LEFT) {
		region_raw.position.x -= (window_size_x - region_raw.scale.x) / 2.0f;
	}

	if (flags & SH_GUI_EDGE_RIGHT) {
		region_raw.position.x += (window_size_x - region_raw.scale.x) / 2.0f;
	}

	if (flags & SH_GUI_EDGE_TOP) {
		region_raw.position.y += (window_size_y - region_raw.scale.y) / 2.0f;
	}

	if (flags & SH_GUI_EDGE_BOTTOM) {
		region_raw.position.y -= (window_size_y - region_raw.scale.y) / 2.0f;
	}

	ShGuiRegionRawWriteFlags region_written = p_gui->region_infos.regions_raw_write_flags[p_gui->region_infos.region_count];
	
	if ((region_written & SH_GUI_REGION_RAW_WRITE_X_POSITION) == 0) {
		p_region_raw->position.x = region_raw.position.x;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_Y_POSITION) == 0) {
		p_region_raw->position.y = region_raw.position.y;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_X_SCALE   ) == 0) {
		p_region_raw->scale.x = region_raw.scale.x;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_Y_SCALE   ) == 0) {
		p_region_raw->scale.y = region_raw.scale.y;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_COLOR     ) == 0) {
		p_region_raw->color = region_raw.color;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_EDGE_COLOR) == 0) {
		p_region_raw->edge_color = region_raw.edge_color;
	} 
	if ((region_written & SH_GUI_REGION_RAW_WRITE_Z_PRIORITY) == 0) {
		p_region_raw->z_priority = region_raw.z_priority;
	} 

	p_gui->region_infos.region_count++;

	return 0;
}

uint8_t shGuiOverwriteRegion(
	ShGui*                   p_gui,
	uint32_t                 region_idx,
	ShGuiRegionRaw*          p_src_data,
	ShGuiRegionRawWriteFlags flags
) {
	shGuiError(p_gui      == NULL, "invalid gui memory",      return 0);
	shGuiError(p_src_data == NULL, "invalid src data memory", return 0);

	ShGuiRegionRaw*           p_region_raw             = &p_gui->region_infos.regions_raw            [region_idx];
	ShGuiRegionRawWriteFlags* p_region_raw_write_flags = &p_gui->region_infos.regions_raw_write_flags[region_idx];

	(*p_region_raw_write_flags) |= flags;

	if (flags & SH_GUI_REGION_RAW_WRITE_X_POSITION) {
		p_region_raw->position.x = p_src_data->position.x;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_Y_POSITION) {
		p_region_raw->position.y = p_src_data->position.y;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_SCALE) {
		p_region_raw->scale.x    = p_src_data->scale.x;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_Y_SCALE) {
		p_region_raw->scale.y    = p_src_data->scale.y;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_COLOR) {
		p_region_raw->color      = p_src_data->color;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_EDGE_COLOR) {
		p_region_raw->edge_color = p_src_data->edge_color;
	}
	if (flags & SH_GUI_REGION_RAW_WRITE_Z_PRIORITY) {
		p_region_raw->z_priority   = p_src_data->z_priority;
	}

	return 1;
}

uint8_t shGuiOverwriteRegions(
	ShGui*                   p_gui,
	uint32_t                 first_region,
	uint32_t                 region_count,
	ShGuiRegionRaw*          p_src_data,
	ShGuiRegionRawWriteFlags flags
) {
	shGuiError(p_gui      == NULL, "invalid gui memory",      return 0);
	shGuiError(p_src_data == NULL, "invalid src data memory", return 0);

	shGuiError(
		first_region + region_count > SH_GUI_MAX_REGION_COUNT,
		"invalid region range",
		return 0
	);

	for (uint32_t region_idx = first_region; region_idx < (first_region + region_count); region_idx++) {
		shGuiOverwriteRegion(p_gui, region_idx, p_src_data, flags);
	}

	return 1;
}

uint8_t shGuiTextLineCount(
	const char* s_text,
	uint32_t*   p_count
) {
	shGuiError(s_text  == VK_NULL_HANDLE, "invalid text memory",            return 0);
	shGuiError(p_count == VK_NULL_HANDLE, "invalid line line count memory", return 0);

	(*p_count) = 1;

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		(s_text[char_idx] == '\n') && ((*p_count)++);
	}

	return 1;
}

uint8_t shGuiText(
	ShGui*           p_gui,
	shguivec2        position_zero, 
	shguivec4        fill_color,
	float            scale_zero_pixels,
	char*            s_text,
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui             == NULL, "invalid gui memory",       return 0);
	shGuiError(s_text            == NULL, "invalid text memory",      return 0);
	shGuiError(scale_zero_pixels == 0.0f, "invalid text scale value", return 0)

	float               window_size_x    = (float)(*p_gui->inputs.p_window_width);
	float               window_size_y    = (float)(*p_gui->inputs.p_window_height);

	ShGuiCharRaw*       chars_raw        = p_gui->char_infos.chars_raw;
	ShGuiCharVertexRaw* chars_vertex_raw = p_gui->char_infos.chars_vertex_raw;

	float               x_offset         = 0.0f;//current char x offset, see loop
	float               x_occupied       = 0.0f;//total x space occupied
	float               blank_h_space    = 0.0f;//blank horizontal space between one char and the other

	float               y_offset         = 0.0f;//current char y offset, see loop
	float               y_occupied       = 0.0f;//total y space occupied
	float               blank_v_space    = 0.0f;//blank vertical space between one char and the other

	uint8_t             new_line           = 1;
	uint32_t            line_idx           = 0;
	uint32_t            line_char_idx      = 0;

	uint32_t            max_chars_in_line  = 0;
	uint32_t            current_line_chars = 0;

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		
		uint32_t               char_count        =  p_gui->char_infos.char_count;
		ShGuiCharRaw*          p_char_raw        = &p_gui->char_infos.chars_raw[char_count];
		float*                 p_char_vertex_raw =  p_gui->char_infos.chars_vertex_raw[char_count];
		ShGuiCharRawWriteFlags char_written      =  p_gui->char_infos.chars_raw_write_flags[char_count];


		if ((char_written & SH_GUI_CHAR_RAW_WRITE_SCALE) == 0) {
			p_char_raw->scale = scale_zero_pixels;
		}
		if ((char_written & SH_GUI_CHAR_RAW_WRITE_Z_PRIORITY) == 0) {
			p_char_raw->z_priority = SH_GUI_TEXT_Z_PRIORITY;
		}
		if ((char_written & SH_GUI_CHAR_RAW_WRITE_COLOR) == 0) {
			p_char_raw->color = fill_color;
		}

		blank_h_space = p_char_raw->scale / 7.0f; //blank horizontal space between characters
		blank_v_space = p_char_raw->scale / 2.5f; //blank vertical space between characters

		//- line_idx to get rid of the \n character x offset
		x_offset      = (float)(!new_line) * (float)(line_char_idx) * (3.0f * blank_h_space + p_char_raw->scale);
		y_offset      =             -1.0f  * (float)(line_idx     ) * (3.0f * blank_v_space + p_char_raw->scale);

		if (new_line) {
			new_line           = 0;
			current_line_chars = 0;
		}

		current_line_chars++;

		if (current_line_chars > max_chars_in_line) {
			max_chars_in_line = current_line_chars;
		}

		if ((char_written & SH_GUI_CHAR_RAW_WRITE_X_POSITION) == 0) {
			p_char_raw->position.x = position_zero.x + x_offset; //pivot on left vertex
		}
		if ((char_written & SH_GUI_CHAR_RAW_WRITE_Y_POSITION) == 0) {
			p_char_raw->position.y = position_zero.y + y_offset; //pivot on line 0
		}

		switch (s_text[char_idx]) {
		case 'a':
		case 'A':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_A]);
			line_char_idx++;
			break;
		case 'B':
		case 'b':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_B]);
			line_char_idx++;
			break;
		case 'C':
		case 'c':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_C]);
			line_char_idx++;
			break;
		case 'D':
		case 'd':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_D]);
			line_char_idx++;
			break;
		case 'E':
		case 'e':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_E]);
			line_char_idx++;
			break;
		case 'F':
		case 'f':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_F]);
			line_char_idx++;
			break;
		case 'G':
		case 'g':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_G]);
			line_char_idx++;
			break;
		case 'H':
		case 'h':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_H]);
			line_char_idx++;
			break;
		case 'I':
		case 'i':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_I]);
			line_char_idx++;
			break;
		case 'J':
		case 'j':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_J]);
			line_char_idx++;
			break;
		case 'K':
		case 'k':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_K]);
			line_char_idx++;
			break;
		case 'L':
		case 'l':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_L]);
			line_char_idx++;
			break;
		case 'M':
		case 'm':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_M]);
			line_char_idx++;
			break;
		case 'N':
		case 'n':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_N]);
			line_char_idx++;
			break;
		case 'O':
		case 'o':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_O]);
			line_char_idx++;
			break;
		case 'P':
		case 'p':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_P]);
			line_char_idx++;
			break;
		case 'Q':
		case 'q':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_Q]);
			line_char_idx++;
			break;
		case 'R':
		case 'r':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_R]);
			line_char_idx++;
			break;
		case 'S':
		case 's':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_S]);
			line_char_idx++;
			break;
		case 'T':
		case 't':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_T]);
			line_char_idx++;
			break;
		case 'U':
		case 'u':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_U]);
			line_char_idx++;
			break;
		case 'V':
		case 'v':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_V]);
			line_char_idx++;
			break;
		case 'W':
		case 'w':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_W]);
			line_char_idx++;
			break;
		case 'X':
		case 'x':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_X]);
			line_char_idx++;
			break;
		case 'Y':
		case 'y':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_Y]);
			line_char_idx++;
			break;
		case 'Z':
		case 'z':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_Z]);
			line_char_idx++;
			break;
		case '0':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_0]);
			line_char_idx++;
			break;
		case '1':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_1]);
			line_char_idx++;
			break;
		case '2':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_2]);
			line_char_idx++;
			break;
		case '3':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_3]);
			line_char_idx++;
			break;
		case '4':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_4]);
			line_char_idx++;
			break;
		case '5':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_5]);
			line_char_idx++;
			break;
		case '6':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_6]);
			line_char_idx++;
			break;
		case '7':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_7]);
			line_char_idx++;
			break;
		case '8':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_8]);
			line_char_idx++;
			break;
		case '9':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_9]);
			line_char_idx++;
			break;
		case ' ':
			p_char_raw->z_priority = SH_GUI_OUT_OF_CANVAS_Z_PRIORITY;
			line_char_idx++;
			break;
		case '\t':
			//manage tab
			p_char_raw->z_priority = SH_GUI_OUT_OF_CANVAS_Z_PRIORITY;
			line_char_idx++;
			break;
		case '.':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_PERIOD]);
			line_char_idx++;
			break;
		case ',':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_COMMA]);
			line_char_idx++;
			break;
		case '!':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_EXCLAMATION]);
			line_char_idx++;
			break;
		case '?':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_QUESTION]);
			line_char_idx++;
			break;
		case ':':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_COLON]);
			line_char_idx++;
			break;
		case ';':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_SEMICOLON]);
			line_char_idx++;
			break;
		case '/':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_FORWARD_SLASH]);
			line_char_idx++;
			break;
		case '\\':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_BACKSLASH]);
			line_char_idx++;
			break;
		case '_':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_UNDERSCORE]);
			line_char_idx++;
			break;
		case '-':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_HYPHEN]);
			line_char_idx++;
			break;
		case '|':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_VERTICAL_BAR]);
			line_char_idx++;
			break;
		case '&':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_AMPERSAND]);
			line_char_idx++;
			break;
		case '%':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_PERCENT]);
			line_char_idx++;
			break;
		case '$':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_DOLLAR]);
			line_char_idx++;
			break;
		case '#':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_HASH]);
			line_char_idx++;
			break;
		case '@':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_AT]);
			line_char_idx++;
			break;
		case '\'':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_SINGLE_QUOTE]);
			line_char_idx++;
			break;
		case '"':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_DOUBLE_QUOTE]);
			line_char_idx++;
			break;
		case '+':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_PLUS]);
			line_char_idx++;
			break;
		//case '-':
		//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_MINUS]);
		//	line_char_idx++;
			break;
		case '*':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_ASTERISK]);
			line_char_idx++;
			break;
		case '=':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_EQUALS]);
			line_char_idx++;
			break;
		case '<':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_LESS_THAN]);
			line_char_idx++;
			break;
		case '>':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_GREATER_THAN]);
			line_char_idx++;
			break;
		case '(':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_LEFT_PAREN]);
			line_char_idx++;
			break;
		case ')':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_RIGHT_PAREN]);
			line_char_idx++;
			break;
		case '[':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_LEFT_BRACKET]);
			line_char_idx++;
			break;
		case ']':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_RIGHT_BRACKET]);
			line_char_idx++;
			break;
		case '^':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_TILDE]);
			line_char_idx++;
			break;
		case '`':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, p_gui->char_infos.chars_font_mesh[SH_GUI_CHAR_INDEX_GRAVE_ACCENT]);
			line_char_idx++;
			break;
		case '\n':
			new_line = 1;
			line_idx++;
			p_char_raw->z_priority = SH_GUI_OUT_OF_CANVAS_Z_PRIORITY;
			line_char_idx = 0;
			break;
		default:
			p_char_raw->z_priority = SH_GUI_OUT_OF_CANVAS_Z_PRIORITY;
			line_char_idx++;
			break;
		}

		float x_occupied_correction_sum = -p_char_raw->scale / 40.0f;
		float y_occupied_correction_sum =  p_char_raw->scale /  1.6f;

		             //slightly different than y_occupied
		x_occupied = max_chars_in_line * (3.0f * blank_h_space + p_char_raw->scale) + p_char_raw->scale + x_occupied_correction_sum;
		y_occupied = y_offset + p_char_raw->scale + y_occupied_correction_sum;

		p_gui->char_infos.char_count++;
	}

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		

		uint32_t char_count    = p_gui->char_infos.char_count;
		uint32_t char_mesh_idx = char_count - ((uint32_t)strlen(s_text) - char_idx);

		ShGuiCharRaw* p_char_raw = &p_gui->char_infos.chars_raw[char_mesh_idx];

		if (flags & SH_GUI_CENTER_WIDTH) {
			float center_correction_sum  = p_char_raw->scale / 1.5f;
			p_char_raw->position.x      -= x_occupied        /  2.0f - center_correction_sum;//not precise for some reason
		}
		if (flags & SH_GUI_EDGE_LEFT) {
			p_char_raw->position.x -= window_size_x - blank_h_space;
		}
		if (flags & SH_GUI_EDGE_RIGHT) {
			p_char_raw->position.x += window_size_x - x_occupied;//max_x_offset already includes x_step_0
		}
		
		if (flags & SH_GUI_CENTER_HEIGHT) {
			float center_correction_sum  = p_char_raw->scale / 16.0f;
			p_char_raw->position.y      -= y_occupied        /  2.0f - center_correction_sum;//not precise for some reason
		}
		if (flags & SH_GUI_EDGE_BOTTOM) {
			p_char_raw->position.y -= window_size_y + y_offset;
		}
		if (flags & SH_GUI_EDGE_TOP) {
			p_char_raw->position.y += window_size_y - y_occupied + y_offset;
		}
	}

	return 1;
}

uint8_t shGuiOverwriteChar(
	ShGui*                 p_gui,
	uint32_t               char_idx,
	ShGuiCharRaw*          p_src_data,
	ShGuiCharRawWriteFlags flags
) {
	shGuiError(p_gui      == VK_NULL_HANDLE, "invalid gui memory",      return 0);
	shGuiError(p_src_data == VK_NULL_HANDLE, "invalid src data memory", return 0);

	ShGuiCharRaw*           p_char_raw             = &p_gui->char_infos.chars_raw            [char_idx];
	ShGuiCharRawWriteFlags* p_char_raw_write_flags = &p_gui->char_infos.chars_raw_write_flags[char_idx];

	(*p_char_raw_write_flags) |= flags;

	if (flags & SH_GUI_CHAR_RAW_WRITE_POSITION) {
		p_char_raw->position = p_src_data->position;
	}
	if (flags & SH_GUI_CHAR_RAW_WRITE_SCALE) {
		p_char_raw->scale    = p_src_data->scale;
	}
	if (flags & SH_GUI_CHAR_RAW_WRITE_Z_PRIORITY) {
		p_char_raw->z_priority = p_src_data->z_priority;
	}
	if (flags & SH_GUI_CHAR_RAW_WRITE_COLOR) {
		p_char_raw->color    = p_src_data->color;
	}

	return 1;
}

uint8_t shGuiOverwriteChars(
	ShGui*                 p_gui,
	uint32_t               first_char,
	uint32_t               char_count,
	ShGuiCharRaw*          p_src_data,
	ShGuiCharRawWriteFlags flags
) {
	shGuiError(p_gui      == NULL, "invalid gui memory", return 0);
	shGuiError(p_src_data == NULL, "invalid src data memory", return 0);

	shGuiError(
		first_char + char_count > SH_GUI_MAX_CHAR_COUNT,
		"invalid char range",
		return 0
	);

	for (uint32_t char_idx = first_char; char_idx < (first_char + char_count); char_idx++) {
		shGuiOverwriteChar(p_gui, char_idx, p_src_data, flags);
	}

	return 1;
}



#ifdef __cplusplus
}
#endif//__cplusplus