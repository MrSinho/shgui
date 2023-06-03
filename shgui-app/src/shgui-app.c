#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include <shgui-app/shgui-app.h>
#include <stdlib.h>
#include <memory.h>


ShGuiApp* shGuiCreateApp(
	uint32_t    window_width,
	uint32_t    window_height,
	const char* application_name
) {
	shGuiError(window_width     == 0,    "invalid window width",            return NULL);
	shGuiError(window_height    == 0,    "invalid window height",           return NULL);
	shGuiError(application_name == NULL, "invalid application name memory", return NULL);

	/*
	GLFW and Vulkan implementation, very simular to shvulkan examples
	*/
	int r = glfwInit();

	shVkError(
		r < 1,
		"failed initializing glfw",
		return 0
	);

	r = glfwVulkanSupported();

	shVkError(
		r == GLFW_FALSE,
		"glfw does not support vulkan",
		return 0;
	);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

	uint32_t width  = window_width;
	uint32_t height = window_height;

	uint32_t    instance_extension_count = 0;
	GLFWwindow* window                   = glfwCreateWindow(width, height, application_name, NULL, NULL);
	char**      pp_instance_extensions   = glfwGetRequiredInstanceExtensions(&instance_extension_count);


	VkInstance                       instance                                     = NULL;
											                                      
	VkSurfaceKHR                     surface                                      = NULL;
	VkSurfaceCapabilitiesKHR         surface_capabilities                         = { 0 };
																	              
	VkPhysicalDevice                 physical_device                              = NULL;
	VkPhysicalDeviceProperties       physical_device_properties                   = { 0 };
	VkPhysicalDeviceFeatures         physical_device_features                     = { 0 };
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties            = { 0 };
																	              
	uint32_t                         graphics_queue_family_index                  = 0;
	uint32_t                         present_queue_family_index                   = 0;
																	              
	VkDevice                         device                                       = NULL;
	uint32_t                         device_extension_count                       = 0;
															                      
	VkQueue                          graphics_queue                               = NULL;
	VkQueue                          present_queue                                = NULL;
					           						                                  
	VkCommandPool                    graphics_cmd_pool                            = NULL;
	VkCommandPool                    present_cmd_pool                             = NULL;
															                      
	VkCommandBuffer                  graphics_cmd_buffers[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]  = { NULL };
	VkCommandBuffer                  present_cmd_buffer                           = NULL;
															                      
	VkFence                          graphics_cmd_fences[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]   = { NULL };
															                      
	VkSemaphore                      current_image_acquired_semaphore             = NULL;
	VkSemaphore                      current_graphics_queue_finished_semaphore    = NULL;

	VkSwapchainKHR                   swapchain                                    = NULL;
	VkFormat                         swapchain_image_format                       = 0;
	uint32_t                         swapchain_image_count                        = 0;
																	              
	uint32_t                         sample_count                                 = 0;
																	              
	VkAttachmentDescription          input_color_attachment                       = { 0 };
	VkAttachmentReference            input_color_attachment_reference             = { 0 };
	VkAttachmentDescription          depth_attachment                             = { 0 };
	VkAttachmentReference            depth_attachment_reference                   = { 0 };
	VkAttachmentDescription          resolve_attachment                           = { 0 };
	VkAttachmentReference            resolve_attachment_reference                 = { 0 };
	VkSubpassDescription             subpass                                      = { 0 };
																	              
	VkRenderPass                     renderpass                                   = NULL;

	VkImage                          swapchain_images[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]      = { NULL };
	VkImageView                      swapchain_image_views[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT] = { NULL };
	VkImage                          depth_image = NULL;
	VkDeviceMemory                   depth_image_memory                           = NULL;
	VkImageView                      depth_image_view                             = NULL;
	VkImage                          input_color_image                            = NULL;
	VkDeviceMemory                   input_color_image_memory                     = NULL;
	VkImageView                      input_color_image_view                       = NULL;

	VkFramebuffer                    framebuffers[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]          = { NULL };

	shCreateInstance(
		//application_name, engine_name, enable_validation_layers,
		"vulkan app", "vulkan engine", 1,
		instance_extension_count, pp_instance_extensions,
		VK_MAKE_API_VERSION(1, 3, 250, 0),
		&instance
	);

	glfwCreateWindowSurface(
		instance, window, NULL, &surface
	);

	shSelectPhysicalDevice(
		instance,//instance,
		surface,//surface,
		VK_QUEUE_GRAPHICS_BIT |
		VK_QUEUE_COMPUTE_BIT |
		VK_QUEUE_TRANSFER_BIT,//requirements,
		&physical_device,//p_physical_device,
		&physical_device_properties,//p_physical_device_properties,
		&physical_device_features,//p_physical_device_features,
		&physical_device_memory_properties//p_physical_device_memory_properties
	);

	uint32_t graphics_queue_families_indices[SH_MAX_STACK_QUEUE_FAMILY_COUNT] = { 0 };
	uint32_t present_queue_families_indices [SH_MAX_STACK_QUEUE_FAMILY_COUNT] = { 0 };
	shGetPhysicalDeviceQueueFamilies(
		physical_device,//physical_device
		surface,//surface
		NULL,//p_queue_family_count
		NULL,//p_graphics_queue_family_count
		NULL,//p_surface_queue_family_count
		NULL,//p_compute_queue_family_count
		NULL,//p_transfer_queue_family_count
		graphics_queue_families_indices,//p_graphics_queue_family_indices
		present_queue_families_indices,//p_surface_queue_family_indices
		NULL,//p_compute_queue_family_indices
		NULL,//p_transfer_queue_family_indices
		NULL//p_queue_families_properties
	);
	graphics_queue_family_index = graphics_queue_families_indices[0];
	present_queue_family_index  = present_queue_families_indices [0];

	shGetPhysicalDeviceSurfaceCapabilities(
		physical_device, surface, &surface_capabilities
	);

	float default_queue_priority = 1.0f;
	VkDeviceQueueCreateInfo graphics_device_queue_info = { 0 };
	shQueryForDeviceQueueInfo(
		graphics_queue_family_index,//queue_family_index
		1,//queue_count
		&default_queue_priority,//p_queue_priorities
		0,//protected
		&graphics_device_queue_info//p_device_queue_info
	);

	VkDeviceQueueCreateInfo present_device_queue_info = { 0 };
	shQueryForDeviceQueueInfo(
		present_queue_family_index,//queue_family_index
		1,//queue_count
		&default_queue_priority,//p_queue_priorities
		0,//protected
		&present_device_queue_info//p_device_queue_info
	);

	VkDeviceQueueCreateInfo device_queue_infos[2] = {
		graphics_device_queue_info,
		present_device_queue_info
	};
	char* device_extensions[2]  = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	uint32_t device_queue_count = (graphics_queue_family_index == present_queue_family_index) ? 1 : 2;
	shSetLogicalDevice(
		physical_device,//physical_device
		&device,//p_device
		1,//extension_count
		device_extensions,//pp_extension_names
		device_queue_count,//device_queue_count
		device_queue_infos//p_device_queue_infos
	);

	shGetDeviceQueues(
		device,//device
		1,//queue_count
		&graphics_queue_family_index,//p_queue_family_indices
		&graphics_queue//p_queues
	);

	if (graphics_queue_family_index != present_queue_family_index) {
		shGetDeviceQueues(
			device,//device
			1,//queue_count
			&present_queue_family_index,//p_queue_family_indices
			&present_queue//p_queues
		);
	}
	else {
		present_queue = graphics_queue;
	}

	VkSharingMode swapchain_image_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
	if (graphics_queue_family_index != present_queue_family_index) {
		swapchain_image_sharing_mode = VK_SHARING_MODE_CONCURRENT;
	}
	shCreateSwapchain(
		device,//device
		physical_device,//physical_device
		surface,//surface
		VK_FORMAT_R8G8B8_UNORM,//image_format
		&swapchain_image_format,//p_image_format
		SH_GUI_APP_SWAPCHAIN_IMAGE_COUNT,//swapchain_image_count
		swapchain_image_sharing_mode,//image_sharing_mode
		0,//vsync
		&swapchain_image_count,
		&swapchain//p_swapchain
	);

	shCreateCommandPool(
		device,//device
		graphics_queue_family_index,//queue_family_index
		&graphics_cmd_pool//p_cmd_pool
	);

	shAllocateCommandBuffers(
		device,//device
		graphics_cmd_pool,//cmd_pool
		swapchain_image_count,//cmd_buffer_count
		graphics_cmd_buffers//p_cmd_buffer
	);

	if (graphics_queue_family_index != present_queue_family_index) {
		shCreateCommandPool(
			device,//device
			present_queue_family_index,//queue_family_index
			&present_cmd_pool//p_cmd_pool
		);
	}
	else {
		present_cmd_pool   = graphics_cmd_pool;
	}
	shAllocateCommandBuffers(
		device,//device
		present_cmd_pool,//cmd_pool
		1,//cmd_buffer_count
		&present_cmd_buffer//p_cmd_buffer
	);

	shCreateFences(
		device,//device
		swapchain_image_count,//fence_count
		1,//signaled
		graphics_cmd_fences//p_fences
	);

	shGetSwapchainImages(
		device,//device
		swapchain,//swapchain
		&swapchain_image_count,//p_swapchain_image_count
		swapchain_images//p_swapchain_images
	);

	for (uint32_t i = 0; i < swapchain_image_count; i++) {
		shCreateImageView(
			device,//device
			swapchain_images[i],//image
			VK_IMAGE_VIEW_TYPE_2D,//view_type
			VK_IMAGE_ASPECT_COLOR_BIT,//image_aspect
			1,//mip_levels
			swapchain_image_format,//format
			&swapchain_image_views[i]//p_image_view
		);
	}

	shCombineMaxSamples(
		physical_device_properties, 64,
		1, 1, &sample_count
	);

	shCreateRenderpassAttachment(
		swapchain_image_format,//format
		sample_count,//sample_count
		VK_ATTACHMENT_LOAD_OP_CLEAR,//load_treatment
		VK_ATTACHMENT_STORE_OP_STORE,//store_treatment
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,//stencil_load_treatment
		VK_ATTACHMENT_STORE_OP_DONT_CARE,//stencil_store_treatment
		VK_IMAGE_LAYOUT_UNDEFINED,//initial_layout
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,//final_layout
		&input_color_attachment//p_attachment_description
	);
	shCreateRenderpassAttachmentReference(
		0,//attachment_idx
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,//layout
		&input_color_attachment_reference//p_attachment_reference
	);

	shCreateRenderpassAttachment(
		VK_FORMAT_D32_SFLOAT,
		sample_count,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		&depth_attachment
	);
	shCreateRenderpassAttachmentReference(
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		&depth_attachment_reference
	);
	
	shCreateRenderpassAttachment(
		swapchain_image_format,
		1,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		&resolve_attachment
	);
	shCreateRenderpassAttachmentReference(
		2,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		&resolve_attachment_reference
	);

	shCreateSubpass(
		VK_PIPELINE_BIND_POINT_GRAPHICS,//bind_point
		0,//input_attachment_count
		NULL,//p_input_attachments_reference
		1,//color_attachment_count
		&input_color_attachment_reference,//p_color_attachments_reference
		&depth_attachment_reference,//p_depth_stencil_attachment_reference
		&resolve_attachment_reference,//p_resolve_attachment_reference
		0,//preserve_attachment_count
		NULL,//p_preserve_attachments
		&subpass//p_subpass
	);

	VkAttachmentDescription attachment_descriptions[SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT] = {
		input_color_attachment, depth_attachment, resolve_attachment
	};
	shCreateRenderpass(
		device,//device
		SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT,//attachment_count
		attachment_descriptions,//p_attachments_descriptions
		1,//subpass_count
		&subpass,//p_subpasses
		&renderpass//p_renderpass
	);
	
	shCreateImage(
		device,
		VK_IMAGE_TYPE_2D,
		width, height, 1,
		VK_FORMAT_D32_SFLOAT,
		1, sample_count,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		&depth_image
	);
	shAllocateImageMemory(
		device, physical_device, depth_image,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&depth_image_memory
	);
	shBindImageMemory(
		device, depth_image, 0, depth_image_memory
	);
	shCreateImageView(
		device, depth_image, VK_IMAGE_VIEW_TYPE_2D,
		VK_IMAGE_ASPECT_DEPTH_BIT, 1,
		VK_FORMAT_D32_SFLOAT, &depth_image_view
	);

	shCreateImage(
		device, VK_IMAGE_TYPE_2D, width, height, 1,
		swapchain_image_format, 1, sample_count,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
		&input_color_image
	);
	shAllocateImageMemory(
		device, physical_device, input_color_image,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &input_color_image_memory
	);
	shBindImageMemory(
		device, input_color_image, 0, input_color_image_memory
	);
	shCreateImageView(
		device, input_color_image, VK_IMAGE_VIEW_TYPE_2D,
		VK_IMAGE_ASPECT_COLOR_BIT, 1, swapchain_image_format,
		&input_color_image_view
	);

	for (uint32_t i = 0; i < swapchain_image_count; i++) {
		VkImageView image_views[SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT] = {
			input_color_image_view, depth_image_view, swapchain_image_views[i]
		};
		shCreateFramebuffer(
			device,//device
			renderpass,//renderpass
			SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT,//image_view_count
			image_views,//p_image_views
			surface_capabilities.currentExtent.width,//x
			surface_capabilities.currentExtent.height,//y
			1,//z
			&framebuffers[i]//p_framebuffer
		);
	}

	shCreateSemaphores(
		device,//device
		1,//semaphore_count
		&current_image_acquired_semaphore//p_semaphores
	);

	shCreateSemaphores(
		device,//device
		1,//semaphore_count
		&current_graphics_queue_finished_semaphore//p_semaphores
	);


	ShGuiCore gui_core = {
		device,
		physical_device,
		graphics_queue_family_index,
		graphics_queue,
		surface,
		renderpass,
		sample_count,
		swapchain_image_count
	};



	/*
	ShGui implementation after shvulkan implementation, very similar to shvulkan examples
	*/
	ShGuiApp* p_app   = shAllocateGuiApp();
	shGuiError(p_app == NULL, "invalid gui app memory", return NULL);

	ShGui*    p_gui   = &p_app->gui;

	shGuiInit(p_gui, gui_core);

	p_app->width                                     = width;
	p_app->height                                    = height;
	p_app->window                                    = window;

	p_app->instance                                  = instance;
	p_app->device                                    = device;
	p_app->surface                                   = surface;
	p_app->surface_capabilities                      = surface_capabilities;
	p_app->physical_device                           = physical_device;
	p_app->physical_device_properties                = physical_device_properties;
	p_app->physical_device_features                  = physical_device_features;
	p_app->physical_device_memory_properties         = physical_device_memory_properties;
	p_app->graphics_queue_family_index               = graphics_queue_family_index;
	p_app->present_queue_family_index                = present_queue_family_index;
	p_app->device                                    = device;
	p_app->device_extension_count                    = device_extension_count;
	p_app->graphics_queue                            = graphics_queue;
	p_app->present_queue                             = present_queue;
	p_app->graphics_cmd_pool                         = graphics_cmd_pool;
	p_app->present_cmd_pool                          = present_cmd_pool;
	memcpy(p_app->graphics_cmd_buffers, graphics_cmd_buffers, sizeof(VkCommandBuffer) * swapchain_image_count);
	p_app->present_cmd_buffer                        = present_cmd_buffer;
	memcpy(p_app->graphics_cmd_fences, graphics_cmd_fences, sizeof(VkFence) * swapchain_image_count);
	p_app->current_image_acquired_semaphore          = current_image_acquired_semaphore;
	p_app->current_graphics_queue_finished_semaphore = current_graphics_queue_finished_semaphore;
	p_app->swapchain                                 = swapchain;
	p_app->swapchain_image_count                     = swapchain_image_count;
	p_app->swapchain_image_sharing_mode              = swapchain_image_sharing_mode;
	p_app->swapchain_image_format                    = swapchain_image_format;
	p_app->swapchain_image_count                     = swapchain_image_count;
	p_app->sample_count                              = sample_count;
	p_app->input_color_attachment                    = input_color_attachment;
	p_app->input_color_attachment_reference          = input_color_attachment_reference;
	p_app->depth_attachment                          = depth_attachment;
	p_app->depth_attachment_reference                = depth_attachment_reference;
	p_app->resolve_attachment                        = resolve_attachment;
	p_app->resolve_attachment_reference              = resolve_attachment_reference;
	p_app->subpass                                   = subpass;
	p_app->subpass.pColorAttachments                 = &p_app->input_color_attachment_reference;
	p_app->subpass.pDepthStencilAttachment           = &p_app->depth_attachment_reference;
	p_app->subpass.pResolveAttachments               = &p_app->resolve_attachment_reference;
	p_app->renderpass                                = renderpass;
	memcpy(p_app->swapchain_images, swapchain_images, sizeof(VkImage) * swapchain_image_count);
	memcpy(p_app->swapchain_image_views, swapchain_image_views, sizeof(VkImageView) * swapchain_image_count);
	p_app->depth_image                               = depth_image;
	p_app->depth_image_memory                        = depth_image_memory;
	p_app->depth_image_view                          = depth_image_view;
	p_app->input_color_image                         = input_color_image;
	p_app->input_color_image_memory                  = input_color_image_memory;
	p_app->input_color_image_view                    = input_color_image_view;
	memcpy(p_app->framebuffers, framebuffers, sizeof(VkFramebuffer) * swapchain_image_count);



	shGuiAllocateMemory(p_gui, 2048, 1024);

	shGuiAllocatePipelineResources(p_gui, swapchain_image_count);

	shGuiBuildRegionPipeline(
		p_gui,
		NULL,
		NULL
	);
	shGuiBuildCharPipeline(
		p_gui,
		NULL,
		NULL
	);

	p_app->last_time  = glfwGetTime();

	shGuiLinkInputs(
		p_gui,
		&p_app->width, &p_app->height, 
		&p_app->cursor_pos_x, &p_app->cursor_pos_y,
		p_app->key_events, p_app->mouse_events,
		&p_app->delta_time
	);

	shGuiScaleUI(p_gui, 1.0f, 1.0f);

	return p_app;
}

