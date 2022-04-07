#pragma once

#include <string>
#include <vector>

namespace vdem {
    class VdemPipeline {

    public:
        VdemPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    private:
    static std::vector<char> readFile(const std::string& filePath);

    void createGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    };
}
