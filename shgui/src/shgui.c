#ifdef __cplusplus
"C" {
#endif//__cplusplus


#include "shgui/shgui.h"
#include "fonts/consolas.h"

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
	ShGui*   p_gui, 
	uint32_t structure_count, 
	uint32_t structure_size
) {
	shGuiError(p_gui           == NULL, "invalid gui memory",          return 0);
	shGuiError(structure_count == 0,    "invalid gui structure count", return 0);
	shGuiError(structure_size  == 0,    "invalid gui structure size",  return 0);

	VkDevice         device          = p_gui->core.device;
	VkPhysicalDevice physical_device = p_gui->core.physical_device;

	shGuiError(device          == VK_NULL_HANDLE, "invalid device memory",          return 0);
	shGuiError(physical_device == VK_NULL_HANDLE, "invalid physical device memory", return 0);

	uint32_t                                  host_memory_type_index      = 0;
	uint32_t                                  host_available_video_memory = 0;
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
	available_heap /= 2;

	max_structures_size = structure_count * structure_size;
	
	available_heap = (available_heap >= max_structures_size) ? max_structures_size : available_heap;

	return available_heap;
}

uint8_t shGuiAllocateMemory(
	ShGui* p_gui,
	uint32_t max_region_count,
	uint32_t max_char_count
) {
	shGuiError(p_gui            == NULL, "invalid gui memory",   return 0);
	shGuiError(max_region_count == 0,    "invalid region count", return 0);
	shGuiError(max_char_count   == 0,    "invalid char count",   return 0);
	
	VkDevice device                  = p_gui->core.device;
	VkPhysicalDevice physical_device = p_gui->core.physical_device;


	shGuiError(device          == VK_NULL_HANDLE, "invalid device memory",          return 0);
	shGuiError(physical_device == VK_NULL_HANDLE, "invalid physical device memory", return 0);

	ShGuiRegionInfos* p_region_infos = &p_gui->region_infos;
	ShGuiCharInfos*   p_char_infos   = &p_gui->char_infos;

	p_region_infos->max_regions_raw_size                         = shGuiGetAvailableHeap(p_gui, max_region_count, sizeof(ShGuiRegionRaw));
	p_region_infos->max_region_count                             = p_region_infos->max_regions_raw_size / sizeof(ShGuiRegionRaw);
					                                                       
									                                       
	p_region_infos->p_regions_raw                                = calloc(1,                                p_region_infos->max_regions_raw_size);
	p_region_infos->p_regions_raw_write_flags                    = calloc(sizeof(ShGuiRegionRawWriteFlags), p_region_infos->max_region_count);
														        
	p_region_infos->p_regions_clicked                            = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_cursor_on_regions                          = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_menus_region_indices                       = calloc(4, p_region_infos->max_region_count);
	p_region_infos->p_windows_region_indices                     = calloc(4, p_region_infos->max_region_count);
	p_region_infos->p_windows_used_height                        = calloc(4, p_region_infos->max_region_count);
														        
	p_region_infos->p_moving_regions                             = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_left_resizing_regions                      = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_right_resizing_regions                     = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_top_resizing_regions                       = calloc(1, p_region_infos->max_region_count);
	p_region_infos->p_bottom_resizing_regions                    = calloc(1, p_region_infos->max_region_count);
														        
	p_region_infos->p_windows_slider_buttons_offsets             = calloc(4, p_region_infos->max_region_count);
														        
	shGuiError(p_region_infos->p_regions_raw                    == VK_NULL_HANDLE, "invalid regions raw memory",                    return 0);
	shGuiError(p_region_infos->p_regions_raw_write_flags        == VK_NULL_HANDLE, "invalid regions write flags memory",            return 0);
	shGuiError(p_region_infos->p_regions_clicked                == VK_NULL_HANDLE, "invalid regions clicked memory",                return 0);
	shGuiError(p_region_infos->p_cursor_on_regions              == VK_NULL_HANDLE, "invalid cursor on regions memory",              return 0);
	shGuiError(p_region_infos->p_windows_region_indices         == VK_NULL_HANDLE, "invalid menu indices memory",                   return 0);
	shGuiError(p_region_infos->p_windows_region_indices         == VK_NULL_HANDLE, "invalid menu indices memory",                   return 0);
	shGuiError(p_region_infos->p_windows_used_height            == VK_NULL_HANDLE, "invalid menu indices memory",                   return 0);
														        														  
	shGuiError(p_region_infos->p_moving_regions                 == VK_NULL_HANDLE, "invalid moving regions bits memory",            return 0);
	shGuiError(p_region_infos->p_left_resizing_regions          == VK_NULL_HANDLE, "invalid left resizing regions bits memory",     return 0);
	shGuiError(p_region_infos->p_right_resizing_regions         == VK_NULL_HANDLE, "invalid right resizing regions bits memory",    return 0);
	shGuiError(p_region_infos->p_top_resizing_regions           == VK_NULL_HANDLE, "invalid top resizing regions bits memory",      return 0);
	shGuiError(p_region_infos->p_bottom_resizing_regions        == VK_NULL_HANDLE, "invalid bottom resizing regions bits memory",   return 0);
	
	shGuiError(p_region_infos->p_windows_slider_buttons_offsets == VK_NULL_HANDLE, "invalid windows slider buttons offsets memory", return 0);


	uint32_t max_chars_total_raw_size = shGuiGetAvailableHeap(
		p_gui, max_char_count, sizeof(ShGuiCharRaw) + sizeof(ShGuiCharVertexRaw)
	);

	uint32_t _max_char_count = max_chars_total_raw_size / (sizeof(ShGuiCharRaw) + sizeof(ShGuiCharVertexRaw));

	p_char_infos->max_char_count                       = _max_char_count;
	p_char_infos->max_chars_raw_size                   = p_char_infos->max_char_count * sizeof(ShGuiCharRaw);
	p_char_infos->max_chars_vertex_raw_size            = p_char_infos->max_char_count * sizeof(ShGuiCharVertexRaw);
										               
	p_char_infos->p_chars_raw                          = calloc(1,                              p_char_infos->max_chars_raw_size);
	p_char_infos->p_chars_vertex_raw                   = calloc(1,                              p_char_infos->max_chars_vertex_raw_size);
	p_char_infos->p_chars_raw_write_flags              = calloc(sizeof(ShGuiCharRawWriteFlags), p_char_infos->max_char_count);

	shGuiError(p_char_infos->p_chars_raw             == VK_NULL_HANDLE, "invalid chars raw memory",         return 0);
	shGuiError(p_char_infos->p_chars_vertex_raw      == VK_NULL_HANDLE, "invalid chars vertex raw memory",  return 0);
	shGuiError(p_char_infos->p_chars_raw_write_flags == VK_NULL_HANDLE, "invalid chars write flags memory", return 0);

	uint32_t staging_buffer_size = SH_GUI_STAGING_BUFFER_SIZE(p_region_infos->max_region_count, p_char_infos->max_char_count);
	uint32_t dst_buffer_size     = SH_GUI_DST_BUFFER_SIZE(p_region_infos->max_region_count, p_char_infos->max_char_count);

	uint8_t r = 1;

	r = r && shCreateBuffer(
		device, staging_buffer_size,
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
		device, dst_buffer_size,
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
		0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		1, VK_SHADER_STAGE_VERTEX_BIT,
		p_pipeline_pool
	);

	r = r && shPipelinePoolCreateDescriptorSetLayout(
		device, 0, 1, 0, p_pipeline_pool
	);

	r = r && shPipelinePoolCopyDescriptorSetLayout(
		0, 1, framebuffer_count * 2 - 1, p_pipeline_pool
	);

	r = r && shPipelinePoolCreateDescriptorPool(
		device, 0, 
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER | VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		framebuffer_count * 2, p_pipeline_pool
	);

	r = r && shPipelinePoolAllocateDescriptorSets(
		device, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		0, framebuffer_count * 2, p_pipeline_pool
	);

	r = r && shPipelinePoolSetDescriptorSetBufferInfos(
		0, framebuffer_count,
		p_gui->dst_buffer,
		SH_GUI_REGIONS_RAW_DST_OFFSET,
		SH_GUI_REGIONS_RAW_DST_SIZE(p_region_infos->max_region_count),
		p_pipeline_pool
	);

	r = r && shPipelinePoolSetDescriptorSetBufferInfos(
		framebuffer_count, framebuffer_count,
		p_gui->dst_buffer,
		SH_GUI_CHARS_RAW_DST_OFFSET(p_gui->region_infos.max_region_count),
		SH_GUI_CHARS_RAW_DST_SIZE(p_gui->char_infos.max_char_count),
		p_pipeline_pool
	);

	r = r && shPipelinePoolUpdateDescriptorSets(
		device, 0, framebuffer_count * 2, p_pipeline_pool
	);

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

	shGuiError(device            == VK_NULL_HANDLE, "invalid device memory",             return 0);
	shGuiError(queue             == VK_NULL_HANDLE, "invalid queue memory",              return 0);
	shGuiError(staging_buffer    == VK_NULL_HANDLE, "invalid staging buffer memory",     return 0);
	shGuiError(staging_memory    == VK_NULL_HANDLE, "invalid staging device memory",     return 0);
	shGuiError(dst_buffer        == VK_NULL_HANDLE, "invalid destination buffer memory", return 0);
	shGuiError(dst_memory        == VK_NULL_HANDLE, "invalid destination device memory", return 0);

	uint32_t regions_write_offset          = SH_GUI_REGIONS_RAW_DST_OFFSET;
	uint32_t regions_write_size            = SH_GUI_REGIONS_RAW_DST_SIZE(p_gui->region_infos.max_region_count);
	uint32_t chars_raw_write_offset        = SH_GUI_CHARS_RAW_DST_OFFSET(p_gui->region_infos.max_region_count);
	uint32_t chars_raw_write_size          = SH_GUI_CHARS_RAW_DST_SIZE(p_gui->char_infos.max_char_count);
	uint32_t chars_vertex_raw_write_offset = SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET(p_gui->region_infos.max_region_count, p_gui->char_infos.max_char_count);
	uint32_t chars_vertex_raw_write_size   = SH_GUI_CHARS_VERTEX_RAW_DST_SIZE(p_gui->char_infos.max_char_count);
	
	uint32_t buffer_write_size             = SH_GUI_DST_BUFFER_SIZE(p_gui->region_infos.max_region_count, p_gui->char_infos.max_char_count);

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
		p_gui->region_infos.p_regions_raw
	);

	r = r && shWriteMemory(
		device, staging_memory,
		chars_raw_write_offset, chars_raw_write_size,
		p_gui->char_infos.p_chars_raw
	);

	r = r && shWriteMemory(
		device, staging_memory,
		chars_vertex_raw_write_offset, chars_vertex_raw_write_size,
		p_gui->char_infos.p_chars_vertex_raw
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

	for (uint32_t region_idx = 0; region_idx < p_gui->region_infos.max_region_count; region_idx++) {
	
		ShGuiRegionRaw* p_region_raw = &p_gui->region_infos.p_regions_raw[region_idx];
		
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

	uint8_t r = 1;

	r = r && shBindPipeline(
		cmd_buffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		p_region_pipeline
	);

	r = r && shPipelinePushConstants(cmd_buffer, push_constant_data, p_region_pipeline);

	r = r && shPipelineBindDescriptorSets(
		cmd_buffer, 
		swapchain_image_idx,
		1, VK_PIPELINE_BIND_POINT_GRAPHICS,
		0, NULL,
		&p_gui->pipeline_pool,
		p_region_pipeline
	);

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

	r = r && shPipelineBindDescriptorSets(
		cmd_buffer, 
		swapchain_image_idx + 2,
		1, VK_PIPELINE_BIND_POINT_GRAPHICS,
		0, NULL,
		&p_gui->pipeline_pool, p_char_pipeline
	);


	VkDeviceSize vertex_raw_offset = SH_GUI_CHARS_VERTEX_RAW_DST_OFFSET(p_gui->region_infos.max_region_count, p_gui->char_infos.max_char_count);
	r = r && shBindVertexBuffers(
		cmd_buffer, 0, 1, &dst_buffer, &vertex_raw_offset
	);
	
	r = r && shDraw(
		cmd_buffer,
		p_gui->char_infos.char_count * SH_GUI_MAX_CHAR_VERTEX_SIZE / 3 / 4, 0,
		1, 0
	);


	memset(
		p_gui->region_infos.p_windows_used_height,
		0,
		p_gui->region_infos.window_count * 4
	);

	p_gui->region_infos.window_count = 0;
	p_gui->region_infos.region_count = 0;
	p_gui->region_infos.menu_count   = 0;
	p_gui->char_infos.char_count     = 0;
	p_gui->item_count                = 0;

	shGuiError(r == 0, "failed rendering gui", return 0);

	return 1;
}

uint8_t shGuiSubmitInputs(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	p_gui->inputs.last.last_cursor_pos_x = (*p_gui->inputs.p_cursor_pos_x);
	p_gui->inputs.last.last_cursor_pos_y = (*p_gui->inputs.p_cursor_pos_y);
	
	memcpy(
		p_gui->inputs.last.last_key_events, 
		p_gui->inputs.p_key_events, 
		sizeof(ShGuiKeyEvents)
	);

	memcpy(
		p_gui->inputs.last.last_mouse_events,
		p_gui->inputs.p_mouse_events,
		sizeof(ShGuiMouseEvents)
	);

	return 1;
}

uint8_t shGuiRelease(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	shGuiReleaseMemory(p_gui);
	shGuiDestroyPipelineResources(p_gui);
	shGuiDestroyPipelines(p_gui);

	shGuiError(p_gui->region_infos.p_regions_raw                    == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_menus_region_indices           == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_windows_region_indices         == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_windows_used_height            == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_regions_raw_write_flags        == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_regions_clicked                == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_cursor_on_regions              == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_moving_regions                 == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_right_resizing_regions         == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_left_resizing_regions          == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_top_resizing_regions           == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_bottom_resizing_regions        == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_windows_slider_buttons_offsets == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->region_infos.p_bottom_resizing_regions        == VK_NULL_HANDLE, "invalid memory", return 0);

	free(p_gui->region_infos.p_regions_raw);
	free(p_gui->region_infos.p_menus_region_indices);
	free(p_gui->region_infos.p_windows_region_indices);
	free(p_gui->region_infos.p_windows_used_height);
	free(p_gui->region_infos.p_regions_raw_write_flags);
	free(p_gui->region_infos.p_regions_clicked);
	free(p_gui->region_infos.p_cursor_on_regions);
	free(p_gui->region_infos.p_moving_regions);
	free(p_gui->region_infos.p_right_resizing_regions);
	free(p_gui->region_infos.p_left_resizing_regions);
	free(p_gui->region_infos.p_top_resizing_regions);
	free(p_gui->region_infos.p_bottom_resizing_regions);
	free(p_gui->region_infos.p_windows_slider_buttons_offsets);

	shGuiError(p_gui->char_infos.p_chars_raw             == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->char_infos.p_chars_raw_write_flags == VK_NULL_HANDLE, "invalid memory", return 0);
	shGuiError(p_gui->char_infos.p_chars_vertex_raw      == VK_NULL_HANDLE, "invalid memory", return 0);

	free(p_gui->char_infos.p_chars_raw);
	free(p_gui->char_infos.p_chars_raw_write_flags);
	free(p_gui->char_infos.p_chars_vertex_raw);

	return 1;
}

uint8_t shGuiLinkInputs(
	ShGui*           p_gui,
	uint32_t*        p_window_width, 
	uint32_t*        p_window_height, 
	float*           p_cursor_pos_x, 
	float*           p_cursor_pos_y, 
	ShGuiKeyEvents   key_events, 
	ShGuiMouseEvents mouse_events, 
	double*          p_delta_time
) {
	shGuiError(p_gui           == VK_NULL_HANDLE, "invalid gui memory",           return 0);
	shGuiError(p_window_width  == VK_NULL_HANDLE, "invalid window width memory",  return 0);
	shGuiError(p_window_height == VK_NULL_HANDLE, "invalid window height memory", return 0);
	shGuiError(p_cursor_pos_x  == VK_NULL_HANDLE, "invalid cursor pos x memory",  return 0);
	shGuiError(p_cursor_pos_y  == VK_NULL_HANDLE, "invalid cursor pos y memory",  return 0);
	shGuiError(key_events      == VK_NULL_HANDLE, "invalid key events memory",    return 0);
	shGuiError(mouse_events    == VK_NULL_HANDLE, "invalid mouse events memory",  return 0);
	shGuiError(p_delta_time    == VK_NULL_HANDLE, "invalid delta time memory",    return 0);

	ShGuiInputs inputs = {
		p_window_width,
		p_window_height,
		p_cursor_pos_x,
		p_cursor_pos_y,
		(int8_t*)key_events,
		(int8_t*)mouse_events,
		0,
		p_delta_time
	};

	p_gui->inputs = inputs;

	return 1;
}

uint8_t shGuiResetInputs(
	ShGui* p_gui
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	p_gui->inputs.active_cursor = SH_GUI_CURSOR_NORMAL;

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

	memcpy(p_gui->inputs.last.last_key_events,   p_gui->inputs.p_key_events, sizeof(ShGuiKeyEvents));
	memcpy(p_gui->inputs.last.last_mouse_events, p_gui->inputs.p_mouse_events, sizeof(ShGuiMouseEvents));

	return 1;
}

uint8_t shGuiSetDefaultValues(
	ShGui*                  p_gui,
	shguivec4*              p_default_region_color,
	shguivec4*              p_default_region_edge_color,
	shguivec4*              p_default_char_color,
	ShGuiDefaultValuesFlags flags
) {
	shGuiError(p_gui == NULL,                           "invalid gui memory",           return 0);
	shGuiError(flags >= SH_GUI_DEFAULT_VALUES_MAX_ENUM, "invalid default values flags", return 0);

	p_gui->default_values.flags = flags;

	shguivec4* p_dst_default_region_color      = &p_gui->default_values.default_region_raw_values.color;
	shguivec4* p_dst_default_region_edge_color = &p_gui->default_values.default_region_raw_values.edge_color;
	shguivec4* p_dst_default_char_color        = &p_gui->default_values.default_char_raw_values.color;

	if (flags == SH_GUI_NO_THEME) {
		
		shGuiError(
			p_default_region_color == VK_NULL_HANDLE,
			"invalid default region color memory",
			return 0
		);

		shGuiError(
			p_default_region_edge_color == VK_NULL_HANDLE,
			"invalid default region edge color memory",
			return 0
		);

		shGuiError(
			p_default_char_color == VK_NULL_HANDLE,
			"invalid default char color memory",
			return 0
		);

		memcpy(p_dst_default_region_color,      p_default_region_color,      sizeof(shguivec4));
		memcpy(p_dst_default_region_edge_color, p_default_region_edge_color, sizeof(shguivec4));
		memcpy(p_dst_default_char_color,        p_default_char_color,        sizeof(shguivec4));
	}
	else {
		switch (flags) {
		case SH_GUI_THEME_DARK:
			p_dst_default_region_color->x      = 0.05f;
			p_dst_default_region_color->y      = 0.05f;
			p_dst_default_region_color->z      = 0.05f;

			p_dst_default_region_edge_color->x = 0.7f;
			p_dst_default_region_edge_color->y = 0.7f;
			p_dst_default_region_edge_color->z = 0.7f;

			p_dst_default_char_color->x        = 1.0f;
			p_dst_default_char_color->y        = 1.0f;
			p_dst_default_char_color->z        = 1.0f;
			break;
		case SH_GUI_THEME_EXTRA_DARK:
			p_dst_default_region_color->x      = 0.01f;
			p_dst_default_region_color->y      = 0.01f;
			p_dst_default_region_color->z      = 0.01f;

			p_dst_default_region_edge_color->x = 0.7f;
			p_dst_default_region_edge_color->y = 0.7f;
			p_dst_default_region_edge_color->z = 0.7f;

			p_dst_default_char_color->x        = 1.0f;
			p_dst_default_char_color->y        = 1.0f;
			p_dst_default_char_color->z        = 1.0f;
			break;
		case SH_GUI_THEME_LIGHT:
			p_dst_default_region_color->x      = 0.8f;
			p_dst_default_region_color->y      = 0.8f;
			p_dst_default_region_color->z      = 0.8f;

			p_dst_default_region_edge_color->x = 0.4f;
			p_dst_default_region_edge_color->y = 0.4f;
			p_dst_default_region_edge_color->z = 0.4f;

			p_dst_default_char_color->x        = 0.0f;
			p_dst_default_char_color->y        = 0.0f;
			p_dst_default_char_color->z        = 0.0f;
			break;
		}
	}

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
	p_gui->char_infos.char_scale_factor     = region_scale_factor;

	return 1;
}

uint8_t shGuiRegion(
	ShGui*           p_gui,
	shguivec2        first_position,
	shguivec2        first_scale,
	shguivec4        first_color,
	shguivec4        first_edge_color,
	ShGuiMouseButton move_mouse_button, 
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags flags,
	ShGuiInputFlags  input_flags
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float          cursor_x       = *p_gui->inputs.p_cursor_pos_x;
	float          cursor_y       = *p_gui->inputs.p_cursor_pos_y;
		           				  
	float          window_size_x  = (float)(*p_gui->inputs.p_window_width);
	float          window_size_y  = (float)(*p_gui->inputs.p_window_height);

	uint32_t        region_count  =  p_gui->region_infos.region_count;
	ShGuiRegionRaw* p_region_raw  = &p_gui->region_infos.p_regions_raw[region_count];

	ShGuiRegionRaw region_raw = {
		first_position,
		first_scale,
		first_color,
		(shguivec3) { first_edge_color.x, first_edge_color.y, first_edge_color.z },
		SH_GUI_REGION_PRIORITY
	};

	if (flags & SH_GUI_PIXELS) {
		//fine
	}

	else if (flags & SH_GUI_RELATIVE) {
		region_raw.scale.x    = first_scale.x    / 100.0f * window_size_x * p_gui->region_infos.region_scale_factor;
		region_raw.scale.y    = first_scale.y    / 100.0f * window_size_y * p_gui->region_infos.region_scale_factor;
		region_raw.position.x = first_position.x / 100.0f * window_size_x / 2.0f;
		region_raw.position.y = first_position.y / 100.0f * window_size_y / 2.0f;
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

	ShGuiRegionRawWriteFlags* p_region_written = &p_gui->region_infos.p_regions_raw_write_flags[p_gui->region_infos.region_count];

	if ((*p_region_written) == 0) {
		(*p_region_raw) = region_raw;
	}

	uint8_t resizing_left   = 0;
	uint8_t resizing_right  = 0;
	uint8_t resizing_top    = 0;
	uint8_t resizing_bottom = 0;

	float delta_time = (float)*p_gui->inputs.p_delta_time;
	float priority   = p_region_raw->priority;

	if (flags & SH_GUI_RESIZABLE) {
		float limit_left         = p_region_raw->position.x - p_region_raw->scale.x / 2.0f;
		float limit_right        = p_region_raw->position.x + p_region_raw->scale.x / 2.0f;
		float limit_top          = p_region_raw->position.y - p_region_raw->scale.y / 2.0f;
		float limit_bottom       = p_region_raw->position.y + p_region_raw->scale.y / 2.0f;

		uint8_t horizontal_left  = (cursor_x >= limit_left   -  SH_GUI_CURSOR_EDGE_CHECK_SIZE && cursor_x <= limit_left   + SH_GUI_CURSOR_EDGE_CHECK_SIZE) &&
		                           (cursor_y <= limit_bottom && cursor_y >= limit_top);
		uint8_t horizontal_right = (cursor_x <= limit_right	 +  SH_GUI_CURSOR_EDGE_CHECK_SIZE && cursor_x >= limit_right  - SH_GUI_CURSOR_EDGE_CHECK_SIZE) &&
		                           (cursor_y <= limit_bottom && cursor_y >= limit_top);
		uint8_t vertical_top     = (cursor_y >= limit_bottom -  SH_GUI_CURSOR_EDGE_CHECK_SIZE && cursor_y <= limit_bottom + SH_GUI_CURSOR_EDGE_CHECK_SIZE) &&
                                   (cursor_x >= limit_left   && cursor_x <= limit_right);
		uint8_t vertical_bottom  = (cursor_y <= limit_top    +  SH_GUI_CURSOR_EDGE_CHECK_SIZE && cursor_y >= limit_top    - SH_GUI_CURSOR_EDGE_CHECK_SIZE) &&
                                   (cursor_x >= limit_left   && cursor_x <= limit_right);


		float d_cursor_pos_x     = (*p_gui->inputs.p_cursor_pos_x) - p_gui->inputs.last.last_cursor_pos_x;
		float d_cursor_pos_y     = (*p_gui->inputs.p_cursor_pos_y) - p_gui->inputs.last.last_cursor_pos_y;
		

		if (horizontal_left || horizontal_right) {
			p_gui->inputs.active_cursor = SH_GUI_CURSOR_HORIZONTAL_RESIZE;
		}
		else if (vertical_top || vertical_bottom) {
			p_gui->inputs.active_cursor = SH_GUI_CURSOR_VERTICAL_RESIZE;
		}
		
		if (p_gui->inputs.p_mouse_events[0] && delta_time < SH_GUI_MAX_TOLERATED_DELTA_TIME) {

			float dx = SH_GUI_WINDOW_CURSOR_RESIZE_SPEED * d_cursor_pos_x * delta_time;//to left is negative
			float dy = SH_GUI_WINDOW_CURSOR_RESIZE_SPEED * d_cursor_pos_y * delta_time;//to bottom is negative

			uint8_t width_ok                 = p_region_raw->scale.x > SH_GUI_MIN_REGION_WIDTH;
			uint8_t height_ok                = p_region_raw->scale.y > SH_GUI_MIN_REGION_HEIGHT;
							                
			resizing_left                    = horizontal_left  || p_gui->region_infos.p_left_resizing_regions[region_count];
			resizing_right                   = horizontal_right || p_gui->region_infos.p_right_resizing_regions[region_count];
			resizing_top                     = vertical_top     || p_gui->region_infos.p_top_resizing_regions[region_count];
			resizing_bottom                  = vertical_bottom  || p_gui->region_infos.p_bottom_resizing_regions[region_count];

			uint8_t left_confirm_to_left     = (dx < 0.0f);
			uint8_t left_confirm_to_right    = (dx > 0.0f && width_ok);

			uint8_t right_confirm_to_left    = (dx < 0.0f && width_ok);
			uint8_t right_confirm_to_right   = (dx > 0.0f);

			uint8_t top_confirm_to_top       = (dy > 0.0f);
			uint8_t top_confirm_to_bottom    = (dy < 0.0f && height_ok);

			uint8_t bottom_confirm_to_top    = (dy > 0.0f && height_ok);
			uint8_t bottom_confirm_to_bottom = (dy < 0.0f);


			uint8_t confirm_to_top           = dy < 0.0f && p_region_raw->scale.y < SH_GUI_MIN_REGION_HEIGHT;
			uint8_t confirm_to_bottom        = dy < 0.0f && p_region_raw->scale.y < SH_GUI_MIN_REGION_HEIGHT;

			if (resizing_left && left_confirm_to_right) {
				p_region_raw->scale.x -= dx;
				(*p_region_written) |= SH_GUI_REGION_RAW_X_SCALE;
				p_gui->region_infos.p_left_resizing_regions[region_count] = 1;
			}
			else if (resizing_left && left_confirm_to_left) {
				p_region_raw->scale.x -= dx;
				(*p_region_written) |= SH_GUI_REGION_RAW_X_SCALE;
				p_gui->region_infos.p_left_resizing_regions[region_count] = 1;
			}

			if (resizing_right && right_confirm_to_left) {
				p_region_raw->scale.x += dx;
				(*p_region_written) |= SH_GUI_REGION_RAW_X_SCALE;
				p_gui->region_infos.p_right_resizing_regions[region_count] = 1;
			}
			else if (resizing_right && right_confirm_to_right) {
				p_region_raw->scale.x += dx;
				(*p_region_written) |= SH_GUI_REGION_RAW_X_SCALE;
				p_gui->region_infos.p_right_resizing_regions[region_count] = 1;
			}

			if (resizing_top && top_confirm_to_top) {
				p_region_raw->scale.y += dy;
				(*p_region_written) |= SH_GUI_REGION_RAW_Y_SCALE;
				p_gui->region_infos.p_top_resizing_regions[region_count] = 1;
			}
			else if (resizing_top && top_confirm_to_bottom) {
				p_region_raw->scale.y += dy;
				(*p_region_written) |= SH_GUI_REGION_RAW_Y_SCALE;
				p_gui->region_infos.p_top_resizing_regions[region_count] = 1;
			}

			if (resizing_bottom && bottom_confirm_to_top) {
				p_region_raw->scale.y -= dy;
				(*p_region_written) |= SH_GUI_REGION_RAW_Y_SCALE;
				p_gui->region_infos.p_bottom_resizing_regions[region_count] = 1;
			}
			else if (resizing_bottom && bottom_confirm_to_bottom) {
				p_region_raw->scale.y -= dy;
				(*p_region_written) |= SH_GUI_REGION_RAW_Y_SCALE;
				p_gui->region_infos.p_bottom_resizing_regions[region_count] = 1;
			}
			
		}
		else {
			p_gui->region_infos.p_left_resizing_regions   [region_count] = 0;
			p_gui->region_infos.p_right_resizing_regions  [region_count] = 0;
			p_gui->region_infos.p_top_resizing_regions    [region_count] = 0;
			p_gui->region_infos.p_bottom_resizing_regions [region_count] = 0;
		}
	}

	uint8_t* p_clicked = &p_gui->region_infos.p_regions_clicked[p_gui->region_infos.region_count];

	ShGuiRegionRaw region_write_src = {
		(shguivec2){ 0 },
		(shguivec2){ 0 },
		(shguivec4){ 0 },
		(shguivec3){ 0 },
		SH_GUI_SELECTED_REGION_PRIORITY
	};

	if (
	!resizing_left && !resizing_right && !resizing_top && !resizing_bottom    &&
	(
		(cursor_x >= p_region_raw->position.x - p_region_raw->scale.x / 2.0f) &&
		(cursor_x <= p_region_raw->position.x + p_region_raw->scale.x / 2.0f) &&
		(cursor_y >= p_region_raw->position.y - p_region_raw->scale.y / 2.0f) &&
		(cursor_y <= p_region_raw->position.y + p_region_raw->scale.y / 2.0f)
	) ||
		p_gui->region_infos.p_moving_regions[region_count]
	) {
		p_gui->inputs.active_cursor                           = SH_GUI_CURSOR_NORMAL;
		p_gui->region_infos.p_cursor_on_regions[region_count] = 1;

		if (
			(p_gui->inputs.p_mouse_events[move_mouse_button] == 1) &&
			delta_time < SH_GUI_MAX_TOLERATED_DELTA_TIME
			) {

			shGuiOverwriteRegion(p_gui, region_count, &region_write_src, SH_GUI_REGION_RAW_PRIORITY);

			if (flags & SH_GUI_X_MOVABLE) {
				//make it cleaner pls
				float dx = cursor_x - p_gui->inputs.last.last_cursor_pos_x;
				if (dx < 0.0f) {
					if (offset_region_scale.x == SH_GUI_VEC2_ZERO.x || p_region_raw->position.x > (offset_region_center.x - offset_region_scale.x)) {
						p_region_raw->position.x += dx;
					}
				}
				else if (offset_region_scale.x == SH_GUI_VEC2_ZERO.x || p_region_raw->position.x < (offset_region_center.x + offset_region_scale.x)) {
					p_region_raw->position.x += dx;
				}
				(*p_region_written) |= SH_GUI_REGION_RAW_X_POSITION;
				p_gui->region_infos.p_moving_regions[region_count] = 1;
			}

			if (flags & SH_GUI_Y_MOVABLE) {
				float dy = cursor_y - p_gui->inputs.last.last_cursor_pos_y;
				//make it cleaner please
				if (dy < 0.0f) {
					if (offset_region_scale.y == SH_GUI_VEC2_ZERO.y || p_region_raw->position.y > -offset_region_scale.y) {
						p_region_raw->position.y += dy;
					}
				}
				else if (offset_region_scale.y == SH_GUI_VEC2_ZERO.y || p_region_raw->position.y < offset_region_scale.y) {
					p_region_raw->position.y += dy;
				}
				(*p_region_written) |= SH_GUI_REGION_RAW_Y_POSITION;
				p_gui->region_infos.p_moving_regions[region_count] = 1;
			}

		}
		else {
			p_gui->region_infos.p_moving_regions[region_count] = 0;
		}
		if (p_gui->inputs.p_mouse_events[0] == 1) {

			shGuiOverwriteRegion(p_gui, region_count, &region_write_src, SH_GUI_REGION_RAW_PRIORITY);

			if (input_flags & SH_GUI_PRESS_ONCE) {
				if (p_gui->inputs.last.last_mouse_events[0] == 0) {
					uint8_t rtrn = (*p_clicked) == 0;
					(*p_clicked) = 1;
					p_gui->region_infos.region_count++;
					return rtrn;
				}
			}
			else {
				(*p_clicked) = 1;
				p_gui->region_infos.region_count++;
				return *p_clicked;
			}
		}
	}
	else {
		p_gui->region_infos.p_cursor_on_regions[region_count] = 0;
	}

	(*p_clicked) = 0;
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

	ShGuiRegionRaw*           p_region_raw             = &p_gui->region_infos.p_regions_raw            [region_idx];
	ShGuiRegionRawWriteFlags* p_region_raw_write_flags = &p_gui->region_infos.p_regions_raw_write_flags[region_idx];

	(*p_region_raw_write_flags) |= flags;

	if (flags & SH_GUI_REGION_RAW_X_POSITION) {
		p_region_raw->position.x = p_src_data->position.x;
	}
	if (flags & SH_GUI_REGION_RAW_Y_POSITION) {
		p_region_raw->position.y = p_src_data->position.y;
	}
	if (flags & SH_GUI_REGION_RAW_SCALE) {
		p_region_raw->scale.x    = p_src_data->scale.x;
	}
	if (flags & SH_GUI_REGION_RAW_Y_SCALE) {
		p_region_raw->scale.y    = p_src_data->scale.y;
	}
	if (flags & SH_GUI_REGION_RAW_COLOR) {
		p_region_raw->color      = p_src_data->color;
	}
	if (flags & SH_GUI_REGION_RAW_EDGE_COLOR) {
		p_region_raw->edge_color = p_src_data->edge_color;
	}
	if (flags & SH_GUI_REGION_RAW_PRIORITY) {
		p_region_raw->priority   = p_src_data->priority;
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
		first_region + region_count > p_gui->region_infos.max_region_count,
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
	shguivec2        first_position, 
	shguivec4        first_color,
	float            first_scale,
	char*            s_text,
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui        == NULL, "invalid gui memory",       return 0);
	shGuiError(s_text       == NULL, "invalid text memory",      return 0);
	shGuiError(first_scale  == 0.0f, "invalid text scale value", return 0)

	float               window_size_x      = (float)(*p_gui->inputs.p_window_width);
	float               window_size_y      = (float)(*p_gui->inputs.p_window_height);

	ShGuiCharRaw*       p_chars_raw        = p_gui->char_infos.p_chars_raw;
	ShGuiCharVertexRaw* p_chars_vertex_raw = p_gui->char_infos.p_chars_vertex_raw;

	float               x_offset           = 0.0f;
	float               y_offset           = 0.0f;

	float               max_x_offset       = x_offset;
	float               min_y_offset       = y_offset;

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		
		ShGuiCharRawWriteFlags char_written = p_gui->char_infos.p_chars_raw_write_flags[char_idx];

		uint32_t      char_count        =  p_gui->char_infos.char_count;
		ShGuiCharRaw* p_char_raw        = &p_gui->char_infos.p_chars_raw       [char_count];
		float*        p_char_vertex_raw =  p_gui->char_infos.p_chars_vertex_raw[char_count];
		
		if ((char_written & SH_GUI_CHAR_RAW_POSITION) == 0) {
			p_char_raw->position.x = first_position.x + x_offset;
			p_char_raw->position.y = first_position.y + y_offset;
		}
		if ((char_written & SH_GUI_CHAR_RAW_SCALE) == 0) {
			p_char_raw->scale = first_scale;
		}
		if ((char_written & SH_GUI_CHAR_RAW_PRIORITY) == 0) {
			p_char_raw->priority = SH_GUI_TEXT_PRIORITY;
		}
		if ((char_written & SH_GUI_CHAR_RAW_COLOR) == 0) {
			p_char_raw->color = first_color;
		}

		x_offset += SH_GUI_CHAR_X_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, first_scale);
		if (x_offset > max_x_offset) {
			max_x_offset = x_offset;
		}

		switch (s_text[char_idx]) {
		case 'q':
		case 'Q':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigQ_vertices);
			break;
		case 'w':
		case 'W':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigW_vertices);
			break;
		case 'e':
		case 'E':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigE_vertices);
			break;
		case 'r':
		case 'R':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigR_vertices);
			break;
		case 't':
		case 'T':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigT_vertices);
			break;
		case 'y':
		case 'Y':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigY_vertices);
			break;
		case 'u':
		case 'U':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigU_vertices);
			break;
		case 'i':
		case 'I':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigI_vertices);
			break;
		case 'o':
		case 'O':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigO_vertices);
			break;
		case 'p':
		case 'P':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigP_vertices);
			break;
		case 'a':
		case 'A':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigA_vertices);
			break;
		case 's':
		case 'S':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigS_vertices);
			break;
		case 'd':
		case 'D':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigD_vertices);
			break;
		case 'f':
		case 'F':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigF_vertices);
			break;
		case 'g':
		case 'G':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigG_vertices);
			break;
		case 'h':
		case 'H':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigH_vertices);
			break;
		case 'j':
		case 'J':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigJ_vertices);
			break;
		case 'k':
		case 'K':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigK_vertices);
			break;
		case 'l':
		case 'L':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigL_vertices);
			break;
		case 'z':
		case 'Z':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigZ_vertices);
			break;
		case 'x':
		case 'X':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigX_vertices);
			break;
		case 'c':
		case 'C':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigC_vertices);
			break;
		case 'v':
		case 'V':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigV_vertices);
			break;
		case 'b':
		case 'B':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigB_vertices);
			break;
		case 'n':
		case 'N':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigN_vertices);
			break;
		case 'm':
		case 'M':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_bigM_vertices);
			break;
		case '\\':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_backslash_vertices);
			break;
		case '1':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_1_vertices);
			break;
		case '2':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_2_vertices);
			break;
		case '3':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_3_vertices);
			break;
		case '4':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_4_vertices);
			break;
		case '5':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_5_vertices);
			break;
		case '6':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_6_vertices);
			break;
		case '7':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_7_vertices);
			break;
		case '8':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_8_vertices);
			break;
		case '9':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_9_vertices);
			break;
		case '0':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_0_vertices);
			break;
		case '\'':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_quote_vertices);
			break;
		case '|':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_stick_vertices);
			break;
		case '!':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_exclamation_vertices);
			break;
		case '"':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_double_quote_vertices);
			break;
		case '£':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_pound_vertices);
			break;
		case '$':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_dollar_vertices);
			break;
		case '%':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_percent_vertices);
			break;
		case '&':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_and_vertices);
			break;
		case '/':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_frontslash_vertices);
			break;
			//case '(':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_open_round_bracket_vertices);
			//	break;
			//case ')':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_close_round_bracket_vertices);
			//	break;
		case '=':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_equal_vertices);
			break;
		case '?':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_question_mark_vertices);
			break;
		case '+':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_plus_vertices);
			break;
		case ',':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_comma_vertices);
			break;
		case '.':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_dot_vertices);
			break;
		case '-':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_dash_vertices);
			break;
		case '*':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_star_vertices);
			break;
		case ';':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_semicolon_vertices);
			break;
		case ':':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_colon_vertices);
			break;
		case '_':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_underscore_vertices);
			break;
			//case '[':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_open_round_bracket_vertices);
			//	break;
			//case ']':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_close_round_bracket_vertices);
			//	break;
		case '@':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_at_vertices);
			break;
		case '#':
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_hash_vertices);
			break;
			//case '{':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_open_curly_bracket_vertices);
			//	break;
			//case '}':
			//	SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, consolas_close_curly_bracket_vertices);
			//	break;
		case '\n':
			x_offset  = 0.0f;
			y_offset -= p_char_raw->scale;
			if (y_offset < min_y_offset) {
				min_y_offset = y_offset;
			}
		default:
			SH_GUI_LOAD_CHAR_VERTICES(p_char_vertex_raw, SH_GUI_NO_CHAR);
			break;
		}

		p_gui->char_infos.char_count++;
	}

	for (uint32_t char_idx = 0; char_idx < strlen(s_text); char_idx++) {
		
		ShGuiCharRaw* p_char_raw = &p_gui->char_infos.p_chars_raw[char_idx];
		
		if (flags & SH_GUI_CENTER_WIDTH) {
			p_char_raw->position.x -= max_x_offset / 2.0f;
		}
		if (flags & SH_GUI_EDGE_LEFT) {
			p_char_raw->position.x -= (window_size_x - max_x_offset) / 2.0f - SH_GUI_CHAR_DISTANCE_OFFSET;
		}
		if (flags & SH_GUI_EDGE_RIGHT) {
			p_char_raw->position.x += (window_size_x - max_x_offset) / 2.0f - SH_GUI_CHAR_DISTANCE_OFFSET;
		}

		if (flags & SH_GUI_CENTER_HEIGHT) {
			p_char_raw->position.y -= min_y_offset / 2.0f - SH_GUI_CHAR_DISTANCE_OFFSET;
		}
		if (flags & SH_GUI_EDGE_TOP) {
			p_char_raw->position.y += (window_size_y + min_y_offset) / 2.0f - SH_GUI_CHAR_DISTANCE_OFFSET;
		}
		if (flags & SH_GUI_EDGE_BOTTOM) {
			p_char_raw->position.y -= window_size_y / 2.0f + min_y_offset - SH_GUI_CHAR_DISTANCE_OFFSET;
		}

	}

	return 1;
}

