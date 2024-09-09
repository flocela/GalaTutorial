#ifndef lve_renderer_hpp
#define lve_renderer_hpp

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"
#include <cassert>
#include <memory>
#include <vector>
//
namespace lve
{
    class LveRenderer
    {
        public:
        
        LveRenderer(LveWindow &window, LveDevice& device);
        LveRenderer(const LveRenderer& o) = delete;
        LveRenderer& operator=(const LveRenderer& o) = delete;
        ~LveRenderer();
        
        VkRenderPass getSwapChainRenderPass() const;
        
        float getAspectRatio() const;
        
        bool isFrameInProgress() const;
        
        VkCommandBuffer getCurrentCommandBuffer() const;
        
        int getFrameIndex() const;
        
        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
        
        private:
        
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();
        
        LveWindow& _lveWindow;
        LveDevice& _lveDevice;
        std::unique_ptr<LveSwapChain> _lveSwapChain;
        std::vector<VkCommandBuffer> _commandBuffers;
        
        uint32_t _currentImageIndex;
        //int currentFrameIndex;
        //bool isFrameStarted;
        int _currentFrameIndex{0};
        bool _isFrameStarted{false};
    };
}

#endif
