#pragma once

// #include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>

#include "QueueFamilyIndices.h"

class CommandBuffer {
	// VkDevice& device;
	// QueueFamilyIndices queueFamilyIndices;

public:
	// VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	// std::vector<VkCommandBuffer> commandBuffers;

public:
	// CommandBuffer(std::vector<VkCommandBuffer> commandBuffers);
	CommandBuffer(VkCommandBuffer commandBuffer);
	~CommandBuffer();

	// void beginRecording(uint32_t bufferIndex, VkRenderPass& renderPass, std::vector<VkFramebuffer>& swapChainFramebuffers, VkExtent2D swapChainExtent);
	// void endRecording(uint32_t bufferIndex);
	void beginRecording();
	void endRecording();
};