#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>

class Instance {
	VkDebugUtilsMessengerEXT debugMessenger;

public:
	VkInstance instance;
	// VkSurfaceKHR surface;

public:
	Instance() = delete;
	Instance(const std::vector<const char*> validationLayers);
	~Instance();
};