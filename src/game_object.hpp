#pragma once

#include "model.hpp"

#include <memory>

namespace vdem {
  struct Transform2dComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.f, 1.f};
    float rotation;

    glm::mat2 mat2() { 
      const float s = glm::sin(rotation);
      const float c = glm::cos(rotation);
      
      glm::mat2 rotMatrix = {{c, s}, {-s, c}};
      glm::mat2 scaleMat{{scale.x, 0.f}, {0.f, scale.y}};
      return rotMatrix * scaleMat; }
  };

  struct RigidBody2dComponent {
   glm::vec2 velocity;
   float mass{1.0f};
 };

  class VdemGameObject {
  public:
    using id_t = unsigned int;

    static VdemGameObject createGameObject() {
      static id_t currentId = 0;
      return VdemGameObject{currentId++};
    }

    VdemGameObject(const VdemGameObject &) = delete;
    VdemGameObject &operator=(const VdemGameObject &) = delete;
    VdemGameObject(VdemGameObject &&) = default;
    VdemGameObject &operator=(VdemGameObject &&) = default;

    id_t getId() { return id; }
    std::shared_ptr<VdemModel> model;
    glm::vec3 color{};
    Transform2dComponent transform2d;
    RigidBody2dComponent rigidBody2d;

  private:
    VdemGameObject(id_t objId) : id(objId) {};

    id_t id;
  };
}