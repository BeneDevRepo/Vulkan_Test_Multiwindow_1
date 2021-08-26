#pragma once

// #include <vulkan/vulkan.h>
#include "QueueFamilyIndices.h"

#include <vector>

class Swapchain {
	VkDevice& device;

public:
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

public:
	Swapchain(VkPhysicalDevice& physicalDevice, VkDevice& device, VkSurfaceKHR& surface, VkExtent2D windowSize, uint32_t numSwapchainImages);
	~Swapchain();
	void create(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkExtent2D windowSize, uint32_t numSwapchainImages);
	void cleanup();
};