#ifndef VK_BUFFER_HPP
#define VK_BUFFER_HPP

#include <vulkan/vulkan.h>
#include <functional>

void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
void initializeBufferData(VkDevice device, VkDeviceMemory bufferMemory, uint32_t vectorSize, std::function<float(int)> initFunction);
void readBufferData(VkDevice device, VkDeviceMemory bufferMemory, uint32_t vectorSize);

#endif // VK_BUFFER_HPP