uint8_t shGuiColorMatrix(
	ShGui*     p_gui,
	uint32_t   matrix_width,
	uint32_t   matrix_height,
	shguivec4* p_colors
) {
	shGuiError(p_gui         == NULL, "invalid gui memory",    return 0);
	shGuiError(p_colors      == NULL, "invalid colors memory", return 0);
	shGuiError(matrix_width  == 0,    "invalid matrix width",  return 0);
	shGuiError(matrix_height == 0,    "invalid matrix height", return 0);
	


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

	ShGuiCharRaw*           p_char_raw             = &p_gui->char_infos.p_chars_raw            [char_idx];
	ShGuiCharRawWriteFlags* p_char_raw_write_flags = &p_gui->char_infos.p_chars_raw_write_flags[char_idx];

	(*p_char_raw_write_flags) |= flags;

	if (flags & SH_GUI_CHAR_RAW_POSITION) {
		p_char_raw->position = p_src_data->position;
	}
	if (flags & SH_GUI_CHAR_RAW_SCALE) {
		p_char_raw->scale    = p_src_data->scale;
	}
	if (flags & SH_GUI_CHAR_RAW_PRIORITY) {
		p_char_raw->priority = p_src_data->priority;
	}
	if (flags & SH_GUI_CHAR_RAW_COLOR) {
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
		first_char + char_count > p_gui->char_infos.max_char_count,
		"invalid char range",
		return 0
	);

	for (uint32_t char_idx = first_char; char_idx < (first_char + char_count); char_idx++) {
		shGuiOverwriteChar(p_gui, char_idx, p_src_data, flags);
	}

	return 1;
}