uint8_t shGuiReleaseApp(
	ShGuiApp* p_app
) {
	shGuiError(p_app == NULL, "invalid app memory", return 0);
	
	ShGui* p_gui = &p_app->gui;

	shGuiError(
		p_gui == NULL, 
		"invalid gui memory", 
		return 0
	);


	VkInstance       instance                                  = p_app->instance;
	VkDevice         device                                    = p_app->device;

	VkSemaphore      current_image_acquired_semaphore          = p_app->current_image_acquired_semaphore;
	VkSemaphore      current_graphics_queue_finished_semaphore = p_app->current_graphics_queue_finished_semaphore;

	VkFence*         p_graphics_cmd_fences                     = p_app->graphics_cmd_fences;
											                   
	VkCommandPool    graphics_cmd_pool                         = p_app->graphics_cmd_pool;
	VkCommandBuffer* p_graphics_cmd_buffers                    = p_app->graphics_cmd_buffers;
											                   
	VkCommandPool    present_cmd_pool                          = p_app->present_cmd_pool;
	VkCommandBuffer  present_cmd_buffer                        = p_app->present_cmd_buffer;
												               
	uint32_t         graphics_queue_family_index               = p_app->graphics_queue_family_index;
	uint32_t         present_queue_family_index                = p_app->present_queue_family_index;
												               
	VkImage          depth_image                               = p_app->depth_image;
	VkDeviceMemory   depth_image_memory                        = p_app->depth_image_memory;
											                   
	VkImage          input_color_image                         = p_app->input_color_image;
	VkDeviceMemory   input_color_image_memory                  = p_app->input_color_image_memory;
											                   
	VkImageView      depth_image_view                          = p_app->depth_image_view;
	VkImageView      input_color_image_view                    = p_app->input_color_image_view;
	VkImageView*     p_swapchain_image_views                   = p_app->swapchain_image_views;
											                   
	VkRenderPass     renderpass                                = p_app->renderpass;
										                       
	uint32_t         swapchain_image_count                     = p_app->swapchain_image_count;
	VkFramebuffer*   p_framebuffers                            = p_app->framebuffers;
												               
	VkSwapchainKHR swapchain                                   = p_app->swapchain;
	VkSurfaceKHR   surface                                     = p_app->surface;


	shWaitDeviceIdle(device);

	shGuiRelease(p_gui);

	shDestroySemaphores(device, 1, &current_image_acquired_semaphore);

	shDestroySemaphores(device, 1, &current_graphics_queue_finished_semaphore);

	shDestroyFences(device, swapchain_image_count, p_graphics_cmd_fences);

	shDestroyCommandBuffers(device, graphics_cmd_pool, swapchain_image_count, p_graphics_cmd_buffers);

	shDestroyCommandBuffers(device, present_cmd_pool, 1, &present_cmd_buffer);

	shDestroyCommandPool(device, graphics_cmd_pool);
	if (graphics_queue_family_index != present_queue_family_index) {
		shDestroyCommandPool(device, present_cmd_pool);
	}

	shClearImageMemory(device, depth_image, depth_image_memory);
	shClearImageMemory(device, input_color_image, input_color_image_memory);
	shDestroyImageViews(device, 1, &depth_image_view);
	shDestroyImageViews(device, 1, &input_color_image_view);

	shDestroyRenderpass(device, renderpass);

	shDestroyFramebuffers(device, swapchain_image_count, p_framebuffers);

	shDestroyImageViews(device, swapchain_image_count, p_swapchain_image_views);

	shDestroySwapchain(device, swapchain);

	shDestroyDevice(device);

	shDestroySurface(instance, surface);

	shDestroyInstance(instance);


	glfwDestroyWindow(p_app->window);

	glfwTerminate();

	shGuiFreeApp(p_app);

	return 1;
}

