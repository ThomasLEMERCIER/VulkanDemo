#pragma once

#include "model.hpp"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace vdem {
  struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

  // Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
  // Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
  // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4();
    glm::mat3 normalMatrix();
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
    TransformComponent transform;

  private:
    VdemGameObject(id_t objId) : id(objId) {};

    id_t id;
  };
}