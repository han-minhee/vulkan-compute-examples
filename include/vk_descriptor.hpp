#ifndef VK_DESCRIPTOR_HPP
#define VK_DESCRIPTOR_HPP

#include <vulkan/vulkan.h>

VkDescriptorPool createDescriptorPool(VkDevice device);
VkDescriptorSet createDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkBuffer bufferA, VkBuffer bufferB, VkBuffer bufferResult, VkDeviceSize bufferSize);

#endif // VK_DESCRIPTOR_HPP