uint8_t shGuiItem(
	ShGui*           p_gui, 
	shguivec2        position,
	shguivec2        scale,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	ShGuiMouseButton move_mouse_button,
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags region_flags,
	ShGuiInputFlags  input_flags,
	char*            text,
	float            text_scale,
	shguivec4        text_color
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t        region_idx   =  p_gui->region_infos.region_count;
	ShGuiRegionRaw*	p_region_raw = &p_gui->region_infos.p_regions_raw[region_idx];

	uint8_t pressed = shGuiRegion(
		p_gui,
		position,
		scale,
		region_color,
		region_edge_color,
		move_mouse_button,
		offset_region_center,
		offset_region_scale,
		region_flags,
		input_flags
	);

	if (text != VK_NULL_HANDLE) {

		uint32_t line_count = 0;
		shGuiTextLineCount(
			text, &line_count
		);

		shguivec2 text_position = {
			p_region_raw->position.x - SH_GUI_CHAR_FINAL_X_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, text_scale, strlen(text)) / 2.0f + SH_GUI_CHAR_DISTANCE_OFFSET / 2.0f,
			p_region_raw->position.y - SH_GUI_CHAR_FINAL_Y_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, line_count, text_scale)   / 2.0f + SH_GUI_CHAR_DISTANCE_OFFSET,
		};

		shGuiText(
			p_gui,
			text_position,
			text_color,
			text_scale,
			text,
			0
		);
	}

	return pressed;
}