uint8_t shGuiAppRunning(
	ShGuiApp* p_app
) {
	shGuiError(p_app         == NULL, "invalid application memory",        return 0);
	shGuiError(p_app->window == NULL, "invalid application window memory", return 0);

	uint8_t running = !glfwWindowShouldClose(p_app->window);

	glfwPollEvents();

	double now = glfwGetTime();
	p_app->delta_time = now - p_app->last_time;
	p_app->last_time = now;

	for (uint32_t key_idx = 0; key_idx < SH_GUI_KEY_LAST + 1; key_idx++) {
		p_app->key_events[key_idx] = glfwGetKey(p_app->window, key_idx);
}
	for (uint32_t mouse_button_idx = 0; mouse_button_idx < SH_GUI_MOUSE_LAST + 1; mouse_button_idx++) {
		p_app->mouse_events[mouse_button_idx] = glfwGetMouseButton(p_app->window, mouse_button_idx);
	}

	glfwGetCursorPos(p_app->window, &p_app->d_cursor_pos_x, &p_app->d_cursor_pos_y);

	p_app->cursor_pos_x =  (float)p_app->d_cursor_pos_x - ((float)(p_app->width) / 2.0f);
	p_app->cursor_pos_y = -(float)p_app->d_cursor_pos_y + ((float)(p_app->height) / 2.0f);

	int cursors[5] = {
		GLFW_CURSOR_NORMAL,
		GLFW_HRESIZE_CURSOR,
		GLFW_VRESIZE_CURSOR,
		GLFW_RESIZE_NESW_CURSOR,
		GLFW_RESIZE_NWSE_CURSOR
	};
	GLFWcursor* cursor = glfwCreateStandardCursor(cursors[p_app->gui.inputs.active_cursor]);
	glfwSetCursor(p_app->window, cursor);

	return running;
}

