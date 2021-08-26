#include "CommandPool.h"

// #include "QueueFamilyIndices.h"

CommandPool::CommandPool(VkPhysicalDevice& physicalDevice, VkDevice& device): device(device) {
	QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(physicalDevice);

	// createCommandPool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}

CommandPool::~CommandPool() {
	vkDestroyCommandPool(device, commandPool, nullptr);
}

std::vector<CommandBuffer> CommandPool::allocateCommandBuffers(uint32_t numBuffers, VkCommandBufferLevel level) {
	std::vector<VkCommandBuffer> commandBufferHandles(numBuffers);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = numBuffers;

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBufferHandles.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");

	std::vector<CommandBuffer> commandBuffers;
	commandBuffers.reserve(numBuffers);
	for(uint32_t i=0; i<numBuffers; i++)
		commandBuffers.push_back({commandBufferHandles[i]});

	return commandBuffers;
}