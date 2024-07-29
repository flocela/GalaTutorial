#ifndef lve_pipeline_hpp
#define lve_pipeline_hpp

#include "lve_device.hpp"
#include <string>
#include <vector>

namespace lve
{
    struct LvePipelineConfigInfo
    {
        VkPipelineInputAssemblyStateCreateInfo  inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo  rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo    multisampleInfo;
        VkPipelineColorBlendStateCreateInfo     colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo   depthStencilInfo;
        
        VkViewport                          viewport;
        VkRect2D                            scissor;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineLayout                    pipelineLayout = nullptr;
        VkRenderPass                        renderPass = nullptr;
        uint32_t                            subpass = 0;
    };

    class LvePipeline
    {
        public:
        
        LvePipeline(
            LveDevice& device,
            const std::string& vertFilepath,
            const std::string fragFilepath,
            const LvePipelineConfigInfo& lvePipelineCI
        );
        
        ~LvePipeline();
        
        LvePipeline(const LvePipeline& o) = delete;
        LvePipeline& operator=(const LvePipeline& o) = delete;
        
        void bind(VkCommandBuffer commandBuffer);
        static LvePipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        private:
        
        static std::vector<char> readFile(const std::string& filepath);
        
        void createGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string fragFilepath,
            const LvePipelineConfigInfo& configInfo
        );
        
        void createShaderModule(
            const std::vector<char>& code,
            VkShaderModule& shaderModule
        );
        
        LveDevice& lveDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

}

#endif
