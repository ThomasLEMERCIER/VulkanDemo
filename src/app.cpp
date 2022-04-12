#include "app.hpp"
#include "sierpinski.hpp"

#include "simple_render_system.hpp"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace vdem {
  
  App::App() {
    loadGameObjects();
  }

  App::~App() {}

  void App::loadGameObjects() {
    std::vector<VdemModel::Vertex> vertices = {{{-0.5f, 0.5f}, {1.f, 0.f, 0.f}}, {{0.f, -0.5f}, {1.f, 0.f, 0.f}}, {{0.5f, 0.5f}, {1.f, 0.f, 0.f}}};
    //populate_vertices(vertices, 1, -0.75, 0.75, 1.5);
    auto model = std::make_shared<VdemModel>(device, vertices);

    auto triangle = VdemGameObject::createGameObject();
    triangle.model = model;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale = {2.f, .5f};
    triangle.transform2d.rotation = glm::two_pi<float>() * 0.25f;
    
    gameObjects.push_back(std::move(triangle));
  }

  void App::run() {
    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
    
    while (!window.shouldClose()) {
      glfwPollEvents();
      
      if (auto commandBuffer = renderer.beginFrame()) {
        renderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
      }
    }

    vkDeviceWaitIdle(device.device());
  }
}