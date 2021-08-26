#pragma once

// #include <vulkan/vulkan.h>
#include "QueueFamilyIndices.h"

#include <vector>

#include "Model.h"

class Pipeline {
	VkDevice& device;

public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

public:
	Pipeline(VkDevice& device, VkRenderPass& renderPass, VkExtent2D swapChainExtent, std::vector<VkImageView>& swapChainImageViews);
	~Pipeline();
	void create(VkRenderPass& renderPass, VkExtent2D swapChainExtent, std::vector<VkImageView>& swapChainImageViews);
	void cleanup();
	void bind(VkCommandBuffer& commandBuffer, VkPipelineBindPoint pipelineBindPoint);
};