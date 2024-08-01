#ifndef simple_render_system_hpp
#define simple_render_system_hpp

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include <memory>
#include <vector>
//
namespace lve
{
    class SimpleRenderSystem
    {
        public:
        
        SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();
        
        SimpleRenderSystem(const SimpleRenderSystem& o) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem& o) = delete;
        
        void renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<LveGameObject>& gameObjects,
                const LveCamera &camera);
        
        private:
        
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);
    
        LveDevice&                   lveDevice;
        VkPipelineLayout             pipelineLayout;
        std::unique_ptr<LvePipeline> lvePipeline;
        
    };
}

#endif
