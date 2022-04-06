#include "app.hpp"

namespace vdem {

    void App::run() {
        while (!window.shouldClose()) {
            glfwPollEvents();
        }
    }

}