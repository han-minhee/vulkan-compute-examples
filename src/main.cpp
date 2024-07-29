#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_buffer.hpp"
#include "vk_pipeline.hpp"
#include "vk_descriptor.hpp"
#include "vk_command.hpp"
#include "vk_utils.hpp"
#include <iostream>

const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 1;
const uint32_t VECTOR_SIZE = WIDTH * HEIGHT;

int main()
{
	VkInstance instance = createInstance();
	VkPhysicalDevice physicalDevice;
	VkQueue graphicsQueue;
	VkDevice device = createLogicalDevice(instance, physicalDevice, graphicsQueue);

	VkBuffer bufferA, bufferB, bufferResult;
	VkDeviceMemory bufferMemoryA, bufferMemoryB, bufferMemoryResult;
	createBuffer(device, physicalDevice, VECTOR_SIZE * sizeof(float), bufferA, bufferMemoryA);
	createBuffer(device, physicalDevice, VECTOR_SIZE * sizeof(float), bufferB, bufferMemoryB);
	createBuffer(device, physicalDevice, VECTOR_SIZE * sizeof(float), bufferResult, bufferMemoryResult);

	initializeBufferData(device, bufferMemoryA, VECTOR_SIZE, [](int i) { return static_cast<float>(i); });
	initializeBufferData(device, bufferMemoryB, VECTOR_SIZE, [](int i) { return static_cast<float>(2 * i); });

	VkShaderModule computeShaderModule = createShaderModule(device, "./kernels/vector_add.comp.spv");
	VkDescriptorSetLayout descriptorSetLayout = createDescriptorSetLayout(device);
	VkPipelineLayout pipelineLayout = createPipelineLayout(device, descriptorSetLayout);
	VkPipeline pipeline = createComputePipeline(device, computeShaderModule, pipelineLayout);

	VkDescriptorPool descriptorPool = createDescriptorPool(device);
	VkDescriptorSet descriptorSet = createDescriptorSet(device, descriptorPool, descriptorSetLayout, bufferA, bufferB, bufferResult, VECTOR_SIZE * sizeof(float));

	VkCommandPool commandPool = createCommandPool(device);
	VkCommandBuffer commandBuffer = createCommandBuffer(device, commandPool);
	recordCommandBuffer(commandBuffer, pipeline, pipelineLayout, descriptorSet, WIDTH, HEIGHT);

	submitCommandBuffer(device, commandBuffer);

	readBufferData(device, bufferMemoryResult, VECTOR_SIZE);

	std::vector<VkBuffer> buffers = { bufferA, bufferB, bufferResult };
	std::vector<VkDeviceMemory> bufferMemories = { bufferMemoryA, bufferMemoryB, bufferMemoryResult };
	cleanup(device, instance, pipeline, pipelineLayout, computeShaderModule, descriptorSetLayout, descriptorPool, commandPool, buffers, bufferMemories);
	return 0;
}
