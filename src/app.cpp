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

  // temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
  std::unique_ptr<VdemModel> createCubeModel(VdemDevice& device, glm::vec3 offset) {
    VdemModel::Builder modelBuilder{};
    modelBuilder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices) {
      v.position += offset;
    }

    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<VdemModel>(device, modelBuilder);
  }

  void App::loadGameObjects() {
    std::shared_ptr<VdemModel> model = createCubeModel(device, {0.f, 0.f, 0.f});

    auto cube = VdemGameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {0.f, 0.f, 2.5f};
    cube.transform.scale = {.5f, .5f, .5f};

    gameObjects.push_back(std::move(cube));
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
      camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 10.f);
      
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