uint8_t SH_GUI_CALL shGuiAppCheckWindowSize(
	ShGuiApp* p_app
) {
	shGuiError(p_app == NULL, "invalid application memory", return 0);

	GLFWwindow* window = p_app->window;

	VkInstance               instance                     = p_app->instance;
	VkDevice                 device                       = p_app->device;
	VkPhysicalDevice         physical_device              = p_app->physical_device;
														  	                  
	uint32_t                 sample_count                 = p_app->sample_count;				    
	VkSubpassDescription     subpass                      = p_app->subpass;
														  
	uint32_t                 swapchain_image_count        = p_app->swapchain_image_count;
					         							  
	VkSharingMode            swapchain_image_sharing_mode = p_app->swapchain_image_sharing_mode;
	VkFormat                 swapchain_image_format       = p_app->swapchain_image_format;

	uint32_t width  = p_app->width;
	uint32_t height = p_app->height;

	ShGui* p_gui    = &p_app->gui;

	int _width = 0;
	int _height = 0;
	glfwGetWindowSize(window, &_width, &_height);

	if (_width != 0 && _height != 0) {//otherwise it's minimized
		if (_width != width || _height != height) {//window is resized

			shGuiResizeInterface(p_gui, width, height, _width, _height);

			p_app->width  = _width;
			p_app->height = _height;

			shWaitDeviceIdle(device);

			shDestroyRenderpass(device, p_app->renderpass);
			shDestroyFramebuffers(device, swapchain_image_count, p_app->framebuffers);
			shDestroyImageViews(device, swapchain_image_count, p_app->swapchain_image_views);
			shDestroySwapchain(device, p_app->swapchain);
			shDestroySurface(instance, p_app->surface);

			shClearImageMemory(device, p_app->depth_image, p_app->depth_image_memory);
			shClearImageMemory(device, p_app->input_color_image, p_app->input_color_image_memory);
			shDestroyImageViews(device, 1, &p_app->depth_image_view);
			shDestroyImageViews(device, 1, &p_app->input_color_image_view);

			glfwCreateWindowSurface(instance, window, NULL, &p_app->surface);
			shGetPhysicalDeviceSurfaceCapabilities(physical_device, p_app->surface, &p_app->surface_capabilities);
			shCreateSwapchain(
				device,
				physical_device,
				p_app->surface,
				swapchain_image_format,
				&swapchain_image_format,
				swapchain_image_count,
				swapchain_image_sharing_mode,
				0,
				&swapchain_image_count,
				&p_app->swapchain
			);
			shGetSwapchainImages(device, p_app->swapchain, &swapchain_image_count, p_app->swapchain_images);
			for (uint32_t i = 0; i < p_app->swapchain_image_count; i++) {
				shCreateImageView(device, p_app->swapchain_images[i], VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, swapchain_image_format, &p_app->swapchain_image_views[i]);
			}

			shCreateImage(
				device, VK_IMAGE_TYPE_2D,
				p_app->width, p_app->height, 1,
				VK_FORMAT_D32_SFLOAT, 
				1, sample_count,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_SHARING_MODE_EXCLUSIVE, &p_app->depth_image
			);
			shAllocateImageMemory(
				device, physical_device, p_app->depth_image,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&p_app->depth_image_memory
			);
			shBindImageMemory(
				device, p_app->depth_image, 0, p_app->depth_image_memory
			);
			shCreateImageView(
				device, p_app->depth_image, VK_IMAGE_VIEW_TYPE_2D,
				VK_IMAGE_ASPECT_DEPTH_BIT, 1,
				VK_FORMAT_D32_SFLOAT, &p_app->depth_image_view
			);

			shCreateImage(
				device, VK_IMAGE_TYPE_2D,
				p_app->width, p_app->height, 1,
				swapchain_image_format, 
				1, sample_count, 
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				&p_app->input_color_image
			);
			shAllocateImageMemory(
				device, physical_device, p_app->input_color_image,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&p_app->input_color_image_memory
			);
			shBindImageMemory(
				device, p_app->input_color_image, 0, p_app->input_color_image_memory
			);
			shCreateImageView(
				device, p_app->input_color_image, VK_IMAGE_VIEW_TYPE_2D,
				VK_IMAGE_ASPECT_COLOR_BIT, 1, swapchain_image_format,
				&p_app->input_color_image_view
			);

			VkAttachmentDescription attachment_descriptions[SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT] = {
				p_app->input_color_attachment, p_app->depth_attachment, p_app->resolve_attachment
			};
			shCreateRenderpass(device, SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT, attachment_descriptions, 1, &subpass, &p_app->renderpass);
			for (uint32_t i = 0; i < swapchain_image_count; i++) {
				VkImageView image_views[SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT] = {
					p_app->input_color_image_view, p_app->depth_image_view, p_app->swapchain_image_views[i] 
				};
				shCreateFramebuffer(device, p_app->renderpass, SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT, image_views, _width, _height, 1, &p_app->framebuffers[i]);
			}

			shGuiDestroyPipelines(p_gui);
			shGuiSetSurface(p_gui, p_app->surface);
			shGuiSetRenderpass(p_gui, p_app->renderpass);
			shGuiBuildRegionPipeline(
				p_gui,
				"../shaders/bin/shgui-region.vert.spv",
				"../shaders/bin/shgui-region.frag.spv"
			);
			shGuiBuildCharPipeline(
				p_gui,
				"../shaders/bin/shgui-char.vert.spv",
				"../shaders/bin/shgui-char.frag.spv"
			);
		}
	}

	return 1;
}

