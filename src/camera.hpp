#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vdem {
  class VdemCamera {
  public:

    void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
    void setPerspectiveProjection(float fovy, float aspect, float near, float far);

    const glm::mat4& getProjection() const { return projectionMatrix; }


  private:
    glm::mat4 projectionMatrix{1.f};
  };
}