#include "pipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace vdem {
  VdemPipeline::VdemPipeline(
    VdemDevice& device,
    const std::string& vertexShaderPath, 
    const std::string& fragmentShaderPath, 
    const PipelineConfigInfo& configInfo)
    : vdemDevice(device) {
    createGraphicsPipeline(vertexShaderPath, fragmentShaderPath, configInfo);
  }

  std::vector<char> VdemPipeline::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("failed to open file: " + filePath);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    return buffer;
  }

  void VdemPipeline::createGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigInfo& configInfo) {
    std::vector<char> vertexShaderCode = readFile(vertexShaderPath);
    std::vector<char> fragmentShaderCode = readFile(fragmentShaderPath);

    std::cout << "Vertex Shader Code Size : " << vertexShaderCode.size() << '\n';
    std::cout << "Fragment Shader Code Size : " << fragmentShaderCode.size() << '\n';
  }

  void VdemPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(vdemDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }
  }

  PipelineConfigInfo VdemPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo;
    return configInfo;
  }
}
