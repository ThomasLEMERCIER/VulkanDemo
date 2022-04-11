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
    VkExtent2D getExtent() { return {(uint32_t)width, (uint32_t)height}; }
    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResized() { framebufferResized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void init_window();

    int width;
    int height;
    bool framebufferResized = false;

    std::string windowName;
    GLFWwindow *window;
  };
}