uint8_t shGuiWindow(
	ShGui*           p_gui, 
	shguivec2        position,
	shguivec2        scale,
	shguivec4        window_color,
	shguivec4        window_edge_color,
	shguivec4        bar_color,
	shguivec4        bar_edge_color,
	ShGuiMouseButton move_mouse_button,
	shguivec2        offset_region_center,
	shguivec2        offset_region_scale,
	ShGuiWidgetFlags region_flags,
	char*            title,
	shguivec4        title_color
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);
	
	uint32_t        window_region_idx   =  p_gui->region_infos.region_count;
	ShGuiRegionRaw* p_window_region_raw = &p_gui->region_infos.p_regions_raw[window_region_idx];
								        
	uint32_t        window_count        = p_gui->region_infos.window_count;
	uint32_t*       p_window_region_idx = &p_gui->region_infos.p_windows_region_indices[window_count];

	(*p_window_region_idx)              = window_region_idx;

	float window_bar_height = SH_GUI_WINDOW_BAR_HEIGHT * p_gui->char_infos.char_scale_factor;

	ShGuiWidgetFlags window_flags = region_flags;
	ShGuiWidgetFlags bar_flags    = SH_GUI_PIXELS;

	shguivec2 window_region_scale = {
		scale.x,
		(window_flags & SH_GUI_RELATIVE) ?
			(scale.y - window_bar_height / ((float)*p_gui->inputs.p_window_height) * 100.0f) :
			(scale.y - window_bar_height)
	};

	if (window_flags &   SH_GUI_MOVABLE) {
		window_flags &= ~SH_GUI_MOVABLE;
		bar_flags    |=  SH_GUI_MOVABLE;
	}

	uint8_t window_pressed = shGuiRegion(
		p_gui,
		position,
		window_region_scale,
		window_color,
		window_edge_color,
		move_mouse_button,
		offset_region_center,
		offset_region_scale,
		window_flags,
		0
	);

	uint32_t bar_region_idx          =  p_gui->region_infos.region_count;
	ShGuiRegionRaw* p_bar_region_raw = &p_gui->region_infos.p_regions_raw[bar_region_idx];

	shguivec2 bar_region_position = {
		p_window_region_raw->position.x,
		p_window_region_raw->position.y + p_window_region_raw->scale.y / 2.0f + window_bar_height / 2.0f
	};
	shguivec2 bar_region_scale = {
		p_window_region_raw->scale.x,
		window_bar_height
	};

	uint32_t first_char_idx = p_gui->char_infos.char_count;

	uint8_t bar_pressed = shGuiItem(
		p_gui,
		bar_region_position,
		bar_region_scale,
		bar_color,
		bar_edge_color,
		SH_GUI_LEFT_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		bar_flags,
		0,
		title,
		SH_GUI_WINDOW_TEXT_SIZE * p_gui->char_infos.char_scale_factor,
		title_color
	);

	p_bar_region_raw->scale.x       = p_window_region_raw->scale.x;
	p_window_region_raw->position.x = p_bar_region_raw->position.x;
	p_window_region_raw->position.y = p_bar_region_raw->position.y - p_window_region_raw->scale.y / 2.0f - window_bar_height / 2.0f;

	if (bar_pressed || window_pressed) {
		
		ShGuiRegionRaw regions_write_src = {
			(shguivec2){ 0 },
			(shguivec2){ 0 },
			(shguivec4){ 0 },
			(shguivec3){ 0 },
			SH_GUI_REGION_PRIORITY
		};

		ShGuiCharRaw chars_write_src = {
			(shguivec2){ 0 },
			0.0f,
			SH_GUI_TEXT_PRIORITY,
			(shguivec4){ 0 }
		};

		shGuiOverwriteRegions(
			p_gui,
			0,
			p_gui->region_infos.max_region_count,
			&regions_write_src,
			SH_GUI_REGION_RAW_PRIORITY
		);
		shGuiOverwriteChars(
			p_gui,
			0,
			p_gui->char_infos.max_char_count,
			&chars_write_src,
			SH_GUI_CHAR_RAW_PRIORITY
		);
		
		regions_write_src.priority = SH_GUI_SELECTED_REGION_PRIORITY;
		chars_write_src.priority   = SH_GUI_SELECTED_TEXT_PRIORITY;
		
		shGuiOverwriteRegion(
			p_gui,
			window_region_idx,
			&regions_write_src,
			SH_GUI_REGION_RAW_PRIORITY
		);
		shGuiOverwriteRegion(
			p_gui,
			bar_region_idx,
			&regions_write_src,
			SH_GUI_REGION_RAW_PRIORITY
		);
		shGuiOverwriteChars(
			p_gui,
			first_char_idx,
			(uint32_t)strlen(title),
			&chars_write_src,
			SH_GUI_CHAR_RAW_PRIORITY
		);
	}

	p_gui->region_infos.window_count++;

	return 1;
}

