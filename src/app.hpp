#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "window.hpp"
#include "renderer.hpp"

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
    void loadGameObjects();

    VdemWindow window{WIDTH, HEIGHT, "Vulkan Demo"};
    VdemDevice device{window};
    VdemRenderer renderer{window, device};

    std::vector<VdemGameObject> gameObjects;

    float MAX_FRAME_TIME = 1.f;
  };
}