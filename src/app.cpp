#include "app.hpp"
#include "sierpinski.hpp"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace vdem {
  struct SimplePushConstantsData {
    glm::mat2 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
  };
  
  App::App() {
    loadGameObjects();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
  }

  App::~App() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);  
  }

  void App::loadGameObjects() {
    std::vector<VdemModel::Vertex> vertices = {{{-0.5f, 0.5f}, {1.f, 0.f, 0.f}}, {{0.f, -0.5f}, {1.f, 0.f, 0.f}}, {{0.5f, 0.5f}, {1.f, 0.f, 0.f}}};
    //populate_vertices(vertices, 1, -0.75, 0.75, 1.5);
    auto model = std::make_shared<VdemModel>(device, vertices);

    auto triangle = VdemGameObject::createGameObject();
    triangle.model = model;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale = {2.f, .5f};
    triangle.transform2d.rotation = glm::two_pi<float>() * 0.25f;
    
    gameObjects.push_back(std::move(triangle));
  }

  void App::run() {
    while (!window.shouldClose()) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(device.device());
  }

  void App::createPipelineLayout() {
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantsData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void App::createPipeline() {
    assert(swapChain != nullptr && "Swap chain must be created before pipeline");
    assert(pipelineLayout != nullptr && "Pipeline layout must be created before pipeline");
    PipelineConfigInfo pipelineConfig{};
    VdemPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<VdemPipeline>(device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
  }

  void App::recreateSwapChain() {
    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = window.getExtent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.device());


    if (swapChain == nullptr) {
      swapChain = std::make_unique<VdemSwapChain>(device, extent);
    } else {
      swapChain = std::make_unique<VdemSwapChain>(device, extent, std::move(swapChain));
      if (swapChain->imageCount() != commandBuffers.size()) {
        freeCommandBuffers();
        createCommandBuffers();
      }
    }

    createPipeline();
  }

  void App::createCommandBuffers() {

    commandBuffers.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void App::freeCommandBuffers() {
    vkFreeCommandBuffers(
      device.device(),
      device.getCommandPool(), 
      (uint32_t)commandBuffers.size(), 
      commandBuffers.data());
    commandBuffers.clear();
  }

  void App::recordCommandBuffer(uint32_t index) {

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[index], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(index);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain->getSwapChainExtent().width;
    viewport.height = (float)swapChain->getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {{0, 0}, swapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[index], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[index], 0, 1, &scissor);
    
    renderGameObjects(commandBuffers[index]);

    vkCmdEndRenderPass(commandBuffers[index]);
    if (vkEndCommandBuffer(commandBuffers[index]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void App::renderGameObjects(VkCommandBuffer commandBuffer) {
    pipeline->bind(commandBuffer);

    for (auto& obj: gameObjects) {
      obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());;

      SimplePushConstantsData push{};
      push.offset = obj.transform2d.translation;
      push.color = obj.color;
      push.transform = obj.transform2d.mat2();
      

      vkCmdPushConstants(
        commandBuffer, 
        pipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
        0, 
        sizeof(SimplePushConstantsData), 
        &push);
      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
    }
  }

  void App::drawFrame() {
    uint32_t imageIndex;
    VkResult result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }
    
    recordCommandBuffer(imageIndex);
    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
      window.resetWindowResized();
      recreateSwapChain();
      return;
    }
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to submit command buffer!");
    }
  }
}