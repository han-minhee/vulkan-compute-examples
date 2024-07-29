#ifndef VK_UTILS_HPP
#define VK_UTILS_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

std::vector<char> readFile(const std::string& filename);
std::vector<const char*> getInstanceExtensions();
std::vector<const char*> getDeviceExtensions();
void checkInstanceExtensions(const std::vector<const char*>& requiredExtensions);
void checkDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions);
uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void cleanup(VkDevice device, VkInstance instance, VkPipeline pipeline, VkPipelineLayout pipelineLayout,	VkShaderModule computeShaderModule, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,	VkCommandPool commandPool, std::vector<VkBuffer> buffers, std::vector<VkDeviceMemory> bufferMemories);

#endif // VK_UTILS_HPP
