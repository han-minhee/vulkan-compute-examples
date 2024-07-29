#include "vk_descriptor.hpp"
#include "vk_utils.hpp"
#include <stdexcept>

VkDescriptorPool createDescriptorPool(VkDevice device) {
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize.descriptorCount = 3;

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 1;
	createInfo.pPoolSizes = &poolSize;
	createInfo.maxSets = 1;

	VkDescriptorPool descriptorPool;
	if (vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

	return descriptorPool;
}

VkDescriptorSet createDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout,
	VkBuffer bufferA, VkBuffer bufferB, VkBuffer bufferResult, VkDeviceSize bufferSize) {
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	VkDescriptorSet descriptorSet;
	if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfos[3] = {};
	bufferInfos[0].buffer = bufferA;
	bufferInfos[0].offset = 0;
	bufferInfos[0].range = bufferSize;
	bufferInfos[1].buffer = bufferB;
	bufferInfos[1].offset = 0;
	bufferInfos[1].range = bufferSize;
	bufferInfos[2].buffer = bufferResult;
	bufferInfos[2].offset = 0;
	bufferInfos[2].range = bufferSize;

	VkWriteDescriptorSet descriptorWrites[3] = {};
	for (int i = 0; i < 3; ++i) {
		descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[i].dstSet = descriptorSet;
		descriptorWrites[i].dstBinding = i;
		descriptorWrites[i].dstArrayElement = 0;
		descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[i].descriptorCount = 1;
		descriptorWrites[i].pBufferInfo = &bufferInfos[i];
	}

	vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);

	return descriptorSet;
}
