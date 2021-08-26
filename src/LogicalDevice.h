#pragma once

#include "QueueFamilyIndices.h"

#include <vector>

// #include <vulkan/vulkan.h>

class LogicalDevice {
public:
	VkDevice device;

public:
	LogicalDevice(VkPhysicalDevice& physicalDevice, const std::vector<const char*> validationLayers, const std::vector<const char*> deviceExtensions);
	~LogicalDevice();
};