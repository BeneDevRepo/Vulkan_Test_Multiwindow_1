#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Vertex.h"

class Model {
	VkDevice& device;
	VkPhysicalDevice& physicalDevice;

public:
    VkDeviceMemory vertexBufferMemory;
	VkBuffer vertexBuffer;
	std::vector<Vertex> vertices;

private:
	void createVertexBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

public:
	Model(VkDevice& device, VkPhysicalDevice& physicalDevice, std::vector<Vertex> vertices);
	~Model();
	void bind(VkCommandBuffer& commandBuffer);
};