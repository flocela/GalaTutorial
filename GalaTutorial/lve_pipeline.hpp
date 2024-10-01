#ifndef lve_pipeline_hpp
#define lve_pipeline_hpp

#include "lve_device.hpp"
#include <string>
#include <vector>

namespace lve
{//
    struct LvePipelineConfigInfo
    {
        LvePipelineConfigInfo() = default;
        LvePipelineConfigInfo(const LvePipelineConfigInfo&) = delete;
        LvePipelineConfigInfo& operator=(const LvePipelineConfigInfo&) = delete;
        
        VkPipelineInputAssemblyStateCreateInfo  inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo  rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo    multisampleInfo;
        VkPipelineColorBlendStateCreateInfo     colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo   depthStencilInfo;
        
        VkPipelineViewportStateCreateInfo   viewportInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineLayout                    pipelineLayout = nullptr;
        std::vector<VkDynamicState>         dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo    dynamicStateInfo;
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
        static void defaultPipelineConfigInfo(LvePipelineConfigInfo& configInfo);
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
