#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include <memory>

namespace vdem {
  class App {

  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();

  private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    VdemWindow window{WIDTH, HEIGHT, "Vulkan Demo"};
    VdemDevice device{window};
    VdemSwapChain swapChain{device, window.getExtent()};
    std::unique_ptr<VdemPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
  };
}