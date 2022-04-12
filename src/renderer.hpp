#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

#include <cassert>
#include <memory>

namespace vdem {
  class VdemRenderer {

  public:
    VdemRenderer(VdemWindow &window, VdemDevice &device);
    ~VdemRenderer();

    VdemRenderer(const VdemRenderer&) = delete;
    VdemRenderer& operator=(const VdemRenderer&) = delete;

    bool isFrameInProgress() const { return isFrameStarted; }
    float getAspectRatio() const { return swapChain->extentAspectRatio(); }
    VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
    
    VkCommandBuffer getCurrentCommandBuffer() const { 
      assert(isFrameStarted && "Cannot get command buffer before frame is started");
      return commandBuffers[currentFrameIndex]; 
    }

    int getFrameIndex() const { 
      assert(isFrameStarted && "Cannot get frame index before frame is started");
      return currentImageIndex; 
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    VdemWindow& window;
    VdemDevice& device;
    std::unique_ptr<VdemSwapChain> swapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
  };
}