#include "lve_descriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace lve
{
    // LveDescriptorSetLayout methods:
    
    VkDescriptorSetLayout  LveDescriptorSetLayout::getVkDescriptorSetLayout() const
    {
        return _vkDescriptorSetLayout;
    }
    
    // DescriptorSetLayout::Builder methods:
    
    LveDescriptorSetLayout::Builder &LveDescriptorSetLayout::Builder::addBinding(
        uint32_t bindingKey,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count)
    {
        assert(_vkBindings.count(bindingKey) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding vkDescSetLayoutBinding{};
        vkDescSetLayoutBinding.binding = bindingKey;
        vkDescSetLayoutBinding.descriptorType = descriptorType;
        vkDescSetLayoutBinding.descriptorCount = count;
        vkDescSetLayoutBinding.stageFlags = stageFlags;
        _vkBindings[bindingKey] = vkDescSetLayoutBinding;
        return *this;
    }
    
    std::unique_ptr<LveDescriptorSetLayout> LveDescriptorSetLayout::Builder::build() const
    {
        return std::make_unique<LveDescriptorSetLayout>(lveDevice, _vkBindings);
    }
    
    // Descriptor Set Layout
    
    LveDescriptorSetLayout::LveDescriptorSetLayout(
        LveDevice &lveDevice,
        std::unordered_map<uint32_t,
        VkDescriptorSetLayoutBinding> bindings)
    :   _lveDevice{lveDevice},
        _vkBindings{bindings}
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCI{};
        
        vkDescriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        
        vkDescriptorSetLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        
        vkDescriptorSetLayoutCI.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            lveDevice.device(),
            &vkDescriptorSetLayoutCI,
            nullptr,
            &_vkDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
     
    LveDescriptorSetLayout::~LveDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(_lveDevice.device(), _vkDescriptorSetLayout, nullptr);
    }
    
    // Descriptor Pool Builder
    
    LveDescriptorPool::Builder &LveDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType,
        uint32_t count)
    {
        poolSizes.push_back({descriptorType, count});
        return *this;
    }
     
    LveDescriptorPool::Builder &LveDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
    {
        poolFlags = flags;
        return *this;
    }
    LveDescriptorPool::Builder &LveDescriptorPool::Builder::setMaxSets(uint32_t count)
    {
        maxSets = count;
        return *this;
    }
     
    std::unique_ptr<LveDescriptorPool> LveDescriptorPool::Builder::build() const
    {
        return std::make_unique<LveDescriptorPool>(lveDevice, maxSets, poolFlags, poolSizes);
    }
    
    // Descriptor Pool
    
    LveDescriptorPool::LveDescriptorPool(
        LveDevice &lveDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes)
    :   lveDevice{lveDevice}
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(lveDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
          VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
     
    LveDescriptorPool::~LveDescriptorPool()
    {
        vkDestroyDescriptorPool(lveDevice.device(), descriptorPool, nullptr);
    }
     
    // allocates Descriptor Set
    bool LveDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet &descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(lveDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }
     
    void LveDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
    {
        vkFreeDescriptorSets(
            lveDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }
     
    void LveDescriptorPool::resetPool()
    {
        vkResetDescriptorPool(lveDevice.device(), descriptorPool, 0);
    }
    
    // Descriptor Writer
    
    LveDescriptorWriter::LveDescriptorWriter(
        LveDescriptorSetLayout &setLayout,
        LveDescriptorPool &pool)
    :   _setLayout{setLayout},
        _pool{pool}
    {}
     
    LveDescriptorWriter &LveDescriptorWriter::writeBuffer(
        uint32_t bindingKey,
        VkDescriptorBufferInfo *bufferInfo)
    {
        assert(_setLayout._vkBindings.count(bindingKey) == 1 &&
               "Layout does not contain specified binding");

        VkDescriptorSetLayoutBinding& bindingDescription = _setLayout._vkBindings[bindingKey];

        assert(bindingDescription.descriptorCount == 1 &&
               "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = bindingKey;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        _writes.push_back(write);
        return *this;
    }
     
    bool LveDescriptorWriter::build(VkDescriptorSet &set)
    {
        bool success = _pool.allocateDescriptor(_setLayout.getVkDescriptorSetLayout(), set);
        if (!success)
        {
            return false;
        }
        overwrite(set);
        return true;
    }
     
    void LveDescriptorWriter::overwrite(VkDescriptorSet &set)
    {
        for (auto &write : _writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(_pool.lveDevice.device(), _writes.size(), _writes.data(), 0, nullptr);
    }
    
   LveDescriptorWriter &LveDescriptorWriter::writeImage(
       uint32_t binding,
       VkDescriptorImageInfo *imageInfo)
   {
       assert(_setLayout._vkBindings.count(binding) == 1 && "Layout does not contain specified binding");

       auto &bindingDescription = _setLayout._vkBindings[binding];

       assert(
         bindingDescription.descriptorCount == 1 &&
         "Binding single descriptor info, but binding expects multiple");

       VkWriteDescriptorSet write{};
       write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
       write.descriptorType = bindingDescription.descriptorType;
       write.dstBinding = binding;
       write.pImageInfo = imageInfo;
       write.descriptorCount = 1;

       _writes.push_back(write);
       return *this;
   }

}
