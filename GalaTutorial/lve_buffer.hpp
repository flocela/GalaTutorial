#ifndef lve_buffer_hpp
#define lve_buffer_hpp

#pragma once
 
#include "lve_device.hpp"
 
namespace lve {
 
    class LveBuffer
    {
        public:
        
        LveBuffer(
            LveDevice& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        
        ~LveBuffer();

        LveBuffer(const LveBuffer&) = delete;
        
        LveBuffer& operator=(const LveBuffer&) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        
        void unmap();

        void writeToBuffer(
            void* data,
            VkDeviceSize size = VK_WHOLE_SIZE,
            VkDeviceSize offset = 0);
        
        VkResult flush(
            VkDeviceSize size = VK_WHOLE_SIZE,
            VkDeviceSize offset = 0);
        
        VkDescriptorBufferInfo descriptorInfo(
            VkDeviceSize size = VK_WHOLE_SIZE,
            VkDeviceSize offset = 0);
        
        VkResult invalidate(
            VkDeviceSize size = VK_WHOLE_SIZE,
            VkDeviceSize offset = 0);

        void writeToIndex(void* data, int index);
        
        VkResult flushIndex(int index);
        
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        
        VkResult invalidateIndex(int index);

        VkBuffer getBuffer() const;
        
        void* getMappedMemory() const;
        
        uint32_t getInstanceCount() const;
        
        VkDeviceSize getInstanceSize() const;
        
        VkDeviceSize getAlignmentSize() const;
        
        VkBufferUsageFlags getUsageFlags() const;
        
        VkMemoryPropertyFlags getMemoryPropertyFlags() const;
        VkDeviceSize getBufferSize() const;

        private:
        
        static VkDeviceSize getAlignment(
            VkDeviceSize instanceSize,
            VkDeviceSize minOffsetAlignment);

        LveDevice&              _lveDevice;
        void*                   _mapped = nullptr;
        VkBuffer                _buffer = VK_NULL_HANDLE;
        VkDeviceMemory          _memory = VK_NULL_HANDLE;

        VkDeviceSize            _bufferSize;
        uint32_t                _instanceCount;
        VkDeviceSize            _instanceSize;
        VkDeviceSize            _alignmentSize;
        VkBufferUsageFlags      _usageFlags;
        VkMemoryPropertyFlags   _memoryPropertyFlags;
    };
 
}  // namespace lve

#endif /* lve_buffer_hpp */
