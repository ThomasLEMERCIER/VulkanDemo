#include "window.hpp"

#include <stdexcept>

namespace vdem {

  VdemWindow::VdemWindow(int w, int h, std::string name) : width(w), height(h), windowName(name) {
    init_window();
  }

  VdemWindow::~VdemWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void VdemWindow::init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  }

  void VdemWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  }

}