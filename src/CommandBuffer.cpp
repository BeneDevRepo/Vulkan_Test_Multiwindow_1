#include "CommandBuffer.h"

// CommandBuffer::CommandBuffer(std::vector<VkCommandBuffer> commandBuffers): commandBuffers(commandBuffers) {
// }
CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer): commandBuffer(commandBuffer) {
}

CommandBuffer::~CommandBuffer() {
}

void CommandBuffer::beginRecording() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");
}

void CommandBuffer::endRecording() {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");
}

// void CommandBuffer::beginRecording(uint32_t bufferIndex, VkRenderPass& renderPass, std::vector<VkFramebuffer>& swapChainFramebuffers, VkExtent2D swapChainExtent) {
// 	VkCommandBufferBeginInfo beginInfo{};
// 	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

// 	if (vkBeginCommandBuffer(commandBuffers[bufferIndex], &beginInfo) != VK_SUCCESS)
// 		throw std::runtime_error("failed to begin recording command buffer!");
// }

// void CommandBuffer::endRecording(uint32_t bufferIndex) {
// 	if (vkEndCommandBuffer(commandBuffers[bufferIndex]) != VK_SUCCESS)
// 		throw std::runtime_error("failed to record command buffer!");
// }