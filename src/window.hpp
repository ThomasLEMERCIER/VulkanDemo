#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vdem {
  class VdemWindow {
  public:
    VdemWindow(int w, int h, std::string name);
    ~VdemWindow();

    VdemWindow(const VdemWindow&) = delete;
    VdemWindow& operator=(const VdemWindow&) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    void init_window();

    const int width;
    const int height;

    std::string windowName;
    GLFWwindow *window;
  };
}