#ifndef first_app_hpp
#define first_app_hpp

#include <stdio.h>
#include "lve_descriptors.hpp"
#include "lve_game_object.hpp"
#include "lve_window.hpp"
#include "lve_renderer.hpp"
#include "lve_model.hpp"
#include <memory>
#include <vector>

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
        
        LveWindow                    _lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        LveDevice                    _lveDevice{_lveWindow};
        LveRenderer                  _lveRenderer{_lveWindow, _lveDevice};
        
        // note: order of declaration matters. Pool needs a device.
        std::unique_ptr<LveDescriptorPool> globalPool{};
        std::vector<LveGameObject>   gameObjects;
        
        void loadGameObjects();
    };
}

#endif /* first_app_hpp */
