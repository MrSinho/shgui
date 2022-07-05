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

int main(void) {
	
	const char* application_name = "shvulkan example";

	ShVkCore		core	= { 0 };
	uint32_t		width	= 720;
	uint32_t		height	= 480;
	GLFWwindow*		window	= createWindow(width, height, application_name);

	//
	//SHVULKAN BASED CODE
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

	double cursor_pos_x, cursor_pos_y = 0.0;
	glfwGetCursorPos(window, &cursor_pos_x, &cursor_pos_y);

	ShGuiKeyParameters key_parameters = {
		0
	};

	shGuiLinkInputs(&width, &height, (float*)&cursor_pos_x, (float*)&cursor_pos_y, key_parameters, &gui);

	shGuiBuildPipeline(&gui, core.render_pass, 256);

	uint32_t frame_idx;
	for (;!glfwWindowShouldClose(window);) {
		
		shGuiWriteMemory(&gui, 1);

		{//GLFW BASED CODE
			glfwPollEvents();
		}//GLFW BASED CODE

		{//SHVULKAN CODE
			shFrameReset(&core, 0);
			shFrameBegin(&core, 0, &frame_idx);
		}//SHVULKAN CODE

		shGuiWindow(&gui, 60.0f, 30.0f, 0.0f, 0.0f, "my window");

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