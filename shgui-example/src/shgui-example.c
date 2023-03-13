#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <shvulkan/shVulkan.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>

#define SWAPCHAIN_IMAGE_COUNT 2
#define RENDERPASS_ATTACHMENT_COUNT 3

#include <shgui/shgui.h>

int main(void) {

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

	int width  = 720;
	int height = 480;

	uint32_t    instance_extension_count = 0;
	GLFWwindow* window                   = glfwCreateWindow(720, 480, "vulkan resizable clear color", NULL, NULL);
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
															                      
	VkCommandBuffer                  graphics_cmd_buffers[SWAPCHAIN_IMAGE_COUNT]  = { NULL };
	VkCommandBuffer                  present_cmd_buffer                           = NULL;
															                      
	VkFence                          graphics_cmd_fences[SWAPCHAIN_IMAGE_COUNT]   = { NULL };
															                      
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

	VkImage                          swapchain_images[SWAPCHAIN_IMAGE_COUNT]      = { NULL };
	VkImageView                      swapchain_image_views[SWAPCHAIN_IMAGE_COUNT] = { NULL };
	VkImage                          depth_image = NULL;
	VkDeviceMemory                   depth_image_memory                           = NULL;
	VkImageView                      depth_image_view                             = NULL;
	VkImage                          input_color_image                            = NULL;
	VkDeviceMemory                   input_color_image_memory                     = NULL;
	VkImageView                      input_color_image_view                       = NULL;

	VkFramebuffer                    framebuffers[SWAPCHAIN_IMAGE_COUNT]          = { NULL };

	shCreateInstance(
		//application_name, engine_name, enable_validation_layers,
		"vulkan app", "vulkan engine", 1,
		instance_extension_count, pp_instance_extensions,
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

	shCreateCommandPool(
		device,//device
		graphics_queue_family_index,//queue_family_index
		&graphics_cmd_pool//p_cmd_pool
	);

	shAllocateCommandBuffers(
		device,//device
		graphics_cmd_pool,//cmd_pool
		SWAPCHAIN_IMAGE_COUNT,//cmd_buffer_count
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
		SWAPCHAIN_IMAGE_COUNT,//fence_count
		1,//signaled
		graphics_cmd_fences//p_fences
	);

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
		SWAPCHAIN_IMAGE_COUNT,//swapchain_image_count
		swapchain_image_sharing_mode,//image_sharing_mode
		0,//vsync
		&swapchain//p_swapchain
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
		VK_ATTACHMENT_LOAD_OP_LOAD,
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

	VkAttachmentDescription attachment_descriptions[RENDERPASS_ATTACHMENT_COUNT] = {
		input_color_attachment, depth_attachment, resolve_attachment
	};
	shCreateRenderpass(
		device,//device
		RENDERPASS_ATTACHMENT_COUNT,//attachment_count
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
		VkImageView image_views[RENDERPASS_ATTACHMENT_COUNT] = {
			input_color_image_view, depth_image_view, swapchain_image_views[i]
		};
		shCreateFramebuffer(
			device,//device
			renderpass,//renderpass
			RENDERPASS_ATTACHMENT_COUNT,//image_view_count
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
	
	ShGui* p_gui = shAllocateGui();
	
	shGuiInit(p_gui, gui_core);

	shGuiAllocateMemory(p_gui, 1024, 1024);

	shGuiAllocatePipelineResources(p_gui, SWAPCHAIN_IMAGE_COUNT);

#ifndef NDEBUG
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
#else
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
#endif//NDEBUG

	

	double d_cursor_pos_x = 0.0f;
	double d_cursor_pos_y = 0.0f;
	float  cursor_pos_x   = 0.0f;
	float  cursor_pos_y   = 0.0f;

	ShGuiKeyEvents   key_events   = { 0 };
	ShGuiMouseEvents mouse_events = { 0 };

	double last_time  = glfwGetTime();
	double delta_time = 0.0;

	shGuiLinkInputs(
		p_gui,
		&width, &height, 
		&cursor_pos_x, &cursor_pos_y,
		key_events, mouse_events,
		&delta_time
	);

	shGuiScaleUI(p_gui, 1.5f, 2.0f);

	uint32_t swapchain_image_idx = 0;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		double now = glfwGetTime();
		delta_time = now - last_time;
		last_time  = now;

		for (uint32_t key_idx = 0; key_idx < SH_GUI_KEY_LAST + 1; key_idx++) {
			key_events[key_idx] = glfwGetKey(window, key_idx);
		}
		for (uint32_t mouse_button_idx = 0; mouse_button_idx < SH_GUI_MOUSE_LAST + 1; mouse_button_idx++) {
			mouse_events[mouse_button_idx] = glfwGetMouseButton(window, mouse_button_idx);
		}

		int _width = 0;
		int _height = 0;
		glfwGetWindowSize(window, &_width, &_height);

		glfwGetCursorPos(window, &d_cursor_pos_x, &d_cursor_pos_y);

		cursor_pos_x =  (float)d_cursor_pos_x - ((float)(width)  / 2.0f);
		cursor_pos_y = -(float)d_cursor_pos_y + ((float)(height) / 2.0f);

		int cursors[5] = {
			GLFW_CURSOR_NORMAL,
			GLFW_HRESIZE_CURSOR,
			GLFW_VRESIZE_CURSOR,
			GLFW_RESIZE_NESW_CURSOR,
			GLFW_RESIZE_NWSE_CURSOR
		};
		GLFWcursor* cursor = glfwCreateStandardCursor(cursors[p_gui->inputs.active_cursor]);
		//printf("%i\n", p_gui->inputs.active_cursor);
		glfwSetCursor(window, cursor);



		if (_width != 0 && height != 0) {//otherwise it's minimized
			if (_width != width || _height != height) {//window is resized

				shGuiResizeInterface(p_gui, width, height, _width, _height);

				width  = _width;
				height = _height;

				shWaitDeviceIdle(device);

				shDestroyRenderpass(device, renderpass);
				shDestroyFramebuffers(device, swapchain_image_count, framebuffers);
				shDestroyImageViews(device, swapchain_image_count, swapchain_image_views);
				shDestroySwapchain(device, swapchain);
				shDestroySurface(instance, surface);

				shClearImageMemory(device, depth_image, depth_image_memory);
				shClearImageMemory(device, input_color_image, input_color_image_memory);
				shDestroyImageViews(device, 1, &depth_image_view);
				shDestroyImageViews(device, 1, &input_color_image_view);

				glfwCreateWindowSurface(instance, window, NULL, &surface);
				shGetPhysicalDeviceSurfaceCapabilities(physical_device, surface, &surface_capabilities);
				shCreateSwapchain(
					device,
					physical_device,
					surface,
					swapchain_image_format,
					&swapchain_image_format,
					SWAPCHAIN_IMAGE_COUNT,
					swapchain_image_sharing_mode,
					0,
					&swapchain
				);
				shGetSwapchainImages(device, swapchain, &swapchain_image_count, swapchain_images);
				for (uint32_t i = 0; i < swapchain_image_count; i++) {
					shCreateImageView(device, swapchain_images[i], VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, swapchain_image_format, &swapchain_image_views[i]);
				}

				shCreateImage(
					device, VK_IMAGE_TYPE_2D,
					width, height, 1,
					VK_FORMAT_D32_SFLOAT, 
					1, sample_count,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					VK_SHARING_MODE_EXCLUSIVE, &depth_image
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
					device, VK_IMAGE_TYPE_2D,
					width, height, 1,
					swapchain_image_format, 
					1, sample_count, 
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					VK_SHARING_MODE_EXCLUSIVE,
					&input_color_image
				);
				shAllocateImageMemory(
					device, physical_device, input_color_image,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					&input_color_image_memory
				);
				shBindImageMemory(
					device, input_color_image, 0, input_color_image_memory
				);
				shCreateImageView(
					device, input_color_image, VK_IMAGE_VIEW_TYPE_2D,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, swapchain_image_format,
					&input_color_image_view
				);

				shCreateRenderpass(device, RENDERPASS_ATTACHMENT_COUNT, attachment_descriptions, 1, &subpass, &renderpass);
				for (uint32_t i = 0; i < swapchain_image_count; i++) {
					VkImageView image_views[RENDERPASS_ATTACHMENT_COUNT] = { input_color_image_view, depth_image_view, swapchain_image_views[i] };
					shCreateFramebuffer(device, renderpass, RENDERPASS_ATTACHMENT_COUNT, image_views, _width, _height, 1, &framebuffers[i]);
				}

				if (swapchain_image_count == 2 && swapchain_image_idx != 0) {//start from swaphain image idx 0
					swapchain_image_idx = 0;
				}

				shGuiDestroyPipelines(p_gui);
				shGuiSetSurface(p_gui, surface);
				shGuiSetRenderpass(p_gui, renderpass);
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
		

		shGuiResetInputs(p_gui);

		shGuiText(
			p_gui,
			(shguivec2) { 0.0f, 100.0f },
			(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
			SH_GUI_WINDOW_TEXT_SIZE * p_gui->char_infos.char_scale_factor,
			"@mrsinho/shgui",
			SH_GUI_PIXELS | SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT
		);

		//shGuiRegion(
		//	p_gui,
		//	(shguivec2) { 80.0f, 80.0f },
		//	(shguivec2) { 20.0f, 20.0f },
		//	(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
		//	(shguivec4) { 1.0f, 0.0f, 0.0f, 1.0f },
		//	SH_GUI_LEFT_MOUSE_BUTTON,
		//	SH_GUI_VEC2_ZERO,
		//	SH_GUI_VEC2_ZERO,
		//	SH_GUI_RELATIVE | SH_GUI_RESIZABLE | SH_GUI_MOVABLE,
		//	0
		//);
		//
		//shGuiRegion(
		//	p_gui,
		//	(shguivec2) { 0.0f, 0.0f },
		//	(shguivec2) { 10.0f, 10.0f },
		//	(shguivec4) { 1.0f, 1.0f, 0.0f, 1.0f },
		//	(shguivec4) { 0.0f, 1.0f, 0.0f, 1.0f },
		//	SH_GUI_LEFT_MOUSE_BUTTON,
		//	SH_GUI_VEC2_ZERO,
		//	SH_GUI_VEC2_ZERO,
		//	SH_GUI_RELATIVE | SH_GUI_MOVABLE | SH_GUI_RESIZABLE | SH_GUI_EDGE_LEFT | SH_GUI_EDGE_TOP,
		//	0
		//);
		//
		//
		shGuiWindow(
			p_gui,
			(shguivec2) { 0.0f, 0.0f },
			(shguivec2) { 20.0f, 50.0f },
			(shguivec4) { 0.0f, 0.5f, 1.0f, 1.0f },
			(shguivec4) { 1.0f, 0.0f, 0.0f, 1.0f },
			(shguivec4) { 0.0f, 0.0f, 1.0f, 1.0f },
			(shguivec4) { 0.0f, 1.0f, 0.0f, 1.0f },
			SH_GUI_LEFT_MOUSE_BUTTON,
			SH_GUI_VEC2_ZERO,
			SH_GUI_VEC2_ZERO,
			SH_GUI_RELATIVE | SH_GUI_EDGE_LEFT | SH_GUI_MOVABLE | SH_GUI_RESIZABLE,
			"1st window",
			(shguivec4) { 0.0f, 1.0f, 1.0f, 1.0f }
		);
		
		shGuiWindow(
			p_gui,
			(shguivec2) { 0.0f, 0.0f },
			(shguivec2) { 20.0f, 50.0f },
			(shguivec4) { 1.0f, 0.5f, 0.0f, 1.0f },
			(shguivec4) { 1.0f, 0.0f, 0.0f, 1.0f },
			(shguivec4) { 0.0f, 0.0f, 1.0f, 1.0f },
			(shguivec4) { 0.0f, 1.0f, 0.0f, 1.0f },
			SH_GUI_LEFT_MOUSE_BUTTON,
			SH_GUI_VEC2_ZERO,
			SH_GUI_VEC2_ZERO,
			SH_GUI_RELATIVE | SH_GUI_MOVABLE | SH_GUI_RESIZABLE | SH_GUI_EDGE_RIGHT,
			"2nd window",
			(shguivec4) { 0.0f, 1.0f, 1.0f, 1.0f }
		);
		if (shGuiWindowButton(
			p_gui, 
			SH_GUI_WINDOW_TEXT_SIZE,
			(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
			SH_GUI_VEC4_ZERO,
			"LMAO",
			SH_GUI_VEC4_ZERO,
			SH_GUI_CENTER_WIDTH,
			SH_GUI_PRESS_ONCE
		)) {
			puts("LMAO");
		}
		shGuiWindowSeparator(
			p_gui,
			SH_GUI_VEC4_ZERO
		);
		float lmao = 0.0f;
		shGuiWindowSliderf(
			p_gui,
			SH_GUI_WINDOW_TEXT_SIZE,
			0.0f, 100.0f,
			"NONE",
			SH_GUI_VEC4_ZERO,
			SH_GUI_VEC4_ZERO,
			(shguivec4) { 1.0f, 1.0f, 1.0f, 1.0f },
			SH_GUI_VEC4_ZERO,
			SH_GUI_VEC4_ZERO,
			&lmao,
			SH_GUI_CENTER_WIDTH
		);
	
		shGuiMenuBar(
			p_gui,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_RED,
			SH_GUI_PIXELS | SH_GUI_EDGE_TOP
		);
		if (shGuiMenuItem(
			p_gui,
			"file",
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_RED,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_BLACK,
			SH_GUI_EDGE_LEFT
		)) {
			puts("File");
		}
		if (shGuiMenuItem(
			p_gui,
			"edit",
			SH_GUI_COLOR_WHITE,
			SH_GUI_COLOR_RED,
			SH_GUI_WINDOW_TEXT_SIZE,
			SH_GUI_COLOR_BLACK,
			SH_GUI_EDGE_LEFT
		)) {
			puts("Edit");
		}

		//shGuiText(
		//	p_gui,
		//	SH_GUI_VEC2_ZERO,
		//	(shguivec4) { 0.0f, 1.0f, 1.0f, 1.0f },
		//	100.0f,
		//	"W",
		//	SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT
		//);

		shGuiUpdateInputs(p_gui);

		shAcquireSwapchainImage(
			device,//device
			swapchain,//swapchain
			UINT64_MAX,//timeout_ns
			current_image_acquired_semaphore,//acquired_signal_semaphore
			NULL,//acquired_signal_fence
			&swapchain_image_idx//p_swapchain_image_index
		);

		shWaitForFences(
			device,//device
			1,//fence_count
			&graphics_cmd_fences[swapchain_image_idx],//p_fences
			1,//wait_for_all
			UINT64_MAX//timeout_ns
		);

		shResetFences(
			device,//device
			1,//fence_count
			&graphics_cmd_fences[swapchain_image_idx]//p_fences
		);

		VkCommandBuffer cmd_buffer = graphics_cmd_buffers[swapchain_image_idx];

		shBeginCommandBuffer(cmd_buffer);




		shGuiWriteMemory(
			p_gui, cmd_buffer, 0
		);

		shGuiSubmitInputs(p_gui);


		VkClearValue clear_values[2] = { 0 };
		float* p_colors = clear_values[0].color.float32;
		p_colors[0] = (float)0.0f;
		p_colors[1] = (float)0.0f;
		p_colors[2] = (float)0.0f;
		float* p_depth = &clear_values[1].depthStencil.depth;
		(*p_depth) = 1.0f;

		shBeginRenderpass(
			cmd_buffer,//graphics_cmd_buffer
			renderpass,//renderpass
			0,//render_offset_x
			0,//render_offset_y
			surface_capabilities.currentExtent.width,//render_size_x
			surface_capabilities.currentExtent.height,//render_size_y
			2,//only attachments with VK_ATTACHMENT_LOAD_OP_CLEAR
			clear_values,//p_clear_values
			framebuffers[swapchain_image_idx]//framebuffer
		);



		shGuiRender(
			p_gui, cmd_buffer,
			swapchain_image_idx
		);


		shEndRenderpass(cmd_buffer);

		shEndCommandBuffer(cmd_buffer);

		shQueueSubmit(
			1,//cmd_buffer_count
			&cmd_buffer,//p_cmd_buffers
			graphics_queue,//queue
			graphics_cmd_fences[swapchain_image_idx],//fence
			1,//semaphores_to_wait_for_count
			&current_image_acquired_semaphore,//p_semaphores_to_wait_for
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,//wait_stage
			1,//signal_semaphore_count
			&current_graphics_queue_finished_semaphore//p_signal_semaphores
		);

		shQueuePresentSwapchainImage(
			present_queue,//present_queue
			1,//semaphores_to_wait_for_count
			&current_graphics_queue_finished_semaphore,//p_semaphores_to_wait_for
			swapchain,//swapchain
			swapchain_image_idx//swapchain_image_idx
		);

		swapchain_image_idx = (swapchain_image_idx + 1) % SWAPCHAIN_IMAGE_COUNT;
	}

	shWaitDeviceIdle(device);

	shGuiRelease(p_gui);

	shDestroySemaphores(device, 1, &current_image_acquired_semaphore);

	shDestroySemaphores(device, 1, &current_graphics_queue_finished_semaphore);

	shDestroyFences(device, SWAPCHAIN_IMAGE_COUNT, graphics_cmd_fences);

	shDestroyCommandBuffers(device, graphics_cmd_pool, swapchain_image_count, graphics_cmd_buffers);

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

	shDestroyFramebuffers(device, swapchain_image_count, framebuffers);

	shDestroyImageViews(device, swapchain_image_count, swapchain_image_views);

	shDestroySwapchain(device, swapchain);

	shDestroyDevice(device);

	shDestroySurface(instance, surface);

	shDestroyInstance(instance);

	return 0;
}

#ifdef __cplusplus
}
#endif//__cplusplus