#ifndef first_app_hpp
#define first_app_hpp

#include <stdio.h>
#include "lve_pipeline.hpp"
#include "lve_window.hpp"
//
namespace lve
{
    class FirstApp
    {
        public:
        
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        
        void run();
        
        private:
        
        // TODO can't use whole path name here!
        LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        LveDevice lveDevice{lveWindow};
        LvePipeline lvePipeline{
            lveDevice,
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.vert.spv",
            "/Users/flo/LocalDocuments/Projects/VulkanLearning/GalaTutorial/GalaTutorial/shaders/simple_shader.frag.spv",
            LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}

#endif /* first_app_hpp */
