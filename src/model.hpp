#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vdem {
class VdemModel
  {
  public:

    struct Vertex {
      glm::vec2 position;

      static std::vector<VkVertexInputBindingDescription> getBindingDescription();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
      };

    VdemModel(VdemDevice &device, const std::vector<Vertex> &vertices);
    ~VdemModel();

    VdemModel(const VdemModel&) = delete;
    VdemModel& operator=(const VdemModel&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    VdemDevice &device;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
  };
}
