#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <fstream>

const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 1;
const uint32_t VECTOR_SIZE = WIDTH * HEIGHT;

std::vector<char> readFile(const std::string &filename)
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

std::vector<const char *> getInstanceExtensions()
{
    std::vector<const char *> extensions;
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

std::vector<const char *> getDeviceExtensions()
{
    std::vector<const char *> extensions;
    extensions.push_back("VK_KHR_portability_subset"); // Required for MoltenVK
    return extensions;
}

void checkInstanceExtensions(const std::vector<const char *> &requiredExtensions)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    std::cout << "Available instance extensions:" << std::endl;
    for (const auto &ext : availableExtensions)
    {
        std::cout << "\t" << ext.extensionName << std::endl;
    }

    for (const auto &reqExt : requiredExtensions)
    {
        bool found = false;
        for (const auto &ext : availableExtensions)
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

void checkDeviceExtensions(VkPhysicalDevice device, const std::vector<const char *> &requiredExtensions)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::cout << "Available device extensions:" << std::endl;
    for (const auto &ext : availableExtensions)
    {
        std::cout << "\t" << ext.extensionName << std::endl;
    }

    for (const auto &reqExt : requiredExtensions)
    {
        bool found = false;
        for (const auto &ext : availableExtensions)
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

int main()
{
    // Initialize Vulkan
    VkInstance instance;

    // Application info (but they're optional actually)
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Instance create info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get extensions for the instance
    std::vector<const char *> extensions = getInstanceExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    checkInstanceExtensions(extensions);

    // Validation layers for debugging
    std::vector<const char *> validationLayers;
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    // Set the flag for portability enumeration
    // This is needed for MoltenVK, but not confirmed on other platforms
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VkResult errorCode = vkCreateInstance(&createInfo, nullptr, &instance);
    if (errorCode != VK_SUCCESS)
    {
        std::cout << "Error code: " << errorCode << std::endl;
        throw std::runtime_error("failed to create instance!");
    }
    std::cout << "Vulkan instance created successfully!" << std::endl;
    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Select the first physical device
    VkPhysicalDevice physicalDevice = devices[0];

    // Get queue family properties
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Find a queue family that supports VK_QUEUE_GRAPHICS_BIT
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

    // Specify the queue creation info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Device create info
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    // Get required device extensions
    std::vector<const char *> deviceExtensions = getDeviceExtensions();
    checkDeviceExtensions(physicalDevice, deviceExtensions);

    // Enable device extensions
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Create the logical device
    VkDevice device;
    VkResult logicalDeviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (logicalDeviceResult != VK_SUCCESS)
    {
        std::cout << "Error code: " << logicalDeviceResult << std::endl;
        throw std::runtime_error("failed to create logical device!");
    }

    // Get the graphics queue
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);

    std::cout << "Logical device created successfully!" << std::endl;

    // Create a buffer for input/output
    VkBuffer bufferA, bufferB, bufferResult;
    VkDeviceMemory bufferMemoryA, bufferMemoryB, bufferMemoryResult;
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = VECTOR_SIZE * sizeof(float);
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    vkCreateBuffer(device, &bufferInfo, nullptr, &bufferA);
    vkCreateBuffer(device, &bufferInfo, nullptr, &bufferB);
    vkCreateBuffer(device, &bufferInfo, nullptr, &bufferResult);

    std::cout << "Buffer created successfully!" << std::endl;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, bufferA, &memRequirements);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemoryA);
    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemoryB);
    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemoryResult);

    vkBindBufferMemory(device, bufferA, bufferMemoryA, 0);
    vkBindBufferMemory(device, bufferB, bufferMemoryB, 0);
    vkBindBufferMemory(device, bufferResult, bufferMemoryResult, 0);

    std::cout << "Memory allocated and bound to buffer successfully!" << std::endl;

    // Map memory and initialize input data
    void *data;
    vkMapMemory(device, bufferMemoryA, 0, bufferInfo.size, 0, &data);
    float *mappedA = (float *)data;
    for (int i = 0; i < VECTOR_SIZE; i++)
        mappedA[i] = i;
    vkUnmapMemory(device, bufferMemoryA);

    vkMapMemory(device, bufferMemoryB, 0, bufferInfo.size, 0, &data);
    float *mappedB = (float *)data;
    for (int i = 0; i < VECTOR_SIZE; i++)
        mappedB[i] = 2 * i;
    vkUnmapMemory(device, bufferMemoryB);

    std::cout << "Input data initialized successfully!" << std::endl;

    // Load compute shader
    auto shaderCode = readFile("vector_add.comp.spv");
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

    VkShaderModule computeShaderModule;
    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &computeShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    // Create descriptor set layout
    VkDescriptorSetLayoutBinding layoutBindings[3] = {};
    for (int i = 0; i < 3; i++)
    {
        layoutBindings[i].binding = i;
        layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBindings[i].descriptorCount = 1;
        layoutBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 3;
    descriptorSetLayoutCreateInfo.pBindings = layoutBindings;

    VkDescriptorSetLayout descriptorSetLayout;
    vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

    // Create compute pipeline
    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineCreateInfo.stage.module = computeShaderModule;
    pipelineCreateInfo.stage.pName = "main";
    pipelineCreateInfo.layout = pipelineLayout;

    VkPipeline pipeline;
    VkResult pipelineResult = vkCreateComputePipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
    if (pipelineResult != VK_SUCCESS)
    {
        std::cout << "Error code: " << pipelineResult << std::endl;
        throw std::runtime_error("failed to create compute pipeline!");
    }

    std::cout << "Compute pipeline created successfully!" << std::endl;

    // Create descriptor pool
    VkDescriptorPoolSize poolSizes[1] = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 3;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = poolSizes;
    descriptorPoolCreateInfo.maxSets = 1;

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

    // Allocate and write descriptor sets
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);

    VkDescriptorBufferInfo bufferInfos[3] = {};
    bufferInfos[0].buffer = bufferA;
    bufferInfos[0].offset = 0;
    bufferInfos[0].range = bufferInfo.size;
    bufferInfos[1].buffer = bufferB;
    bufferInfos[1].offset = 0;
    bufferInfos[1].range = bufferInfo.size;
    bufferInfos[2].buffer = bufferResult;
    bufferInfos[2].offset = 0;
    bufferInfos[2].range = bufferInfo.size;

    VkWriteDescriptorSet descriptorWrites[3] = {};
    for (int i = 0; i < 3; i++)
    {
        descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet = descriptorSet;
        descriptorWrites[i].dstBinding = i;
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pBufferInfo = &bufferInfos[i];
    }

    vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);

    std::cout << "Descriptor set created and updated successfully!" << std::endl;

    // Create command pool and buffer
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

    // Record command buffer
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDispatch(commandBuffer, WIDTH, HEIGHT, 1);
    vkEndCommandBuffer(commandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);
    VkResult submitResult = vkQueueSubmit(queue, 1, &submitInfo, nullptr);
    if ( submitResult != VK_SUCCESS)
    {
        std::cout << "Error code: " << submitResult << std::endl;
        throw std::runtime_error("failed to submit queue!");
    }
    vkQueueWaitIdle(queue);
    
    // Read back the results
    vkMapMemory(device, bufferMemoryResult, 0, bufferInfo.size, 0, &data);
    float *mappedResult = (float *)data;
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        std::cout << mappedResult[i] << " ";
    }
    std::cout << std::endl;
    vkUnmapMemory(device, bufferMemoryResult);

    // Cleanup
    vkDestroyBuffer(device, bufferA, nullptr);
    vkDestroyBuffer(device, bufferB, nullptr);
    vkDestroyBuffer(device, bufferResult, nullptr);
    vkFreeMemory(device, bufferMemoryA, nullptr);
    vkFreeMemory(device, bufferMemoryB, nullptr);
    vkFreeMemory(device, bufferMemoryResult, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyShaderModule(device, computeShaderModule, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    std::cout << "Vulkan resources cleaned up successfully!" << std::endl;

    return 0;
}
