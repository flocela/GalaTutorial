#pragma once
#include "lve_device.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//
namespace lve
{
    class LveModel
    {
        public:
        
        struct Vertex{
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal{};
            glm::vec2 uv{};
            
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        
        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            
            void loadModel(const std::string &filepath);
        };
        
        LveModel(LveDevice& device, const LveModel::Builder& builder);
        ~LveModel();
        
        LveModel(const LveModel& o) = delete;
        LveModel& operator=(const LveModel& o) = delete;
        
        static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);
        
        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);
        
        private:
        
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);
        LveDevice& lveDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
        
        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
        
    };
}
