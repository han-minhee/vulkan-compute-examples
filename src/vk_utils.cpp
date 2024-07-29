#include "vk_utils.hpp"
#include <iostream>
#include <fstream>

std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

std::vector<const char*> getInstanceExtensions()
{
	std::vector<const char*> extensions;
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	return extensions;
}

std::vector<const char*> getDeviceExtensions()
{
	std::vector<const char*> extensions;

#ifdef __APPLE__
	extensions.push_back("VK_KHR_portability_subset"); // Required for MoltenVK
#endif

	return extensions;
}

void checkInstanceExtensions(const std::vector<const char*>& requiredExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	std::cout << "Available instance extensions:" << std::endl;
	for (const auto& ext : availableExtensions)
	{
		std::cout << "\t" << ext.extensionName << std::endl;
	}

	for (const auto& reqExt : requiredExtensions)
	{
		bool found = false;
		for (const auto& ext : availableExtensions)
		{
			if (std::strcmp(reqExt, ext.extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::cout << "Required instance extension not found: " << reqExt << std::endl;
		}
	}
}

void checkDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::cout << "Available device extensions:" << std::endl;
	for (const auto& ext : availableExtensions)
	{
		std::cout << "\t" << ext.extensionName << std::endl;
	}

	for (const auto& reqExt : requiredExtensions)
	{
		bool found = false;
		for (const auto& ext : availableExtensions)
		{
			if (std::strcmp(reqExt, ext.extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::cout << "Required device extension not found: " << reqExt << std::endl;
		}
	}
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}


void cleanup(VkDevice device, VkInstance instance, VkPipeline pipeline, VkPipelineLayout pipelineLayout,
	VkShaderModule computeShaderModule, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
	VkCommandPool commandPool, std::vector<VkBuffer> buffers, std::vector<VkDeviceMemory> bufferMemories) {

	for (VkBuffer buffer : buffers) {
		if (buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, buffer, nullptr);
		}
	}

	for (VkDeviceMemory bufferMemory : bufferMemories) {
		if (bufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(device, bufferMemory, nullptr);
		}
	}

	if (pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(device, pipeline, nullptr);
	}

	if (pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}

	if (computeShaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(device, computeShaderModule, nullptr);
	}

	if (descriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	}

	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}

	if (commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device, commandPool, nullptr);
	}

	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);

	std::cout << "Vulkan resources cleaned up successfully!" << std::endl;
}