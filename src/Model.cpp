#include "Model.h"

Model::Model(VkDevice& device, VkPhysicalDevice& physicalDevice, std::vector<Vertex> vertices): device(device), physicalDevice(physicalDevice), vertices(vertices) {
	// vertices = {
	// 	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	// 	};
	// vertices = {
	// 	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

	// 	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	// 	};
	createVertexBuffer();
}

Model::~Model() {
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
}

void Model::bind(VkCommandBuffer& commandBuffer) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void Model::createVertexBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t) bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
}

uint32_t Model::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}