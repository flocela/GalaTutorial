#ifndef lve_descriptors_hpp
#define lve_descriptors_hpp

#pragma once
 
#include "lve_device.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
 
namespace lve {
 
    // LveDescriptorSetLayout
    class LveDescriptorSetLayout {
        
        public:
            class Builder {
                
                public:
                
                Builder(LveDevice &lveDevice) : lveDevice{lveDevice} {}
                
                Builder &addBinding(
                    uint32_t bindingKey,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);
                
                std::unique_ptr<LveDescriptorSetLayout> build() const;
                
                private:
                
                LveDevice &lveDevice;
                
                std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
                    _vkBindings{};
            }; // End Builder class

        public:
            LveDescriptorSetLayout(
                LveDevice &lveDevice,
                std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        
            ~LveDescriptorSetLayout();
        
            LveDescriptorSetLayout(const LveDescriptorSetLayout &) = delete;
        
            LveDescriptorSetLayout &operator=(const LveDescriptorSetLayout &) = delete;
        VkDescriptorSetLayout getVkDescriptorSetLayout() const;

        private:
            LveDevice&              _lveDevice;
            VkDescriptorSetLayout   _vkDescriptorSetLayout;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>
                                    _vkBindings;

            friend class LveDescriptorWriter;
    };
//
    // LveDescriptorPool
    class LveDescriptorPool {
        
        public:
            class Builder {
                public:
                Builder(LveDevice &lveDevice) : lveDevice{lveDevice} {}
                Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
                Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
                Builder &setMaxSets(uint32_t count);
                std::unique_ptr<LveDescriptorPool> build() const;
                private:
                LveDevice &lveDevice;
                std::vector<VkDescriptorPoolSize> poolSizes{};
                uint32_t maxSets = 1000;
                VkDescriptorPoolCreateFlags poolFlags = 0;
            };

            LveDescriptorPool(
                LveDevice &lveDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
            ~LveDescriptorPool();
            LveDescriptorPool(const LveDescriptorPool &) = delete;
            LveDescriptorPool &operator=(const LveDescriptorPool &) = delete;
            bool allocateDescriptor (const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &vkescriptorSet) const;
            void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
            void resetPool();

        private:
            LveDevice &lveDevice;
            VkDescriptorPool descriptorPool;
            friend class LveDescriptorWriter;
    };

    // LveDescriptorWriter
    class LveDescriptorWriter {
        
        public:
        
            LveDescriptorWriter(
                LveDescriptorSetLayout &setLayout,
                LveDescriptorPool &pool);
        
            LveDescriptorWriter &writeBuffer(
                uint32_t binding,
                VkDescriptorBufferInfo *bufferInfo);
        
            LveDescriptorWriter &writeImage(
                uint32_t binding,
                VkDescriptorImageInfo *imageInfo);
        
            bool build(VkDescriptorSet &set);
        
            void overwrite(VkDescriptorSet &set);
        
        
        private:
        
            LveDescriptorSetLayout&             _setLayout;
            LveDescriptorPool&                  _pool;
            std::vector<VkWriteDescriptorSet>   _writes;
        
    };
}  // namespace lve


#endif /* lve_descriptors_hpp */