uint8_t shGuiWindowText(
	ShGui*           p_gui,
	shguivec4        color,
	float            scale,
	char*            text, 
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui == NULL, "invalid gui memory",  return 0);
	shGuiError(text  == NULL, "invalid text memory", return 0);

	uint32_t       window_count      = p_gui->region_infos.window_count;
	uint32_t       window_idx        = p_gui->region_infos.p_windows_region_indices[window_count - 1];
	uint32_t       last_region       = p_gui->region_infos.region_count - 1;

	ShGuiRegionRaw window_region_raw = p_gui->region_infos.p_regions_raw[window_idx];
	shguivec2      window_position   = window_region_raw.position;
	shguivec2      window_scale      = window_region_raw.scale;

	float*         p_used_height     = &p_gui->region_infos.p_windows_used_height[window_count - 1];

	uint32_t first_char_idx          = p_gui->char_infos.char_count;

	shguivec2 text_position = {
		window_position.x - window_scale.x / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET,
		window_position.y + window_scale.y / 2.0f - SH_GUI_WINDOW_BAR_HEIGHT - SH_GUI_WINDOW_TEXT_BORDER_OFFSET - (*p_used_height) - scale,
	};

	if (flags & SH_GUI_CENTER_WIDTH) {
		float chars_offset = SH_GUI_CHAR_FINAL_X_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, scale, strlen(text) - 1);
		text_position.x    = window_position.x - chars_offset / 2.0f;
	}

	(*p_used_height) += scale + SH_GUI_WINDOW_ITEMS_OFFSET;

	shGuiText(
		p_gui,
		text_position,
		color,
		scale,
		text,
		0
	);

	float char_priority   = SH_GUI_ITEM_TEXT_PRIORITY;

	if (window_region_raw.priority == SH_GUI_SELECTED_REGION_PRIORITY) {
		char_priority   = SH_GUI_SELECTED_ITEM_TEXT_PRIORITY;
	}

	ShGuiCharRaw chars_write_src = {
		(shguivec2){ 0 },
		0.0f,
		char_priority,
		(shguivec4){ 0 }
	};

	shGuiOverwriteChars(
		p_gui,
		first_char_idx,
		(uint32_t)strlen(text),
		&chars_write_src,
		SH_GUI_CHAR_RAW_PRIORITY
	);

	return 1;
}

