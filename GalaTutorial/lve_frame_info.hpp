#ifndef lve_frame_info_hpp
#define lve_frame_info_hpp

#pragma once

#include "lve_camera.hpp"

#include <vulkan/vulkan.h>

namespace lve
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LveCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}

#endif /* lve_frame_info_hpp */
