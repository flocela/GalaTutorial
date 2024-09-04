#ifndef simple_render_system_hpp
#define simple_render_system_hpp

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "lve_frame_info.hpp"
#include <memory>
#include <vector>
//
namespace lve
{
    class SimpleRenderSystem
    {
        public:
        
        SimpleRenderSystem(
            LveDevice &device,
            VkRenderPass renderPass,
            VkDescriptorSetLayout globalSetLayout);
        
        ~SimpleRenderSystem();
        
        SimpleRenderSystem(const SimpleRenderSystem& o) = delete;
        
        SimpleRenderSystem& operator=(const SimpleRenderSystem& o) = delete;
        
        void renderGameObjects(
            FrameInfo& frameInfo,
            std::vector<LveGameObject>& gameObjects);
        
        private:
        
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);
    
        LveDevice&                   _lveDevice;
        VkPipelineLayout             _vkPipelineLayout;
        std::unique_ptr<LvePipeline> _lvePipeline;
        
    };
}

#endif
