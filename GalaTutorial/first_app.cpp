#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>
#include <glm/gtc/constants.hpp>
//
namespace lve
{
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f}; // with light intensity
        
    };
    
    FirstApp::FirstApp()
    {
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        
        loadGameObjects();
    }

    FirstApp::~FirstApp()
    {}

    void FirstApp::run()
    {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i=0; i<uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }
        
        LveDescriptorSetLayout::Builder builder{lveDevice};
        
        builder.addBinding(
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_VERTEX_BIT);

        std::unique_ptr<LveDescriptorSetLayout> globalSetLayout = builder.build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets (LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        
        for(int i=0; i<globalDescriptorSets.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
            
            LveDescriptorWriter lveDescWriter{*globalSetLayout, *globalPool};
            
            lveDescWriter.writeBuffer(0, &bufferInfo);
            lveDescWriter.build(globalDescriptorSets[i]);
        }
        
        SimpleRenderSystem simpleRenderSystem(
          lveDevice,
          lveRenderer.getSwapChainRenderPass(),
          globalSetLayout->getVkDescriptorSetLayout());
        LveCamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        
        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5;
        KeyboardMovementController cameraController{};
        //
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while(!lveWindow.shouldClose())
        {
            glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            
            currentTime = newTime;
            
            float MAX_FRAME_TIME = 100.f;
            frameTime = glm::min(frameTime, MAX_FRAME_TIME);
            
            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            
            float aspect = lveRenderer.getAspectRatio();
            
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);
            
            // beginFrame() begins drawing to the vkCommandBuffer and returns VkCommandBuffer.
            // Get current vkCommandBuffer buffer, then  vkBeginCommandBuffer(current vkCommandBuffer, ...)
            VkCommandBuffer commandBuffer = lveRenderer.beginFrame();
            if ( commandBuffer )
            {
                int frameIndex = lveRenderer.getFrameIndex();
                
                FrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };
                
                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                
                // Render
                
                //   Record to vkCommandBuffer to begin this render pass vkCmdRenderPass(...).
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                
                //   Bind pipeline with vkCommandBuffer, vkCmdBindPipeline(...), then vkCmdDraw(...)
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                
                //   vkCmdEndRenderPass(...)
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                
                //   vkEndCommandBuffer(...), vkQueueSubmit(..., submitInfo containing buffer, ...)
                lveRenderer.endFrame();
            }
        }
        
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/models/flat_vase.obj");
        
        auto flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-.5f, .5f, 0.f};
        flatVase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
        gameObjects.push_back(std::move(flatVase));
        
        lveModel = LveModel::createModelFromFile(lveDevice, "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/models/smooth_vase.obj");
        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {.5f, .5f, 0.f};
        smoothVase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
        gameObjects.push_back(std::move(smoothVase));
        
        lveModel = LveModel::createModelFromFile(lveDevice, "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/models/quad.obj");
        auto floor = LveGameObject::createGameObject();
        floor.model = lveModel;
        floor.transform.translation = {0.f, .5f, 0.f};
        floor.transform.scale = glm::vec3(3.f, 1.0f, 3.f);
        gameObjects.push_back(std::move(floor));
    }

}// namespace lve

