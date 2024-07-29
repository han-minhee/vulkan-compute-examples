#ifndef VK_COMMAND_HPP
#define VK_COMMAND_HPP

#include <vulkan/vulkan.h>

VkCommandPool createCommandPool(VkDevice device);
VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool);
void recordCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout, 
                         VkDescriptorSet descriptorSet, uint32_t width, uint32_t height);
void submitCommandBuffer(VkDevice device, VkCommandBuffer commandBuffer);

#endif // VK_COMMAND_HPP
