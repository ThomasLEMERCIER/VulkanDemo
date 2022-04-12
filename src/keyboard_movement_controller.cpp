#include "keyboard_movement_controller.hpp"

#include <limits>

namespace vdem {
  void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, VdemGameObject& game_object) {
    
    glm::vec3 rotate{0};
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) { rotate.y += 1.f; }
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) { rotate.y -= 1.f; }
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) { rotate.x += 1.f; }
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) { rotate.x -= 1.f; }

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
          game_object.transform.rotation += glm::normalize(rotate) * lookSpeed * dt;
    }

    game_object.transform.rotation.x = glm::clamp(game_object.transform.rotation.x, -1.5f, 1.5f);
    game_object.transform.rotation.y = glm::mod(game_object.transform.rotation.y, glm::two_pi<float>());

    float yaw = game_object.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 move{0};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) { move += forwardDir; }
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) { move -= forwardDir; }
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) { move -= rightDir; }
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) { move += rightDir; }
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) { move += upDir; }
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) { move -= upDir; }

    if (glm::dot(move, move) > std::numeric_limits<float>::epsilon()) {
      game_object.transform.translation += glm::normalize(move) * moveSpeed * dt;
    } 
  }
}