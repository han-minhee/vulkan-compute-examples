#include "vk_device.hpp"
#include "vk_utils.hpp"
#include <iostream>
#include <stdexcept>

VkDevice createLogicalDevice(VkInstance instance, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// Select the first physical device
	physicalDevice = devices[0];

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int graphicsFamily = -1;
	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
			break;
		}
	}
	if (graphicsFamily == -1)
	{
		throw std::runtime_error("failed to find a suitable queue family!");
	}

	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

	std::vector<const char*> deviceExtensions = getDeviceExtensions();
	checkDeviceExtensions(physicalDevice, deviceExtensions);

	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VkDevice device;
	VkResult logicalDeviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	if (logicalDeviceResult != VK_SUCCESS)
	{
		std::cout << "Error code: " << logicalDeviceResult << std::endl;
		throw std::runtime_error("failed to create logical device!");
	}
	vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
	std::cout << "Logical device created successfully!" << std::endl;
	return device;
}
