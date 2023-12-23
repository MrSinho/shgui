#ifndef SH_GUI_APP_H
#define SH_GUI_APP_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#include <shgui/shgui.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define SH_GUI_APP_SWAPCHAIN_IMAGE_COUNT       1
#define SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT   6
#define SH_GUI_APP_RENDERPASS_ATTACHMENT_COUNT 3


typedef struct ShGuiApp {
	VkInstance                       instance;
											                                      
	VkSurfaceKHR                     surface;
	VkSurfaceCapabilitiesKHR         surface_capabilities;
																	              
	VkPhysicalDevice                 physical_device;            
	VkPhysicalDeviceProperties       physical_device_properties;            
	VkPhysicalDeviceFeatures         physical_device_features;            
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
																	              
	uint32_t                         graphics_queue_family_index;
	uint32_t                         present_queue_family_index;
																	              
	VkDevice                         device;
	uint32_t                         device_extension_count;
															                      
	VkQueue                          graphics_queue;
	VkQueue                          present_queue;
					           						                              
	VkCommandPool                    graphics_cmd_pool;                            
	VkCommandPool                    present_cmd_pool;
															                      
	VkCommandBuffer                  graphics_cmd_buffers[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT];  
	VkCommandBuffer                  present_cmd_buffer;
															                      
	VkFence                          graphics_cmd_fences [SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT];
															                      
	VkSemaphore                      current_image_acquired_semaphore;    
	VkSemaphore                      current_graphics_queue_finished_semaphore;

	VkSwapchainKHR                   swapchain;                       
	VkSharingMode                    swapchain_image_sharing_mode;
	VkFormat                         swapchain_image_format;                       
	uint32_t                         swapchain_image_count;
																	              
	uint32_t                         sample_count;

	VkAttachmentDescription          input_color_attachment;             
	VkAttachmentReference            input_color_attachment_reference;             
	VkAttachmentDescription          depth_attachment;             
	VkAttachmentReference            depth_attachment_reference;             
	VkAttachmentDescription          resolve_attachment;             
	VkAttachmentReference            resolve_attachment_reference;             
	VkSubpassDescription             subpass;
																	              
	VkRenderPass                     renderpass;

	VkImage                          swapchain_images     [SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]; 
	VkImageView                      swapchain_image_views[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT]; 
	VkImage                          depth_image;
	VkDeviceMemory                   depth_image_memory; 
	VkImageView                      depth_image_view; 
	VkImage                          input_color_image; 
	VkDeviceMemory                   input_color_image_memory; 
	VkImageView                      input_color_image_view; 

	VkFramebuffer                    framebuffers[SH_GUI_APP_MAX_SWAPCHAIN_IMAGE_COUNT];

	uint32_t                         swapchain_image_idx;

	/*
		ShGui structures and data below
	*/
	ShGui                            gui;

	uint32_t                         width;
	uint32_t                         height;
	GLFWwindow*                      window;

	double                           d_cursor_pos_x;
	double                           d_cursor_pos_y;
	float                            cursor_pos_x;
	float                            cursor_pos_y;
					                 
	ShGuiKeyEvents                   key_events;
	ShGuiKeyEvents                   last_key_events;
					                 
	double                           last_time;
	double                           delta_time;

} ShGuiApp;

#define shAllocateGuiApp()  ((ShGuiApp*)calloc(1, sizeof(ShGuiApp)))

#define shGuiFreeApp(p_app) free(p_app)



extern ShGuiApp* SH_GUI_CALL shGuiCreateApp(
	uint32_t    window_width,
	uint32_t    window_height,
	const char* application_name
);

extern uint8_t SH_GUI_CALL shGuiReleaseApp(
	ShGuiApp* p_app
);

extern uint8_t SH_GUI_CALL shGuiAppRunning(
	ShGuiApp* p_app
);

extern uint8_t SH_GUI_CALL shGuiAppCheckWindowSize(
	ShGuiApp* p_app
);

extern uint8_t SH_GUI_CALL shGuiAppResizeWindow(
	ShGuiApp* p_app
);

extern uint8_t SH_GUI_CALL shGuiAppUpdate(
	ShGuiApp* p_app
);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_APP_H