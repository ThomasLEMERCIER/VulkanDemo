#include "app.hpp"
#include "sierpinski.hpp"

#include <stdexcept>
#include <array>

namespace vdem {
  App::App() {
    loadModel();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
  }

  App::~App() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);  
  }

  void App::loadModel() {
    std::vector<VdemModel::Vertex> vertices = {};
    populate_vertices(vertices, 1, -0.75, 0.75, 1.5);
    model = std::make_unique<VdemModel>(device, vertices);
  }

  void App::run() {
    while (!window.shouldClose()) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(device.device());
  }

  void App::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

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
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
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
    
    pipeline->bind(commandBuffers[index]);
    model->bind(commandBuffers[index]);
    model->draw(commandBuffers[index]);

    vkCmdEndRenderPass(commandBuffers[index]);
    if (vkEndCommandBuffer(commandBuffers[index]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
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