#include "app.hpp"

#include "simple_render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

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
    std::shared_ptr<VdemModel> smooth_model = VdemModel::createModelFromFile(device, "models/flat_vase.obj");
    std::shared_ptr<VdemModel> flat_model = VdemModel::createModelFromFile(device, "models/smooth_vase.obj");

    auto smooth_vase = VdemGameObject::createGameObject();
    smooth_vase.model = smooth_model;
    smooth_vase.transform.translation = {0.f, 0.f, 2.5f};
    smooth_vase.transform.scale = {3.f, 3.f, 3.f};

    auto flat_vase = VdemGameObject::createGameObject();
    flat_vase.model = flat_model;
    flat_vase.transform.translation = {0.f, 0.f, -2.5f};
    flat_vase.transform.scale = {3.f, 3.f, 3.f};

    gameObjects.push_back(std::move(flat_vase));
    gameObjects.push_back(std::move(smooth_vase));
  }

  void App::run() {
    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
    VdemCamera camera{};
    //camera.setViewDirection(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});
    camera.setViewTarget(glm::vec3{-1.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 2.5f});

    auto viewerObject = VdemGameObject::createGameObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();
    
    while (!window.shouldClose()) {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      auto frameTime = std::chrono::duration_cast<std::chrono::duration<float>>(newTime - currentTime).count();
      currentTime = newTime;

      frameTime = glm::min(frameTime, MAX_FRAME_TIME);

      cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

      float aspectRatio = renderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 100.f);
      
      if (auto commandBuffer = renderer.beginFrame()) {
        renderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
      }
    }

    vkDeviceWaitIdle(device.device());
  }
}