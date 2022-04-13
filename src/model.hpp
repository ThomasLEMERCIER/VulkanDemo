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
      glm::vec3 position;
      glm::vec3 color;

      static std::vector<VkVertexInputBindingDescription> getBindingDescription();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
      };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
    };

    VdemModel(VdemDevice &device, const VdemModel::Builder &builder);
    ~VdemModel();

    VdemModel(const VdemModel&) = delete;
    VdemModel& operator=(const VdemModel&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    VdemDevice &device;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;

    bool hasIndexBuffer{false};
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
  };
}