uint8_t shGuiWindowButton(
	ShGui*           p_gui, 
	float            scale,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	char*            text,
	shguivec4        text_color,
	ShGuiWidgetFlags flags,
	ShGuiInputFlags  input_flags
) {
	shGuiError(p_gui == NULL, "invalid gui memmory", return 0);

	uint32_t       window_count      = p_gui->region_infos.window_count;
	uint32_t       window_idx        = p_gui->region_infos.p_windows_region_indices[window_count - 1];
	uint32_t       last_region       = p_gui->region_infos.region_count - 1;

	ShGuiRegionRaw window_region_raw = p_gui->region_infos.p_regions_raw[window_idx];
	shguivec2      window_position   = window_region_raw.position;
	shguivec2      window_scale      = window_region_raw.scale;

	float*         p_used_height     = &p_gui->region_infos.p_windows_used_height[window_count - 1];


	shguivec2 button_scale = {
		text != VK_NULL_HANDLE ? SH_GUI_CHAR_FINAL_X_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, scale, strlen(text)) + SH_GUI_WINDOW_TEXT_BORDER_OFFSET : 50.0f,
		scale + SH_GUI_WINDOW_TEXT_BORDER_OFFSET
	};

	shguivec2 button_position = {
		window_position.x - window_scale.x / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET + button_scale.x / 2.0f,
		SH_GUI_WINDOW_USED_HEIGHT(window_position.y, window_scale.y, *p_used_height, p_gui->region_infos.region_scale_factor) - button_scale.y / 2.0f,
	};

	(*p_used_height) += scale + SH_GUI_WINDOW_ITEMS_OFFSET;


	if (flags & SH_GUI_CENTER_WIDTH) {
		button_position.x = window_position.x;
	}

	uint32_t button_region_idx = p_gui->region_infos.region_count;

	uint32_t first_char_idx = p_gui->char_infos.char_count;

	uint8_t pressed = shGuiItem(
		p_gui,
		button_position,
		button_scale,
		region_color,
		region_edge_color,
		SH_GUI_NO_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		SH_GUI_PIXELS,
		input_flags,
		text,
		scale,
		text_color
	);

	ShGuiRegionRaw window_region = p_gui->region_infos.p_regions_raw[window_idx];

	float region_priority = SH_GUI_ITEM_REGION_PRIORITY;
	float char_priority   = SH_GUI_ITEM_TEXT_PRIORITY;

	if (window_region.priority == SH_GUI_SELECTED_REGION_PRIORITY) {
		region_priority = SH_GUI_SELECTED_ITEM_REGION_PRIORITY;
		char_priority   = SH_GUI_SELECTED_ITEM_TEXT_PRIORITY;
	}

	ShGuiRegionRaw region_write_src = {
		button_position,
		(shguivec2){ 0 },
		(shguivec4){ 0 },
		(shguivec3){ 0 },
		region_priority
	};

	ShGuiCharRaw chars_write_src = {
		(shguivec2){ 0 },
		0.0f,
		char_priority,
		(shguivec4){ 0 }
	};

	shGuiOverwriteRegions(
		p_gui,
		button_region_idx,
		1,
		&region_write_src,
		SH_GUI_REGION_RAW_POSITION | 
		SH_GUI_REGION_RAW_PRIORITY
	);
	if (text != VK_NULL_HANDLE) {
		shGuiOverwriteChars(
			p_gui,
			first_char_idx,
			(uint32_t)strlen(text),
			&chars_write_src,
			SH_GUI_CHAR_RAW_PRIORITY
		);
	}

	return pressed;
}

