#ifndef SH_GUI_ASSERT_H
#define SH_GUI_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "shgui/shgui-call.h"

#include <assert.h>
#include <stdio.h>

#include <vulkan/vulkan.h>

static const char* SH_GUI_CALL shGuiTranslateVkResult(const VkResult result) {
	switch (result) {
	case VK_SUCCESS:return "VK_SUCCESS";
	case VK_NOT_READY:return "VK_NOT_READY";
	case VK_TIMEOUT:return "VK_TIMEOUT";
	case VK_EVENT_SET:return "VK_EVENT_SET";
	case VK_EVENT_RESET:return "VK_EVENT_RESET";
	case VK_INCOMPLETE:return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_SURFACE_LOST_KHR:return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_SUBOPTIMAL_KHR:return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_RESULT_MAX_ENUM:return "VK_RESULT_MAX_ENUM";
	}
	return "unknown vkresult";
}

static void SH_GUI_CALL shGuiAssert(int condition, const char* error_msg) {
	if (!condition) {
		printf("shgui error: %s\n", error_msg);
	}
	assert(condition && error_msg);
}
static void SH_GUI_CALL shGuiAssertVkResult(VkResult result, const char* error_msg) {
	if (result != VK_SUCCESS) {
		printf("shgui error: %s %s\n", error_msg, shGuiTranslateVkResult(result));
	}
	assert(result == VK_SUCCESS && error_msg);
}


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//SH_GUI_ASSERT_H