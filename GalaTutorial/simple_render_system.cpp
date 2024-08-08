#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <stdexcept>
#include <glm/gtc/constants.hpp>
//
namespace lve
{
    struct SimplePushConstantData
    {
        glm::mat4 transform{1.f};
        glm::mat4 normalMatrix{1.f};
    };
    
    SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass)
    : lveDevice{device}
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout()
    {
        // Not sending push constant here. Just telling Pipeline that one (or many) will be sent.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);
        
        // pipelineLayoutInfo has pSetLayouts and pPushConstantRanges. This is information that is sent to the shader programs.
        // pSetLayouts is used to pass data other than vertex data to our vertex and fragment shaders (that is textures and uniform buffer objects).
        // push constants send small amount of data to shader programs.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if ( vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");
        
        LvePipelineConfigInfo pipelineConfig {};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        
        pipelineConfig.renderPass = renderPass;
        
        pipelineConfig.pipelineLayout = pipelineLayout;
        
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.vert.spv",
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.frag.spv",
            pipelineConfig
        );
    }

    void SimpleRenderSystem::renderGameObjects(
            FrameInfo& frameInfo,
            std::vector<LveGameObject>& gameObjects)
    {
        lvePipeline->bind(frameInfo.commandBuffer);
        
        auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();
        
        for (LveGameObject& obj: gameObjects)
        {
            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();
            
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }

}
