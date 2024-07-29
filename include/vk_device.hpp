#ifndef VK_DEVICE_HPP
#define VK_DEVICE_HPP

#include <vulkan/vulkan.h>
#include <vector>

VkDevice createLogicalDevice(VkInstance instance, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue);
void checkDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions);

#endif // VK_DEVICE_HPP
