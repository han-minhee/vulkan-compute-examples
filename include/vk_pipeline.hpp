#ifndef VK_PIPELINE_HPP
#define VK_PIPELINE_HPP

#include <vulkan/vulkan.h>
#include <string>

VkShaderModule createShaderModule(VkDevice device, const std::string &filename);
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);
VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);
VkPipeline createComputePipeline(VkDevice device, VkShaderModule shaderModule, VkPipelineLayout pipelineLayout);

#endif // VK_PIPELINE_HPP
