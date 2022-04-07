#include "pipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace vdem {
    VdemPipeline::VdemPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        createGraphicsPipeline(vertexShaderPath, fragmentShaderPath);
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

    void VdemPipeline::createGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        std::vector<char> vertexShaderCode = readFile(vertexShaderPath);
        std::vector<char> fragmentShaderCode = readFile(fragmentShaderPath);

        std::cout << "Vertex Shader Code Size : " << vertexShaderCode.size() << '\n';
        std::cout << "Fragment Shader Code Size : " << fragmentShaderCode.size() << '\n';
    }
}
