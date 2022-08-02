#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <shgui/shgui.h>
#include <shvulkan/shVkCore.h>
#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkMemoryInfo.h>
#include <shvulkan/shVkDrawLoop.h>

#include <GLFW/glfw3.h>

GLFWwindow* createWindow(const uint32_t width, const uint32_t height, const char* title);

const char* readBinary(const char* path, uint32_t* p_size);

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

#define MAX_GUI_ITEMS 256

int main(void) {
	
	uint32_t width = WINDOW_WIDTH;
	uint32_t height = WINDOW_HEIGHT;

	const char* application_name = "shgui example";

	ShVkCore		core	= { 0 };
	GLFWwindow*		window	= createWindow(width, height, application_name);

	//
	//SHVULKAN BASED CODE, write your own
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
		//shInitDepthData(&core);
		shCreateRenderPass(&core);
		shSetFramebuffers(&core);
		shCreateGraphicsCommandBuffers(&core, 1);
		shSetSyncObjects(&core);
		shGetGraphicsQueue(&core);
	}

	//
	//SHGUI BASED CODE
	//

	ShGui gui = { 
		core.device,									//
		core.physical_device,							//
		(ShGuiQueue) {									//vulkan based data
			core.graphics_queue.queue_family_index,		//
			core.graphics_queue.queue					//
		},												//
		core.p_graphics_commands[0].cmd_buffer,			//
		core.p_graphics_commands[0].fence,				//
		core.surface.surface,							//
	};


	
	float cursor_pos_x, cursor_pos_y = 0.0;// update in realtime
	ShGuiKeyEvents key_events = {// update in realtime
		0
	};
	ShGuiMouseEvents mouse_events = {// update in realtime
		0
	};
	float delta_time = 0.0f;
	shGuiLinkInputs(
		&width, 
		&height, 
		&cursor_pos_x, 
		&cursor_pos_y, 
		key_events, 
		mouse_events, 
		&delta_time,
		&gui
	);

	shGuiBuildRegionPipeline(&gui, core.render_pass, MAX_GUI_ITEMS);
	shGuiBuildTextPipeline(&gui, core.render_pass, MAX_GUI_ITEMS);

	shGuiSetDefaultValues(&gui, SH_GUI_THEME_DARK, SH_GUI_INITIALIZE | SH_GUI_RECORD);

	uint32_t frame_idx;
	float last_time = (float)glfwGetTime();
	
	
	uint8_t window_1_active = 0;
	for (;!glfwWindowShouldClose(window);) {
		
		{//GLFW BASED CODE
			glfwPollEvents();
			//LINK INPUTS
			for (uint32_t key_idx = 0; key_idx < SH_GUI_KEY_LAST + 1; key_idx++) {
				key_events[key_idx] = glfwGetKey(window, key_idx);
			}
			for (uint32_t mouse_button_idx = 0; mouse_button_idx < SH_GUI_MOUSE_LAST + 1; mouse_button_idx++) {
				mouse_events[mouse_button_idx] = glfwGetMouseButton(window, mouse_button_idx);
			}
			float now = (float)glfwGetTime();
			delta_time = now - last_time;
			last_time = now;

			glfwGetWindowSize(window, &width, &height);
			//Resize window
			//if (width != gui.region_infos.fixed_states.scissor.extent.width || height != gui.region_infos.fixed_states.scissor.extent.height) {
			//	shWaitDeviceIdle(core.device);
			//	
			//	shSwapchainRelease(&core);
			//	shSurfaceRelease(&core);
			//
			//	glfwCreateWindowSurface(core.instance, window, NULL, &core.surface.surface);
			//	core.surface.width = width;
			//	core.surface.height = height;
			//	shInitSwapchainData(&core);
			//	shSetFramebuffers(&core);
			//
			//	shGuiDestroyPipelines(&gui);
			//	shGuiBuildRegionPipeline(&gui, core.render_pass, MAX_GUI_ITEMS);
			//	shGuiBuildTextPipeline(&gui, core.render_pass, MAX_GUI_ITEMS);
			//
			//	shGuiSetDefaultValues(&gui, SH_GUI_THEME_DARK, SH_GUI_INITIALIZE | SH_GUI_RECORD);
			//}

		}//GLFW BASED CODE



		double d_cursor_pos_x, d_cursor_pos_y = 0.0;
		glfwGetCursorPos(window, &d_cursor_pos_x, &d_cursor_pos_y);

		cursor_pos_x = (float)d_cursor_pos_x - ((float)(width) / 2.0f);
		cursor_pos_y = (float)d_cursor_pos_y - ((float)(height) / 2.0f);


		SH_GUI_REGION_CONDITION(
			gui,
			shGuiRegion(
				&gui,
				30.0f, 20.0f,
				-50.0f, 20.0f,
				"QUERTY"
			),

			puts("region clicked!");
			
			,

			if (shGuiRegion(
				&gui,
				10.0f, 10.0f,
				70.0f, 70.0f,
				"UIOP"
			)) {
				puts("another region clicked!");
			}
		);

		shGuiText(&gui, "QWERTY", 3.0f, -60.0f, 0.0f);
		shGuiText(&gui, "UIOP", 3.0f, -60.0f, -30.0f);

		shGuiWriteMemory(&gui, 1);


		{//SHVULKAN CODE, write your own
			shFrameReset(&core, 0);
			float v = 0.5f;
			shFrameBegin(
				&core, 
				0,
				(VkClearColorValue){ { v, v, v, 1.0f } },
				&frame_idx
			);
		}//SHVULKAN CODE, write your own



		shGuiRender(&gui);



		{//SHVULKAN CODE
			shFrameEnd(&core, 0, frame_idx);
		}//SHVULKAN CODE

	}

	shGuiRelease(&gui);

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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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