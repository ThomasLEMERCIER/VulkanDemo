#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "camera.hpp"

#include <memory>

namespace vdem {
  class SimpleRenderSystem {

  public:

    SimpleRenderSystem(VdemDevice &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(
      VkCommandBuffer commandBuffer, 
      std::vector<VdemGameObject> &gameObjects,
      const VdemCamera &camera);
  
  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    VdemDevice& device;

    std::unique_ptr<VdemPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
}