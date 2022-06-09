#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <shgui/shgui.h>
#include <shvulkan/shVkCore.h>
#include <shvulkan/shVkPipelineData.h>
#include <shvulkan/shVkMemoryInfo.h>

#include <GLFW/glfw3.h>

GLFWwindow* createWindow(const uint32_t width, const uint32_t height, const char* title);

const char* readBinary(const char* path, uint32_t* p_size);

#define THREAD_COUNT 1
#define VALIDATION_LAYERS_ENABLED 1

int main(void) {
	
	const char* application_name = "shvulkan example";

	ShVkCore		core	= { 0 };
	const uint32_t	width	= 720;
	const uint32_t	height	= 480;
	GLFWwindow*		window	= createWindow(width, height, application_name);

	//
	//SHVULKAN BASED CODE
	//
	{
		uint32_t extension_count = 2;
		const char** extension_names = glfwGetRequiredInstanceExtensions(&extension_count);
		printf("required instance extensions:\n");
		for (uint32_t i = 0; i < extension_count; i++) {
			printf("%s\n", extension_names[i]);
		}
		shCreateInstance(&core, application_name, "shvulkan engine", VALIDATION_LAYERS_ENABLED, extension_count, extension_names);
		shVkAssertResult(
			glfwCreateWindowSurface(core.instance, window, NULL, &core.surface.surface),
			"error creating window surface"
		);
		core.surface.width = width;
		core.surface.height = height;
		shSelectPhysicalDevice(&core, SH_VK_CORE_GRAPHICS);
		shSetLogicalDevice(&core);
		shInitSwapchainData(&core);
		shInitDepthData(&core);
		shCreateRenderPass(&core);
		shSetFramebuffers(&core);
		shCreateGraphicsCommandBuffers(&core, THREAD_COUNT);
		shSetSyncObjects(&core);
		shGetGraphicsQueue(&core);
	}

	//
	//SHGUI BASED CODE
	//

	ShGui gui = { 
		core.device,									//
		(ShGuiQueue) {									//vulkan based data
			core.graphics_queue.queue_family_index,		//
			core.graphics_queue.queue					//
		},												//
		VK_NULL_HANDLE,
		{ 0 } //use shGuiLinkInputs
	};

	double cursor_pos_x, cursor_pos_y = 0.0;
	glfwGetCursorPos(window, &cursor_pos_x, &cursor_pos_y);

	shGuiLinkInputs(&width, height, (float*)cursor_pos_x, (float*)cursor_pos_y, , &gui);

	for (;glfwWindowShouldClose(window);) {

	}
	return 0;
}

GLFWwindow* createWindow(const uint32_t width, const uint32_t height, const char* title) {
	assert(glfwInit());
	assert(glfwVulkanSupported() != GLFW_FALSE);
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