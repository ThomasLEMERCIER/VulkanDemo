#include "renderer.hpp"

#include <cassert>
#include <stdexcept>
#include <array>

namespace vdem {
  
  VdemRenderer::VdemRenderer(VdemWindow &window, VdemDevice &device) : window{window}, device{device} {
    recreateSwapChain();
    createCommandBuffers();
  }

  VdemRenderer::~VdemRenderer() {
    freeCommandBuffers();
  }

  void VdemRenderer::recreateSwapChain() {
    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = window.getExtent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.device());

    if (swapChain == nullptr) {
      swapChain = std::make_unique<VdemSwapChain>(device, extent);
    } else {
      std::shared_ptr<VdemSwapChain> oldSwapChain = std::move(swapChain);
      swapChain = std::make_unique<VdemSwapChain>(device, extent, oldSwapChain);

      if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
        throw std::runtime_error("Swap chain formats are not compatible");
      }
    }
  }

  void VdemRenderer::createCommandBuffers() {
    commandBuffers.resize(VdemSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void VdemRenderer::freeCommandBuffers() {
    vkFreeCommandBuffers(
      device.device(),
      device.getCommandPool(), 
      (uint32_t)commandBuffers.size(), 
      commandBuffers.data());
    commandBuffers.clear();
  }

  VkCommandBuffer VdemRenderer::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while a frame is already started!");
    VkResult result = swapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return nullptr;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
  }

  void VdemRenderer::endFrame() {
    assert(isFrameStarted && "Can't call endFrame without a frame being started!");
    auto commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }

    auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
      window.resetWindowResized();
      recreateSwapChain();
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to submit command buffer!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % VdemSwapChain::MAX_FRAMES_IN_FLIGHT;
  }
  void VdemRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass without a frame being started!");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't start render pass with a command buffer that is not the current command buffer!");

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain->getSwapChainExtent().width;
    viewport.height = (float)swapChain->getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {{0, 0}, swapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }
  void VdemRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call endSwapChainRenderPass without a frame being started!");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass with a command buffer that is not the current command buffer!");

    vkCmdEndRenderPass(commandBuffer);
  }
}