uint8_t shGuiWindowSeparator(
	ShGui*    p_gui,
	shguivec4 color
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	uint32_t       window_count      = p_gui->region_infos.window_count;
	uint32_t       window_idx        = p_gui->region_infos.p_windows_region_indices[window_count - 1];
	uint32_t       last_region       = p_gui->region_infos.region_count - 1;

	ShGuiRegionRaw window_region_raw = p_gui->region_infos.p_regions_raw[window_idx];
	shguivec2      window_position   = window_region_raw.position;
	shguivec2      window_scale      = window_region_raw.scale;

	float*         p_used_height     = &p_gui->region_infos.p_windows_used_height[window_count - 1];

	shguivec2 position = {
		window_position.x,
		SH_GUI_WINDOW_USED_HEIGHT(window_position.y, window_scale.y, *p_used_height, p_gui->region_infos.region_scale_factor) - SH_GUI_SEPARATOR_OFFSET
	};

	shguivec2 scale = {
		SH_GUI_WINDOW_SEPARATOR_EXTENT(window_scale.x),
		2.0f
	};

	(*p_used_height) += scale.y + SH_GUI_WINDOW_ITEMS_OFFSET;

	uint32_t separator_region_idx = p_gui->region_infos.region_count;

	shGuiRegion(
		p_gui,
		position,
		scale,
		color,
		color,
		SH_GUI_NO_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		SH_GUI_PIXELS,
		0
	);

	ShGuiRegionRaw window_region = p_gui->region_infos.p_regions_raw[window_idx];

	float region_priority = SH_GUI_ITEM_REGION_PRIORITY;

	if (window_region.priority == SH_GUI_SELECTED_REGION_PRIORITY) {
		region_priority = SH_GUI_SELECTED_ITEM_REGION_PRIORITY;
	}

	ShGuiRegionRaw region_write_src = {
		position,
		scale,
		(shguivec4){ 0 },
		(shguivec3){ 0 },
		region_priority
	};

	shGuiOverwriteRegions(
		p_gui,
		separator_region_idx,
		1,
		&region_write_src,
		SH_GUI_REGION_RAW_POSITION |
		SH_GUI_REGION_RAW_SCALE    |
		SH_GUI_REGION_RAW_PRIORITY
	);

	return 1;
}

uint8_t shGuiWindowSliderf(
	ShGui*           p_gui, 
	float            scale,
	float            min,
	float            max, 
	char*            hint, 
	shguivec4        rail_color,
	shguivec4        rail_edge_color,
	shguivec4        button_color,
	shguivec4        button_edge_color,
	shguivec4        text_color,
	float*           p_dst, 
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui == NULL, "invalid gui memory",         return 0);
	shGuiError(p_dst == NULL, "invalid destination memory", return 0);
	shGuiError(hint  == NULL, "invalid hint text memory",   return 0);

	uint32_t       window_count      = p_gui->region_infos.window_count;
	uint32_t       window_idx        = p_gui->region_infos.p_windows_region_indices[window_count - 1];
	uint32_t       last_region       = p_gui->region_infos.region_count - 1;

	ShGuiRegionRaw window_region_raw = p_gui->region_infos.p_regions_raw[window_idx];
	shguivec2      window_position   = window_region_raw.position;
	shguivec2      window_scale      = window_region_raw.scale;

	float*         p_used_height     = &p_gui->region_infos.p_windows_used_height[window_count - 1];


	shguivec2 button_scale = {
		hint != VK_NULL_HANDLE ? SH_GUI_CHAR_FINAL_X_OFFSET(SH_GUI_CHAR_DISTANCE_OFFSET, scale, strlen(hint)) + SH_GUI_WINDOW_TEXT_BORDER_OFFSET : 50.0f,
		scale
	};

	shguivec2 rail_scale = {
		SH_GUI_WINDOW_SLIDER_EXTENT(window_scale.x),
		2.0f
	};

	shguivec2 rail_position = {
		window_position.x - window_scale.x / 2.0f + SH_GUI_WINDOW_TEXT_BORDER_OFFSET + rail_scale.x / 2.0f,
		SH_GUI_WINDOW_USED_HEIGHT(window_position.y, window_scale.y, *p_used_height, p_gui->region_infos.region_scale_factor) - rail_scale.y / 2.0f
	};


	if (flags & SH_GUI_CENTER_WIDTH) {
		rail_position.x = window_position.x;
	}
	(*p_used_height) += scale + SH_GUI_WINDOW_ITEMS_OFFSET;

	uint32_t rail_region_idx = p_gui->region_infos.region_count;

	shGuiRegion(
		p_gui,
		rail_position,
		rail_scale,
		rail_color,
		rail_edge_color,
		SH_GUI_NO_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		SH_GUI_PIXELS,
		0
	);

	ShGuiRegionRaw window_region = p_gui->region_infos.p_regions_raw[window_idx];

	float region_priority = SH_GUI_ITEM_REGION_PRIORITY;
	float chars_priority  = SH_GUI_ITEM_TEXT_PRIORITY;

	if (window_region.priority == SH_GUI_SELECTED_REGION_PRIORITY) {
		region_priority = SH_GUI_SELECTED_ITEM_REGION_PRIORITY;
		chars_priority  = SH_GUI_SELECTED_ITEM_TEXT_PRIORITY;
	}

	ShGuiRegionRaw region_write_src = {
		rail_position,
		rail_scale,
		SH_GUI_VEC4_ZERO,
		SH_GUI_VEC3_ZERO,
		region_priority
	};

	ShGuiCharRaw chars_write_src = {
		SH_GUI_VEC2_ZERO,
		0.0f,
		chars_priority,
		SH_GUI_VEC4_ZERO
	};

	shGuiOverwriteRegions(
		p_gui,
		rail_region_idx,
		1,
		&region_write_src,
		SH_GUI_REGION_RAW_POSITION | 
		SH_GUI_REGION_RAW_SCALE    |
		SH_GUI_REGION_RAW_PRIORITY
	);

	float           button_offset_x     =  0.0f;
	uint32_t        button_region_idx   =  p_gui->region_infos.region_count;
	ShGuiRegionRaw* p_button_region_raw = &p_gui->region_infos.p_regions_raw[button_region_idx];

	uint32_t first_char_idx = p_gui->char_infos.char_count;

	uint8_t pressed = shGuiItem(
		p_gui,
		rail_position,
		button_scale,
		button_color,
		button_edge_color,
		SH_GUI_LEFT_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		SH_GUI_X_MOVABLE | SH_GUI_PIXELS,
		SH_GUI_LEFT_MOUSE_BUTTON,
		hint,
		scale,
		text_color
	);

	float rail_factor = (p_button_region_raw->position.x - window_region_raw.position.x) / rail_scale.x + 0.5f;
	(*p_dst)          = rail_factor * (max - min) + min;

	float cursor_dx = 0.0f;

	if (pressed) {
		float cursor_x = (float)*p_gui->inputs.p_cursor_pos_x;
		cursor_dx      = cursor_x - p_gui->inputs.last.last_cursor_pos_x;
	
		float _dst = (*p_dst) + cursor_dx;
		if (_dst < max && _dst > min) {
			(*p_dst) = _dst;
		}

		if (
			p_button_region_raw->position.x > (rail_position.x - rail_scale.x / 2.0f) &&
			p_button_region_raw->position.x < (rail_position.x + rail_scale.x / 2.0f)
			) {
			
			p_gui->region_infos.p_windows_slider_buttons_offsets[button_region_idx] += cursor_dx;
			region_write_src.position.x = p_button_region_raw->position.x + cursor_dx * ((float)*p_gui->inputs.p_delta_time);
		}
		else {
			region_write_src.position.x = p_button_region_raw->position.x;
		}



	}
	else {
		region_write_src.position.x = window_region_raw.position.x + p_gui->region_infos.p_windows_slider_buttons_offsets[button_region_idx];
	}

	if (
		p_button_region_raw->position.x >= (rail_position.x + rail_scale.x / 2.0f)
		) {
		region_write_src.position.x = rail_position.x + rail_scale.x / 2.0f;
	}
	else if (
		p_button_region_raw->position.x <= (rail_position.x - rail_scale.x / 2.0f)
		) {
		region_write_src.position.x = rail_position.x - rail_scale.x / 2.0f;
	}

	region_write_src.priority = SH_GUI_ITEM_REGION_PRIORITY + SH_GUI_MINIMUM_PRIORITY_INCREASE_STEP;

	if (window_region.priority   == SH_GUI_SELECTED_REGION_PRIORITY) {
		region_write_src.priority = SH_GUI_SELECTED_ITEM_REGION_PRIORITY + SH_GUI_MINIMUM_PRIORITY_INCREASE_STEP;
		chars_write_src.priority  = SH_GUI_SELECTED_ITEM_TEXT_PRIORITY + SH_GUI_MINIMUM_PRIORITY_INCREASE_STEP;
	}

	shGuiOverwriteRegion(
		p_gui,
		button_region_idx,
		&region_write_src,
		SH_GUI_REGION_RAW_POSITION |
		SH_GUI_REGION_RAW_PRIORITY
	);

	shGuiOverwriteChars(
		p_gui,
		first_char_idx,
		(uint32_t)strlen(hint),
		&chars_write_src,
		SH_GUI_CHAR_RAW_PRIORITY
	);

	return 1;
}

