#pragma once

// #include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

#include <vector>

#include "CommandBuffer.h"

class CommandPool {
private:
	VkDevice& device;
	VkCommandPool commandPool;

public:
	CommandPool(VkPhysicalDevice& physicalDevice, VkDevice& device);
	~CommandPool();
	std::vector<CommandBuffer> allocateCommandBuffers(uint32_t numBuffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};