/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */
 
#include "lve_buffer.hpp"
 
// std
#include <cassert>
#include <cstring>
 
namespace lve {
 
/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize LveBuffer::getAlignment(
    VkDeviceSize instanceSize,
    VkDeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}
 
LveBuffer::LveBuffer(
    LveDevice &device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment
):  
    _lveDevice{device},
    _instanceSize{instanceSize},
    _instanceCount{instanceCount},
    _usageFlags{usageFlags},
    _memoryPropertyFlags{memoryPropertyFlags}
{
    _alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    _bufferSize = _alignmentSize * instanceCount;
    device.createBuffer(_bufferSize, usageFlags, memoryPropertyFlags, _buffer, _memory);
}
 
LveBuffer::~LveBuffer()
{
    unmap();
    vkDestroyBuffer(_lveDevice.device(), _buffer, nullptr);
    vkFreeMemory(_lveDevice.device(), _memory, nullptr);
}
 
/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult LveBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
    assert(_buffer && _memory && "Called map on buffer before create");
    return vkMapMemory(_lveDevice.device(), _memory, offset, size, 0, &_mapped);
}
 
/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void LveBuffer::unmap()
{
    if (_mapped)
    {
        vkUnmapMemory(_lveDevice.device(), _memory);
        _mapped = nullptr;
    }
}
 
/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void LveBuffer::writeToBuffer(
    void *data,
    VkDeviceSize size,
    VkDeviceSize offset)
{
    assert(_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        // memcpy is (dest, src, count)
        memcpy(_mapped, data, _bufferSize);
    }
    else
    {
        char *memOffset = (char *)_mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}
 
/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult LveBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = _memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(_lveDevice.device(), 1, &mappedRange);
}
 
/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult LveBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = _memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(_lveDevice.device(), 1, &mappedRange);
}
 
/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo LveBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{_buffer, offset, size};
}
 
/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void LveBuffer::writeToIndex(void *data, int index)
{
    writeToBuffer(data, _instanceSize, index * _alignmentSize);
}
 
/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult LveBuffer::flushIndex(int index)
{
    return flush(_alignmentSize, index * _alignmentSize);
}
 
/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo LveBuffer::descriptorInfoForIndex(int index)
{
    return descriptorInfo(_alignmentSize, index * _alignmentSize);
}
 
/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult LveBuffer::invalidateIndex(int index)
{
    return invalidate(_alignmentSize, index * _alignmentSize);
}
    
VkBuffer LveBuffer::getBuffer() const
{
        return _buffer;
}

void* LveBuffer::getMappedMemory() const
{
    return _mapped;
}

uint32_t LveBuffer::getInstanceCount() const
{ 
    return _instanceCount;
}

VkDeviceSize LveBuffer::getInstanceSize() const
{
    return _instanceSize;
}

VkDeviceSize LveBuffer::getAlignmentSize() const
{
    return _instanceSize;
}

VkBufferUsageFlags LveBuffer::getUsageFlags() const
{
    return _usageFlags;
}

VkMemoryPropertyFlags LveBuffer::getMemoryPropertyFlags() const
{
    return _memoryPropertyFlags;
}
    
VkDeviceSize LveBuffer::getBufferSize() const
{
    return _bufferSize;
}
 
}  // namespace lve
