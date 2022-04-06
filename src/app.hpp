#pragma once

#include "window.hpp"

namespace vdem {

    class App {

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
            VdemWindow window{WIDTH, HEIGHT, "Vulkan Demo"};
    };

}