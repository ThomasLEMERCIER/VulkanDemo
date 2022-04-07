#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

namespace vdem {
  class App {

  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

  private:
    VdemWindow window{WIDTH, HEIGHT, "Vulkan Demo"};
    VdemDevice device{window};
    VdemPipeline pipeline{
      device,
      "../shaders/simple_shader.vert.spv",
      "../shaders/simple_shader.frag.spv",
      VdemPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

  };
}