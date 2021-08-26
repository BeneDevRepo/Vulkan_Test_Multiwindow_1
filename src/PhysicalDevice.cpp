#include "PhysicalDevice.h"

#include <stdexcept>
#include <vector>
#include <optional>
#include <set>
#include <string>
// #include <iostream>

namespace {
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

	bool isDeviceSuitable(VkPhysicalDevice physicalDevice) {
		if(!checkDeviceExtensionSupport(physicalDevice))
			return false;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		bool graphicsFamilyFresent = false;
		bool presentFamilyPresent = false;
		for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphicsFamilyFresent = true;

			if (vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, i))
				presentFamilyPresent = true;

			if (graphicsFamilyFresent && presentFamilyPresent)
				return true;

			i++;
		}

		return false;
    }
}

PhysicalDevice::PhysicalDevice(VkInstance& instance) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

	// for(VkPhysicalDevice device : physicalDevices) {
	// 	VkPhysicalDeviceProperties prop;
	// 	vkGetPhysicalDeviceProperties(device, &prop);
	// 	std::cout << prop.deviceName << "\n";
	// }

	for (const auto& device : physicalDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU!");
}

PhysicalDevice::~PhysicalDevice() {
}