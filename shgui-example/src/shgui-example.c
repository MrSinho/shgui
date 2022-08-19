#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



#include <shvulkan/shVkCore.h>
#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkMemoryInfo.h>
#include <shvulkan/shVkDrawLoop.h>

#include <GLFW/glfw3.h>



//ONLY FOR DEV, COMMENT IF 
//
//
#define SH_GUI_DEBUG_SHADERS 1
#include <shgui/shgui.h>



GLFWwindow* createWindow(const uint32_t width, const uint32_t height, const char* title);

const char* readBinary(const char* path, uint32_t* p_size);

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MAX_GUI_ITEMS 256

int main(void) {
	

	uint32_t width					= WINDOW_WIDTH;
	uint32_t height					= WINDOW_HEIGHT;


	const char* application_name	= "shgui example";


	ShVkCore		core			= { 0 };
	GLFWwindow*		window			= createWindow(width, height, application_name);


	//SHVULKAN BASED CODE, not related to shgui
	//
	//
	{
		uint32_t extension_count = 2;
		const char** extension_names = glfwGetRequiredInstanceExtensions(&extension_count);
		shCreateInstance(&core, application_name, "shvulkan engine", 1, extension_count, extension_names);
		shVkError(
			glfwCreateWindowSurface(core.instance, window, NULL, &core.surface.surface),
			"error creating window surface",
			return -1;
		);
		core.surface.width = width;
		core.surface.height = height;
		shSelectPhysicalDevice(&core, VK_QUEUE_GRAPHICS_BIT);
		shSetLogicalDevice(&core);
		shInitSwapchainData(&core);
		shInitDepthData(&core);
		shCreateRenderPass(&core);
		shSetFramebuffers(&core);
		shCreateGraphicsCommandBuffers(&core, 1);
		shSetSyncObjects(&core);
		shGetGraphicsQueue(&core);
	}

	//
	//SHGUI BASED CODE
	//

	ShGuiCore gui_core = { 
		core.device,									
		core.physical_device,							
		(ShGuiQueue) {									
			core.graphics_queue.queue_family_index,		
			core.graphics_queue.queue					
		},												
		core.p_graphics_commands[0].cmd_buffer,			
		core.p_graphics_commands[0].fence,				
		core.surface.surface,
		core.render_pass
	};
	ShGui* p_gui = shGuiInit(gui_core);
	
	float cursor_pos_x, cursor_pos_y	= 0.0;// update in realtime
	ShGuiKeyEvents key_events			= { 0 };
	ShGuiMouseEvents mouse_events		= { 0 };
	ShGuiCursorIcons cursor_icons		= {
		GLFW_CURSOR_NORMAL, GLFW_HRESIZE_CURSOR, GLFW_VRESIZE_CURSOR //normal, horizontal_resize, vertical_resize
	};
	double delta_time = 0.0f;
	shGuiLinkInputs(
		&width, 
		&height, 
		&cursor_pos_x, 
		&cursor_pos_y, 
		key_events, 
		mouse_events,
		cursor_icons,
		&delta_time,
		p_gui
	);

	shGuiBuildRegionPipeline(p_gui, MAX_GUI_ITEMS);
	shGuiBuildTextPipeline(p_gui, MAX_GUI_ITEMS);

	shGuiSetDefaultValues(p_gui, SH_GUI_THEME_EXTRA_DARK, SH_GUI_INITIALIZE | SH_GUI_RECORD);

	uint32_t frame_idx	= 0;
	double last_time	= glfwGetTime();
	
	while (!glfwWindowShouldClose(window)) {
		
		{//GLFW BASED CODE
			glfwPollEvents();
			//LINK INPUTS
			for (uint32_t key_idx = 0; key_idx < SH_GUI_KEY_LAST + 1; key_idx++) {
				key_events[key_idx] = glfwGetKey(window, key_idx);
			}
			for (uint32_t mouse_button_idx = 0; mouse_button_idx < SH_GUI_MOUSE_LAST + 1; mouse_button_idx++) {
				mouse_events[mouse_button_idx] = glfwGetMouseButton(window, mouse_button_idx);
			}
			double now = glfwGetTime();
			delta_time = now - last_time;
			last_time = now;

			GLFWcursor* cursor = glfwCreateStandardCursor((int)p_gui->inputs.active_cursor_icon);
			glfwSetCursor(window, cursor);
			p_gui->inputs.active_cursor_icon = GLFW_CROSSHAIR_CURSOR;

			glfwGetWindowSize(window, &width, &height);
			if (width != p_gui->region_infos.fixed_states.scissor.extent.width || height != p_gui->region_infos.fixed_states.scissor.extent.height) {
				
				//shvulkan and glfw based code, write your own
				//
				//
				shWaitDeviceIdle(core.device);
				shSwapchainRelease(&core);
				shDepthBufferRelease(&core);
				shSurfaceRelease(&core);
				glfwCreateWindowSurface(core.instance, window, NULL, &core.surface.surface);
				gui_core.surface = core.surface.surface;
				p_gui->core.surface = core.surface.surface;
				core.surface.width = width;
				core.surface.height = height;
				shInitSwapchainData(&core);
				shInitDepthData(&core);
				shSetFramebuffers(&core);
				
				//update shgui canvas
				//
				//
				shGuiDestroyPipelines(p_gui);
				shGuiBuildRegionPipeline(p_gui, MAX_GUI_ITEMS);
				shGuiBuildTextPipeline(p_gui, MAX_GUI_ITEMS);
				shGuiSetDefaultValues(p_gui, p_gui->default_infos.default_values, SH_GUI_RECORD);
			}

		}//GLFW BASED CODE



		double d_cursor_pos_x, d_cursor_pos_y = 0.0;
		glfwGetCursorPos(window, &d_cursor_pos_x, &d_cursor_pos_y);

		cursor_pos_x = (float)d_cursor_pos_x - ((float)(width) / 2.0f);
		cursor_pos_y = (float)d_cursor_pos_y - ((float)(height) / 2.0f);


		shGuiWindow(
			p_gui,
			30.0f, 20.0f,
			0.0f, 0.0f,
			"Menu",
			SH_GUI_MOVABLE | SH_GUI_RELATIVE | SH_GUI_RESIZABLE
		);
		shGuiWindowText(p_gui, SH_GUI_WINDOW_TEXT_SIZE, "Menu text", SH_GUI_CENTER_WIDTH);
		shGuiWindowText(p_gui, SH_GUI_WINDOW_TEXT_SIZE, "Some info here", 0);
		shGuiWindowSeparator(p_gui);
		if (shGuiWindowButton(p_gui, SH_GUI_WINDOW_TEXT_SIZE * 2.0f, "Popup", SH_GUI_CENTER_WIDTH)) {
			puts("Popup");
		}



		shGuiWindow(
			p_gui,
			300.0f, 100.0f,
			-200.0f, -200.0f,
			"Window",
			SH_GUI_MOVABLE | SH_GUI_PIXELS | SH_GUI_RESIZABLE
		);
		shGuiWindowText(p_gui, SH_GUI_WINDOW_TEXT_SIZE * 2.0f, "0123456789", SH_GUI_CENTER_WIDTH);



		shGuiWindow(
			p_gui,
			350.0f, 100.0f,
			200.0f, -200.0f,
			"Another window",
			SH_GUI_MOVABLE | SH_GUI_PIXELS | SH_GUI_RESIZABLE
		);
		shGuiWindowText(p_gui, SH_GUI_WINDOW_TEXT_SIZE, "an email: lmao555@gmail.com", 0);
		shGuiWindowText(p_gui, SH_GUI_WINDOW_TEXT_SIZE * 1.5f, "|!\"£$%&/=?*@#,.-;:_", 0);



		shGuiMenuBar(p_gui, SH_GUI_WINDOW_BAR_SIZE * 1.5f, SH_GUI_TOP);
		if (shGuiMenuItem(p_gui, 0.0f, "File", 0)) {
			puts("File");
		}
		if (shGuiMenuItem(p_gui, 0.0f, "Settings", 0)) {
			puts("Settings");
		}

		
		//shGuiText(p_gui, 25.0f, 0.0f, 0.0f, "QWERTY", SH_GUI_CENTER_WIDTH | SH_GUI_CENTER_HEIGHT);
		//shGuiText(p_gui, 50.0f, 0.0f, -50.0f, "QWERTY");
		//shGuiText(p_gui, 100.0f, 0.0f, -150.0f, "QWERTY");
		//shGuiText(p_gui, 200.0f, 0.0f, -300.0f, "QWERTY");


		
		shGuiWriteMemory(p_gui, 1);


		{//SHVULKAN CODE, write your own
			shFrameReset(&core, 0);
			float v = 0.5f;
			shFrameBegin(
				&core, 
				0,
				(VkClearColorValue){ { 1.0, v, v, 1.0f } },
				&frame_idx
			);
		}//SHVULKAN CODE, write your own



		shGuiUpdateInputs(p_gui);
		shGuiRender(p_gui);


		{//SHVULKAN CODE
			shFrameEnd(&core, 0, frame_idx);
		}//SHVULKAN CODE

	}

	shGuiRelease(p_gui);

	{//GLFW CODE
		glfwTerminate();
	}//GLFW CODE

	{//SHVULKAN CODE
		shVulkanRelease(&core);
	}//SHVULKAN CODE



	return 0;
}

GLFWwindow* createWindow(const uint32_t width, const uint32_t height, const char* title) {
	shVkError(!glfwInit(), "error initializing glfw", return NULL);
	shVkError(glfwVulkanSupported() == GLFW_FALSE, "vulkan not supported by glfw", return NULL);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	return glfwCreateWindow(width, height, title, NULL, NULL);
}

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif//_MSC_VER
#include <stdlib.h>
const char* readBinary(const char* path, uint32_t* p_size) {
	FILE* stream = fopen(path, "rb");
	if (stream == NULL) {
		return NULL;
	}
	fseek(stream, 0, SEEK_END);
	uint32_t code_size = ftell(stream);
	fseek(stream, 0, SEEK_SET);
	char* code = (char*)calloc(1, code_size);
	if (code == NULL) {
		fclose(stream);
		return NULL;
	}
	fread(code, code_size, 1, stream);
	*p_size = code_size;
	fclose(stream);
	return code;
}

#ifdef __cplusplus
}
#endif//__cplusplus