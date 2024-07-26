#ifndef first_app_hpp
#define first_app_hpp

#include <stdio.h>
#include "lve_pipeline.hpp"
#include "lve_window.hpp"
#include "lve_swap_chain.hpp"
#include <memory>
#include <vector>
//
namespace lve
{
    class FirstApp
    {
        public:
        
        FirstApp();
        ~FirstApp();
        
        FirstApp(const FirstApp& o) = delete;
        FirstApp& operator=(const FirstApp& o) = delete;
        
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        
        void run();
        
        private:
        
        // TODO can't use whole path name here!
        LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        LveDevice lveDevice{lveWindow};
        LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<LvePipeline> lvePipeline;
        std::vector<VkCommandBuffer> commandBuffers;
        
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
    };
}

#endif /* first_app_hpp */
