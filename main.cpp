// #define VK_USE_PLATFORM_WIN32_KHR
// #define VK_PROTOTYPES
// #include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

#include <iostream>

#include <chrono>

#include "Instance.h"
#include "PhysicalDevice.h"

#include "LogicalDevice.h"

#include "Window.h"

#include "Swapchain.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

#include "Model.h"

#ifdef NDEBUG // No Debug
	const std::vector<const char*> validationLayers = {};
#else
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

int main() {
	Instance instance(validationLayers);

	Window window(instance.instance, 100, 100, WIDTH, HEIGHT);
	Window window2(instance.instance, 500, 500, WIDTH, HEIGHT);

	PhysicalDevice physicalDevice(instance.instance);

	LogicalDevice logicalDevice(physicalDevice.physicalDevice, validationLayers, deviceExtensions);

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(physicalDevice.physicalDevice);
	vkGetDeviceQueue(logicalDevice.device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice.device, indices.presentFamily.value(), 0, &presentQueue);

	uint32_t numSwapchainImages = -1;
	{
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice.physicalDevice, window.surface, &capabilities);
		numSwapchainImages = std::max(capabilities.minImageCount + 1, MAX_FRAMES_IN_FLIGHT);
		if (capabilities.maxImageCount > 0 && numSwapchainImages > capabilities.maxImageCount)
			numSwapchainImages = capabilities.maxImageCount;
	}

	CommandPool commandPool(physicalDevice.physicalDevice, logicalDevice.device);

	std::vector<CommandBuffer> commandBuffers = commandPool.allocateCommandBuffers(numSwapchainImages);
	Swapchain swapchain(physicalDevice.physicalDevice, logicalDevice.device, window.surface, {WIDTH, HEIGHT}, numSwapchainImages);
	RenderPass renderPass(logicalDevice.device, swapchain.swapChainImageFormat);
	Pipeline pipeline(logicalDevice.device, renderPass.renderPass, swapchain.swapChainExtent, swapchain.swapChainImageViews);

	std::vector<CommandBuffer> commandBuffers2 = commandPool.allocateCommandBuffers(numSwapchainImages);
	Swapchain swapchain2(physicalDevice.physicalDevice, logicalDevice.device, window2.surface, {WIDTH, HEIGHT}, numSwapchainImages);
	// RenderPass renderPass2(logicalDevice.device, swapchain2.swapChainImageFormat);
	Pipeline pipeline2(logicalDevice.device, renderPass.renderPass, swapchain2.swapChainExtent, swapchain2.swapChainImageViews);

	Model model(logicalDevice.device, physicalDevice.physicalDevice, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},

		{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
		});

	auto recreateSwapchain = [](PhysicalDevice& physicalDevice, Window& window, LogicalDevice& logicalDevice, Pipeline& pipeline, RenderPass& renderPass, Swapchain& swapchain) {
		while(window.isMinimized || window.noArea())
			window.pollMsg();
		VkExtent2D windowSize = window.getSize();

		vkDeviceWaitIdle(logicalDevice.device);
		// std::cout << "Recreating <width: " << windowSize.width << " height: " << windowSize.height << ">\n";

		pipeline.cleanup();
		// renderPass.cleanup();
		swapchain.cleanup();

		swapchain.create(physicalDevice.physicalDevice, window.surface, windowSize, MAX_FRAMES_IN_FLIGHT);
		// renderPass.create(swapchain.swapChainImageFormat);
		pipeline.create(renderPass.renderPass, swapchain.swapChainExtent, swapchain.swapChainImageViews);
	};

	auto tryDrawFrame = [&](Window& window, LogicalDevice& logicalDevice, std::vector<CommandBuffer>& commandBuffers,
			Swapchain& swapchain, RenderPass& renderPass, Pipeline& pipeline,
			size_t currentFrame)->bool {
		bool recreationRequired = false;

		vkWaitForFences(logicalDevice.device, 1, &pipeline.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		
		// vkDeviceWaitIdle(logicalDevice.device);// commandBuffers push constants --------------------- --------------------- Test

		uint32_t imageIndex;
		VkResult acquireResult = vkAcquireNextImageKHR(logicalDevice.device, swapchain.swapChain, UINT64_MAX, pipeline.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
			recreationRequired = true;
		else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image!");

		// commandBuffers push constants --------------------- --------------------- Test
		// for (size_t i = 0; i < swapchain.swapChainImages.size(); i++) {
			// VkCommandBuffer& targetCommandBuffer = commandBuffers.commandBuffers[i];
		// for (int i = 1; i != -1; i = -1) {
		CommandBuffer& targetCommandBuffer = commandBuffers[imageIndex];
		targetCommandBuffer.beginRecording();

			renderPass.begin(targetCommandBuffer.commandBuffer, pipeline.swapChainFramebuffers[imageIndex], swapchain.swapChainExtent, {0., 0., 0., 0});

				pipeline.bind(targetCommandBuffer.commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS);

				model.bind(targetCommandBuffer.commandBuffer);

				struct alignas(16) MeshPushConstants {
					struct {
						float x, y;
					} scale;
					struct {
						float x, y;
					} off;
				} constants{};

				VkExtent2D winSize = window.getSize();
				// float aspectRatio = winSize.height * 1. / winSize.width;
				// constants.scale.x = aspectRatio;
				// constants.scale.y = 1.;
				constants.scale.x = 400. / winSize.width;
				constants.scale.y = 400. / winSize.height;

				constants.off.x = -.5;
				constants.off.y = -.5;
				vkCmdPushConstants(targetCommandBuffer.commandBuffer, pipeline.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstants), &constants);
				vkCmdDraw(targetCommandBuffer.commandBuffer, (uint32_t)model.vertices.size(), 1, 0, 0);

				constants.off.x = 0.;
				constants.off.y = 0.;
				vkCmdPushConstants(targetCommandBuffer.commandBuffer, pipeline.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstants), &constants);
				vkCmdDraw(targetCommandBuffer.commandBuffer, (uint32_t)model.vertices.size(), 1, 0, 0);

				POINT mousePos = window.getMouseRel();
				constants.off.x = (mousePos.x * 2. / winSize.width - 1.) / constants.scale.x;
				constants.off.y = (mousePos.y * 2. / winSize.height - 1.) / constants.scale.y;
				vkCmdPushConstants(targetCommandBuffer.commandBuffer, pipeline.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstants), &constants);
				vkCmdDraw(targetCommandBuffer.commandBuffer, (uint32_t)model.vertices.size(), 1, 0, 0);

			renderPass.end(targetCommandBuffer.commandBuffer);

		targetCommandBuffer.endRecording();
		// }

		if (pipeline.imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(logicalDevice.device, 1, &pipeline.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		pipeline.imagesInFlight[imageIndex] = pipeline.inFlightFences[currentFrame];

		VkSubmitInfo submitInfo{ };
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { pipeline.imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex].commandBuffer;

		VkSemaphore signalSemaphores[] = { pipeline.renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(logicalDevice.device, 1, &pipeline.inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, pipeline.inFlightFences[currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchain.swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
		if(presentResult == VK_ERROR_OUT_OF_DATE_KHR)
			recreationRequired = true;
		else if (presentResult != VK_SUCCESS && presentResult != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to present image!");

		return recreationRequired;
	};

	LARGE_INTEGER performanceFrequency;
	QueryPerformanceFrequency(&performanceFrequency); // counts per second

	constexpr uint64_t TICKS_PER_SECOND = 1000 * 1000 * 10;
	constexpr uint64_t desiredSteps = TICKS_PER_SECOND / 240;
	const auto unixTime = [&performanceFrequency]()->uint64_t{LARGE_INTEGER performanceCounter; QueryPerformanceCounter(&performanceCounter); return performanceCounter.QuadPart / (performanceFrequency.QuadPart / TICKS_PER_SECOND); };
	
	uint64_t lastUnix = 0;

	size_t currentFrame = 0;
	bool recreationRequired = false;
	bool recreationRequired2 = false;
	while (true) {
		uint64_t stepsSinceLastFrame = unixTime() - lastUnix;
		// while(stepsSinceLastFrame < desiredSteps) {
		// 	stepsSinceLastFrame = unixTime() - lastUnix;
		// 	// if(stepsSinceLastFrame+(TICKS_PER_SECOND / 1000) * 2 < desiredSteps)
		// 	// Sleep(1);
		// }
		lastUnix += stepsSinceLastFrame;

		SetWindowTextA(window.wnd, std::to_string(TICKS_PER_SECOND * 1. / stepsSinceLastFrame).data());
		SetWindowTextA(window2.wnd, std::to_string(TICKS_PER_SECOND * 1. / stepsSinceLastFrame).data());

		window.pollMsg();
		window2.pollMsg();

		if(window.shouldClose)
			break;
		if(window2.shouldClose)
			break;

		if(window.isResized || recreationRequired) {
			recreateSwapchain(physicalDevice, window, logicalDevice, pipeline, renderPass, swapchain);
			window.isResized = false;
		}
		if(window2.isResized || recreationRequired2) {
			recreateSwapchain(physicalDevice, window2, logicalDevice, pipeline2, renderPass, swapchain2);
			window2.isResized = false;
		}

		recreationRequired = tryDrawFrame(window, logicalDevice, commandBuffers, swapchain, renderPass, pipeline, currentFrame);
		recreationRequired2 = tryDrawFrame(window2, logicalDevice, commandBuffers2, swapchain2, renderPass, pipeline2, currentFrame);
		// recreationRequired = tryDrawFrame(currentFrame);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	vkDeviceWaitIdle(logicalDevice.device);

	std::cout << "Terminated without crashing!\n";

    return 0;
}