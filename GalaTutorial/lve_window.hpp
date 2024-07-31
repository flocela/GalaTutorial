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
        VkExtent2D getExtent();
        bool wasWindowResized();
        void resetWindowResizedFlag();
        
        // initializes VkSurfaceKHR*
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        
        private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        void initWindow();
        int width;
        int height;
        bool framebufferResized = false;
        std::string windowName;
        GLFWwindow *window;
    };
    
}

#endif /* lve_window_h */
