#include "vk_buffer.hpp"
#include "vk_utils.hpp"
#include <stdexcept>
#include <iostream>

void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void initializeBufferData(VkDevice device, VkDeviceMemory bufferMemory, uint32_t vectorSize, std::function<float(int)> initFunction) {
	void* data;
	vkMapMemory(device, bufferMemory, 0, vectorSize * sizeof(float), 0, &data);
	float* mappedData = static_cast<float*>(data);
	for (uint32_t i = 0; i < vectorSize; ++i) {
		mappedData[i] = initFunction(i);
	}
	vkUnmapMemory(device, bufferMemory);
}

void readBufferData(VkDevice device, VkDeviceMemory bufferMemory, uint32_t vectorSize) {
	void* data;
	vkMapMemory(device, bufferMemory, 0, vectorSize * sizeof(float), 0, &data);
	float* mappedData = static_cast<float*>(data);
	for (uint32_t i = 0; i < vectorSize; ++i) {
		std::cout << mappedData[i] << " ";
	}
	std::cout << std::endl;
	vkUnmapMemory(device, bufferMemory);
}
