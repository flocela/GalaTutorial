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
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };
    
    SimpleRenderSystem::SimpleRenderSystem(
        LveDevice& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout)
    :   _lveDevice{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDestroyPipelineLayout(_lveDevice.device(), _vkPipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(
        VkDescriptorSetLayout globalSetLayout)
    {
        // Not sending push constant here. Just telling Pipeline that one (or many) will be sent.
        VkPushConstantRange vkPushConstantRange{};
        vkPushConstantRange.stageFlags =
            VK_SHADER_STAGE_VERTEX_BIT |
            VK_SHADER_STAGE_FRAGMENT_BIT;
        vkPushConstantRange.offset = 0;
        vkPushConstantRange.size = sizeof(SimplePushConstantData);
        
        // pipelineLayoutInfo has pSetLayouts and pPushConstantRanges. This is information that is sent to the shader programs.
        // pSetLayouts is used to pass data other than vertex data to our vertex and fragment shaders (that is textures and uniform buffer objects).
        // push constants send small amount of data to shader programs.
        
        std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts{globalSetLayout};
        
        
        VkPipelineLayoutCreateInfo vkPipelineLayoutInfoCI{};
        vkPipelineLayoutInfoCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vkPipelineLayoutInfoCI.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
        vkPipelineLayoutInfoCI.pSetLayouts = vkDescriptorSetLayouts.data();
        vkPipelineLayoutInfoCI.pushConstantRangeCount = 1;
        vkPipelineLayoutInfoCI.pPushConstantRanges = &vkPushConstantRange;
        if ( vkCreatePipelineLayout(_lveDevice.device(), &vkPipelineLayoutInfoCI, nullptr, &_vkPipelineLayout) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(_vkPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");
        
        LvePipelineConfigInfo lvePipelineCI {};
        LvePipeline::defaultPipelineConfigInfo(lvePipelineCI);
        
        lvePipelineCI.renderPass = renderPass;
        
        lvePipelineCI.pipelineLayout = _vkPipelineLayout;
        
        _lvePipeline = std::make_unique<LvePipeline>(
            _lveDevice,
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.vert.spv",
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.frag.spv",
            lvePipelineCI
        );
    }

    void SimpleRenderSystem::renderGameObjects(
            FrameInfo& frameInfo,
            std::vector<LveGameObject>& gameObjects)
    {
        _lvePipeline->bind(frameInfo.commandBuffer);
        
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vkPipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);
        
        for (LveGameObject& obj: gameObjects)
        {
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();
            
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                _vkPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }

}
