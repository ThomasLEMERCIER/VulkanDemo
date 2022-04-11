#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

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
    void loadModel();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    void recreateSwapChain();
    void recordCommandBuffer(uint32_t index);

    VdemWindow window{WIDTH, HEIGHT, "Vulkan Demo"};
    VdemDevice device{window};
    std::unique_ptr<VdemSwapChain> swapChain;
    std::unique_ptr<VdemPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<VdemModel> model;
  };
}