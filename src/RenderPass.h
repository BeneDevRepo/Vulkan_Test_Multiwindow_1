#pragma once

#include <vulkan/vulkan.h>

class RenderPass {
	VkDevice& device;

public:
	VkRenderPass renderPass;

public:
	RenderPass(VkDevice& device, VkFormat& swapChainImageFormat);
	~RenderPass();
	void create(VkFormat& swapChainImageFormat);
	void cleanup();
	void begin(VkCommandBuffer& commandBuffer, VkFramebuffer& frameBuffer, VkExtent2D frameSize, VkClearColorValue color);
	void end(VkCommandBuffer& commandBuffer);
};