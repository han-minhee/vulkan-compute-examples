#ifndef VK_INSTANCE_HPP
#define VK_INSTANCE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

VkInstance createInstance();
void checkInstanceExtensions(const std::vector<const char*>& requiredExtensions);

#endif // VK_INSTANCE_HPP