uint8_t SH_GUI_CALL shGuiAppUpdate(
	ShGuiApp* p_app
) {
	shGuiError(p_app == NULL, "invalid app memory", return 0);

	ShGui*   p_gui  = &p_app->gui;
	VkDevice device =  p_app->device;

	shGuiError(
		p_gui == NULL, "invalid gui memory",
		return 0
	);

	shGuiUpdateInputs(p_gui);
	shGuiSubmitInputs(p_gui);


	shAcquireSwapchainImage(
		device,//device
		p_app->swapchain,//swapchain
		UINT64_MAX,//timeout_ns
		p_app->current_image_acquired_semaphore,//acquired_signal_semaphore
		NULL,//acquired_signal_fence
		&p_app->swapchain_image_idx//p_swapchain_image_index
	);

	VkCommandBuffer cmd_buffer = p_app->graphics_cmd_buffers[p_app->swapchain_image_idx];

	shWaitForFences(
		device,//device
		1,//fence_count
		&p_app->graphics_cmd_fences[p_app->swapchain_image_idx],//p_fences
		1,//wait_for_all
		UINT64_MAX//timeout_ns
	);

	shResetFences(
		device,//device
		1,//fence_count
		&p_app->graphics_cmd_fences[p_app->swapchain_image_idx]//p_fences
	);

	shBeginCommandBuffer(cmd_buffer);

	shGuiWriteMemory(
		p_gui, cmd_buffer, 0
	);

	VkClearValue clear_values[2] = { 0 };
	float* p_colors = clear_values[0].color.float32;
	p_colors[0] = (float)0.0f;
	p_colors[1] = (float)0.0f;
	p_colors[2] = (float)0.0f;
	float* p_depth = &clear_values[1].depthStencil.depth;
	(*p_depth) = 1.0f;

	shBeginRenderpass(
		cmd_buffer,//graphics_cmd_buffer
		p_app->renderpass,//renderpass
		0,//render_offset_x
		0,//render_offset_y
		p_app->surface_capabilities.currentExtent.width,//render_size_x
		p_app->surface_capabilities.currentExtent.height,//render_size_y
		2,//only attachments with VK_ATTACHMENT_LOAD_OP_CLEAR
		clear_values,//p_clear_values
		p_app->framebuffers[p_app->swapchain_image_idx]//framebuffer
	);

	shGuiRender(
		p_gui, cmd_buffer,
		p_app->swapchain_image_idx
	);

	shEndRenderpass(cmd_buffer);

	shEndCommandBuffer(cmd_buffer);

	shQueueSubmit(
		1,//cmd_buffer_count
		&cmd_buffer,//p_cmd_buffers
		p_app->graphics_queue,//queue
		p_app->graphics_cmd_fences[p_app->swapchain_image_idx],//fence
		1,//semaphores_to_wait_for_count
		&p_app->current_image_acquired_semaphore,//p_semaphores_to_wait_for
		VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,//wait_stage
		1,//signal_semaphore_count
		&p_app->current_graphics_queue_finished_semaphore//p_signal_semaphores
	);

	shQueuePresentSwapchainImage(
		p_app->present_queue,//present_queue
		1,//semaphores_to_wait_for_count
		&p_app->current_graphics_queue_finished_semaphore,//p_semaphores_to_wait_for
		p_app->swapchain,//swapchain
		p_app->swapchain_image_idx//swapchain_image_idx
	);

	p_app->swapchain_image_idx = (p_app->swapchain_image_idx + 1) % p_app->swapchain_image_count;

	return 1;
}

#ifdef __cplusplus
}
#endif//__cplusplus
