//#include <stdio.h>
#include "lve_window.hpp"
#include <stdexcept>
//
namespace lve
{
//
    LveWindow::LveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    LveWindow::~LveWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LveWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        
    }

    bool LveWindow::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    VkExtent2D LveWindow::getExtent()
    {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    bool LveWindow::wasWindowResized()
    {
        return framebufferResized;
    }

    void LveWindow::resetWindowResizedFlag()
    {
        framebufferResized = false;
    }

    void LveWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto lveWindow = reinterpret_cast<LveWindow*>(glfwGetWindowUserPointer(window));
        lveWindow->framebufferResized = true;
        lveWindow->width = width;
        lveWindow->height = height;
    }

}
