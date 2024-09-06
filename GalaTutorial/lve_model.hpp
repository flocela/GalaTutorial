#pragma once
#include "lve_buffer.hpp"
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
            
            bool operator==(const Vertex &other) const
            {
                return  position == other.position &&
                        color == other.color &&
                        normal == other.normal &&
                        uv == other.uv;
            }
        };
        
        struct Builder
        {
            std::vector<Vertex> _vertices{};
            std::vector<uint32_t> _indices{};
            
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
        
        LveDevice& _lveDevice;
        
        std::unique_ptr<LveBuffer> _vertexBuffer;
        uint32_t _vertexCount;
        
        bool _hasIndexBuffer = false;
        std::unique_ptr<LveBuffer> _indexBuffer;
        uint32_t _indexCount;
        
    };
}
