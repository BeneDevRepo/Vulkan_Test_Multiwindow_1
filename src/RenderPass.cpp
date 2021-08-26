#include "RenderPass.h"

#include <stdexcept>

RenderPass::RenderPass(VkDevice& device, VkFormat& swapChainImageFormat): device(device) {
	create(swapChainImageFormat);
}

RenderPass::~RenderPass() {
	cleanup();
}

void RenderPass::create(VkFormat& swapChainImageFormat) {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // 1 sample, we won't be doing MSAA
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // we Clear when this attachment is loaded
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // we keep the attachment stored when the renderpass ends
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // we don't care about stencil
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // =
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // we don't know or care about the starting layout of the attachment
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //after the renderpass ends, the image has to be on a layout ready for display

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; // attachment number will index into the pAttachments array in the parent renderpass itself
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1; // create 1 subpass, which is the minimum you can do
	subpass.pColorAttachments = &colorAttachmentRef;

	// VkSubpassDependency dependency{}; //-------- Maybe unneccessary
	// dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	// dependency.dstSubpass = 0;
	// dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	// dependency.srcAccessMask = 0;
	// dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	// dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	// renderPassInfo.dependencyCount = 1; //-------- Maybe unneccessary
	// renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");
}

void RenderPass::cleanup() {
	vkDestroyRenderPass(device, renderPass, nullptr);
}

void RenderPass::begin(VkCommandBuffer& commandBuffer, VkFramebuffer& frameBuffer, VkExtent2D frameSize, VkClearColorValue color) {
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = frameSize;

	VkClearValue clearColor = {{color}};
	// VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end(VkCommandBuffer& commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}