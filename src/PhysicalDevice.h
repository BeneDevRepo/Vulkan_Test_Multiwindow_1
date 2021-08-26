#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

class PhysicalDevice {
public:
	VkPhysicalDevice physicalDevice;

public:
	PhysicalDevice(VkInstance& instance);
	~PhysicalDevice();
};