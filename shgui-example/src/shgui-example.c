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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

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

	ShGuiCore gui_core = { 
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

	ShGui* p_gui = shGuiInit(gui_core);
	
	float cursor_pos_x, cursor_pos_y = 0.0;// update in realtime
	ShGuiKeyEvents key_events = {// update in realtime
		0
	};
	ShGuiMouseEvents mouse_events = {// update in realtime
		0
	};
	ShGuiCursorIcons cursor_icons = {//update in realtime
		GLFW_CURSOR_NORMAL, GLFW_HRESIZE_CURSOR, GLFW_VRESIZE_CURSOR //normal, horizontal_resize, vertical_resize
	};
	float delta_time = 0.0f;
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

	shGuiBuildRegionPipeline(p_gui, core.render_pass, MAX_GUI_ITEMS);
	shGuiBuildTextPipeline(p_gui, core.render_pass, MAX_GUI_ITEMS);

	shGuiSetDefaultValues(p_gui, SH_GUI_THEME_LIGHT, SH_GUI_INITIALIZE | SH_GUI_RECORD);

	uint32_t frame_idx;
	float last_time = (float)glfwGetTime();
	
	
	uint8_t window_1_active = 0;

	float hey = 100.0f;
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
			float now = (float)glfwGetTime();
			delta_time = now - last_time;
			last_time = now;

			GLFWcursor* cursor = glfwCreateStandardCursor((int)p_gui->inputs.active_cursor_icon);
			glfwSetCursor(window, cursor);

			//glfwGetWindowSize(window, &width, &height);
			//Resize window
			//if (width != p_gui->region_infos.fixed_states.scissor.extent.width || height != p_gui->region_infos.fixed_states.scissor.extent.height) {
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
			//	shGuiDestroyPipelines(p_gui);
			//	shGuiBuildRegionPipeline(p_gui, core.render_pass, MAX_GUI_ITEMS);
			//	shGuiBuildTextPipeline(p_gui, core.render_pass, MAX_GUI_ITEMS);
			//
			//	shGuiSetDefaultValues(p_gui, SH_GUI_THEME_DARK, SH_GUI_INITIALIZE | SH_GUI_RECORD);
			//}

		}//GLFW BASED CODE



		double d_cursor_pos_x, d_cursor_pos_y = 0.0;
		glfwGetCursorPos(window, &d_cursor_pos_x, &d_cursor_pos_y);

		cursor_pos_x = (float)d_cursor_pos_x - ((float)(width) / 2.0f);
		cursor_pos_y = (float)d_cursor_pos_y - ((float)(height) / 2.0f);


		if (shGuiRegion(
			p_gui,
			30.0f, 20.0f,
			0.0f, 0.0f,
			"QUERTY",
			SH_GUI_MOVABLE | SH_GUI_RELATIVE | SH_GUI_RESIZABLE
		)) {
			puts("region clicked!");
			p_gui->region_infos.p_regions_active[p_gui->region_infos.region_count] = 1 * (p_gui->region_infos.p_regions_active[p_gui->region_infos.region_count] == 0);
		}
		if (p_gui->region_infos.p_regions_active[p_gui->region_infos.region_count]) {
			if (shGuiRegion(p_gui, 100.0f, 100.0f, 200.0f, 100.0f, NULL, SH_GUI_MOVABLE | SH_GUI_PIXELS | SH_GUI_MINIMIZABLE | SH_GUI_RESIZABLE)) {
				puts("another region clicked!");
				p_gui->region_infos.region_count--;
			}
		}
		p_gui->region_infos.region_count++;


		shGuiMenuBar(p_gui, 5.0f, "POWER", SH_GUI_TOP);

		
		ShGuiRegion* p_reg_0 = &p_gui->region_infos.p_regions_data[0];
		ShGuiRegion* p_reg_1 = &p_gui->region_infos.p_regions_data[1];
		ShGuiRegion* p_reg_2 = &p_gui->region_infos.p_regions_data[2];

		shGuiText(p_gui, 25.0f, 0.0f, 0.0f, "QWERTY");
		shGuiText(p_gui, 50.0f, 0.0f, -50.0f, "QWERTY");
		shGuiText(p_gui, 100.0f, 0.0f, -150.0f, "QWERTY");
		shGuiText(p_gui, 200.0f, 0.0f, -300.0f, "QWERTY");


		shGuiRegion(
			p_gui,
			300.0f, 100.0f,
			-200.0f, -200.0f,
			"TROYU",
			SH_GUI_MOVABLE | SH_GUI_PIXELS | SH_GUI_RESIZABLE
		);
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



		shGuiRender(p_gui);
		shGuiUpdateInputs(p_gui);


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