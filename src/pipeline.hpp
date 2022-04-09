#pragma once

#include <string>
#include <vector>

#include "device.hpp"

namespace vdem {

  struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };

  class VdemPipeline {
  public:
    VdemPipeline(
      VdemDevice& device,
      const std::string& vertexShaderPath, 
      const std::string& fragmentShaderPath, 
      const PipelineConfigInfo& configInfo);
    ~VdemPipeline();

    VdemPipeline(const VdemPipeline&) = delete;
    VdemPipeline& operator=(const VdemPipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

  private:
    static std::vector<char> readFile(const std::string& filePath);

    void createGraphicsPipeline(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const PipelineConfigInfo& configInfo);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    VdemDevice& vdemDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

  };
}
