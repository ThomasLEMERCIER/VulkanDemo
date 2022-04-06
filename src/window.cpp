#include "window.hpp"

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

}