uint8_t shGuiMenuBar(
	ShGui*           p_gui, 
	float            extent,
	shguivec4        color,
	shguivec4        edge_color,
	ShGuiWidgetFlags flags
) {
	shGuiError(p_gui == NULL, "invalid gui memory", return 0);

	float region_width  = 100.0f;
	float region_height = extent;
	float region_pos_x  = 0.0f;
	float region_pos_y  = 100.0f;

	if (flags & SH_GUI_RELATIVE) {
		if (flags & SH_GUI_EDGE_TOP || flags & SH_GUI_EDGE_BOTTOM) {
			region_width = 100.0f;
			region_height = extent;
			region_pos_x = 0.0f;
		}
		if (flags & SH_GUI_EDGE_LEFT || flags & SH_GUI_EDGE_RIGHT) {
			region_width = extent;
			region_height = 100.0f;
			region_pos_y = 0.0f;
		}
	}
	else {
		float full_window_width  = (float)(*p_gui->inputs.p_window_width);
		float full_window_height = (float)(*p_gui->inputs.p_window_height);
	
		if (flags & SH_GUI_EDGE_TOP || flags & SH_GUI_EDGE_BOTTOM) {
			region_width = full_window_width;
			region_height = extent;
		}
		if (flags & SH_GUI_EDGE_LEFT || flags & SH_GUI_EDGE_RIGHT) {
			region_width = extent;
			region_height = full_window_height;
		}
		flags |= SH_GUI_PIXELS;
	}

	p_gui->region_infos.p_menus_region_indices[p_gui->region_infos.menu_count] = p_gui->region_infos.region_count;


	uint32_t bar_region_idx = p_gui->region_infos.region_count;
	

	shguivec2 region_scale = {
		region_width, region_height
	};

	shGuiRegion(
		p_gui, 
		SH_GUI_VEC2_ZERO,
		region_scale,
		color,
		edge_color,
		SH_GUI_NO_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		flags,
		0
	);

	p_gui->region_infos.menu_count++;

	return 1;
}

uint8_t shGuiMenuItem(
	ShGui*           p_gui,
	char*            title,
	shguivec4        region_color,
	shguivec4        region_edge_color,
	float            text_scale,
	shguivec4        text_color,
	ShGuiWidgetFlags anchor_flags
) {
	shGuiError(p_gui == NULL,           "invalid gui memory",   return 0);
	shGuiError(title == NULL, "invalid title memory", return 0);

	float width                         =  10.0f;//%
	ShGuiWidgetFlags additional_flags   =  SH_GUI_PIXELS;
	
	uint32_t        bar_count           =  p_gui->region_infos.menu_count - 1;
	uint32_t        bar_idx             =  p_gui->region_infos.p_menus_region_indices[bar_count];
	ShGuiRegionRaw* bar_region_raw      = &p_gui->region_infos.p_regions_raw[bar_idx];
	
	uint32_t       last_region_idx      =  p_gui->region_infos.region_count - 1;
	ShGuiRegionRaw last_region_raw      =  p_gui->region_infos.p_regions_raw[last_region_idx];

	width = (strlen(title) + 1.0f) * SH_GUI_CHAR_DISTANCE_OFFSET * text_scale / 4.0f;//in pixels

	shguivec2 item_position = { 0 };
	shguivec2 item_scale    = { 
		width,
		bar_region_raw->scale.y
	};
	

	ShGuiRegionRaw bar_region_write_src = {
		SH_GUI_VEC2_ZERO,
		bar_region_raw->scale,
		SH_GUI_VEC4_ZERO,
		SH_GUI_VEC3_ZERO,
		0.0f
	};
	
	if (
		bar_region_raw->scale.x == ((float)*p_gui->inputs.p_window_width) &&
		bar_region_raw->position.y > 0.0f
		) {
		additional_flags |= SH_GUI_EDGE_TOP;
		if (last_region_idx != bar_idx) {
			if (anchor_flags == SH_GUI_EDGE_LEFT) {
				item_position.x = last_region_raw.scale.x;
			}
			else if (anchor_flags == SH_GUI_EDGE_RIGHT) {
				item_position.x = -last_region_raw.scale.x;
			}
		}
	}
	else if (
		bar_region_raw->scale.x == ((float)*p_gui->inputs.p_window_width) &&
		bar_region_raw->position.y < 0.0f
		) {
		additional_flags |= SH_GUI_EDGE_BOTTOM;
		if (last_region_idx != bar_idx) {
			if (anchor_flags == SH_GUI_EDGE_LEFT) {
				item_position.x = last_region_raw.scale.x;
			}
			else if (anchor_flags == SH_GUI_EDGE_RIGHT) {
				item_position.x = -last_region_raw.scale.x;
			}
		}
	}
	else if (
		bar_region_raw->scale.y == ((float)*p_gui->inputs.p_window_height) &&
		bar_region_raw->position.x < 0.0f
		) {
		item_scale.y = text_scale;
		additional_flags |= SH_GUI_EDGE_LEFT;
		if (last_region_idx != bar_idx) {
			if (anchor_flags == SH_GUI_EDGE_TOP) {
				item_position.y = -last_region_raw.scale.y;
			}
			else if (anchor_flags == SH_GUI_EDGE_BOTTOM) {
				item_position.y = last_region_raw.scale.y;
			}
		}
	}
	else {
		item_scale.y = text_scale;
		additional_flags |= SH_GUI_EDGE_RIGHT;
		if (last_region_idx != bar_idx) {
			if (anchor_flags == SH_GUI_EDGE_TOP) {
				item_position.y = -last_region_raw.scale.y;
			}
			else if (anchor_flags == SH_GUI_EDGE_BOTTOM) {
				item_position.y = last_region_raw.scale.y;
			}
		}
	}

	if (additional_flags & SH_GUI_EDGE_LEFT || additional_flags & SH_GUI_EDGE_RIGHT) {
		item_scale.x = width;
		item_scale.y = text_scale;
		if (bar_region_raw->scale.x < item_scale.x) {
			bar_region_write_src.position.x = bar_region_raw->position.x + (item_scale.x - bar_region_raw->scale.x) / 2.0f;
			bar_region_write_src.scale.x = item_scale.x;
			if (additional_flags |= SH_GUI_EDGE_RIGHT) {
				bar_region_write_src.position.x = bar_region_raw->position.x - (item_scale.x - bar_region_raw->scale.x) / 2.0f;
			}
			shGuiOverwriteRegion(
				p_gui,
				bar_idx,
				&bar_region_write_src,
				SH_GUI_REGION_RAW_X_POSITION | SH_GUI_REGION_RAW_X_SCALE//position necessarily nedds to be changed
			);
		}
		else if (bar_region_raw->scale.x > item_scale.x) {
			item_scale.x = bar_region_raw->scale.x;
		}
	}

	uint32_t        item_region_idx     = p_gui->region_infos.region_count;
	uint32_t        first_char_idx      = p_gui->char_infos.char_count;
	ShGuiRegionRaw* p_button_region_raw = &p_gui->region_infos.p_regions_raw[item_region_idx];

	uint8_t pressed = shGuiItem(
		p_gui,
		item_position,
		item_scale,
		region_color,
		region_edge_color,
		SH_GUI_NO_MOUSE_BUTTON,
		SH_GUI_VEC2_ZERO,
		SH_GUI_VEC2_ZERO,
		anchor_flags |= additional_flags,
		SH_GUI_PRESS_ONCE,
		title,
		text_scale,
		text_color
	);

	ShGuiRegionRaw region_write_src = {
		(shguivec2){ 0 },
		(shguivec2){ 0 },
		(shguivec4){ 0 },
		(shguivec3){ 0 },
		SH_GUI_SELECTED_ITEM_REGION_PRIORITY
	};

	ShGuiCharRaw chars_write_src = {
		(shguivec2){ 0 },
		0.0f,
		SH_GUI_SELECTED_ITEM_TEXT_PRIORITY,
		(shguivec4){ 0 },
	};


	shGuiOverwriteRegion(
		p_gui,
		item_region_idx,
		&region_write_src,
		SH_GUI_REGION_RAW_PRIORITY
	);

	shGuiOverwriteChars(
		p_gui,
		first_char_idx,
		(uint32_t)strlen(title),
		&chars_write_src,
		SH_GUI_CHAR_RAW_PRIORITY
	);

	return pressed;
}


#ifdef __cplusplus
}
#endif//__cplusplus