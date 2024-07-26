#ifndef lve_window_h
#define lve_window_h

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve{

    class LveWindow
    {
        public:
        
        LveWindow(int w, int h, std::string name);
        ~LveWindow();
        LveWindow(const LveWindow& o) = delete;
        LveWindow& operator= (const LveWindow& o) = delete;
        
        bool shouldClose();
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
        
        // initializes VkSurfaceKHR*
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        
        private:
        
        void initWindow();
        const int width;
        const int height;
        std::string windowName;
        GLFWwindow *window;
    };
    
}

#endif /* lve_window_h */
