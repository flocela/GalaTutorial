#include "lve_model.hpp"
#include "lve_utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace std
{
    template<>
    struct hash<lve::LveModel::Vertex>
    {
        size_t operator()(lve::LveModel::Vertex const &vertex) const
        {
            size_t seed = 0;
            lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace lve
{
    LveModel::LveModel(LveDevice& device, const LveModel::Builder& builder)
    : _lveDevice{device}
    {
        createVertexBuffers(builder._vertices);
        createIndexBuffers(builder._indices);
    }

    // TODO make destructor the default implimentation = default.
    LveModel::~LveModel()
    {}
    
    std::unique_ptr<LveModel> LveModel::createModelFromFile(LveDevice& device, const std::string& filepath)
    {
        Builder builder{};
        builder.loadModel(filepath);
        std::cout << "Vertex count: " << builder._vertices.size() << "\n";
        return std::make_unique<LveModel>(device, builder);
    }

    void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        _vertexCount = static_cast<uint32_t>(vertices.size());
        assert(_vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);
        
        LveBuffer stagingBuffer {
            _lveDevice,
            vertexSize,
            _vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        }; // Creates stagingBuffer's VkBuffer buffer and VkDevice memory attributes. Binds these two attributes.
        
        stagingBuffer.map(); // Maps stagingBuffer's memory to (void*) mapped
        stagingBuffer.writeToBuffer((void*)vertices.data()); // Writes from vertices to mapped.
        
        _vertexBuffer = std::make_unique<LveBuffer>(
            _lveDevice,
            vertexSize,
            _vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        ); // Creates vertexBuffer's VkBuffer buffer and VkDevice memory attributes. Binds these two attributes.
        //                   src,                       dst
        _lveDevice.copyBuffer(stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), bufferSize); // Copies from stagingBuffer's buffer to vertexBuffer's buffer.
    }
    
    void LveModel::createIndexBuffers(const std::vector<uint32_t>& indices)
    {
        _indexCount = static_cast<uint32_t>(indices.size());
        _hasIndexBuffer = _indexCount > 0;
        
        if(!_hasIndexBuffer)
        {
            return;
        }
        
        VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;
        uint32_t indexSize = sizeof(indices[0]);
        
        LveBuffer stagingBuffer{
            _lveDevice,
            indexSize,
            _indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());
        
        _indexBuffer = std::make_unique<LveBuffer>(
            _lveDevice,
            indexSize,
            _indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        _lveDevice.copyBuffer(stagingBuffer.getBuffer(), _indexBuffer->getBuffer(), bufferSize);
    }

    // Bind vertex buffer to command buffer.
    void LveModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {_vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        
        if(_hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
        
    }

    // draw primitives, first is assembling primitives.
    void LveModel::draw(VkCommandBuffer commandBuffer)
    {
        if(_hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
        }
        else
        {
            // draw vertexCount number of vertices and one instance.
            vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
        }
        
    }

    std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
        attributeDescriptions.push_back(
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back(
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back(
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back(
            {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        
        return attributeDescriptions;
    }
    
    void LveModel::Builder::loadModel(const std::string &filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        
        if( !tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }
        
        _vertices.clear();
        _indices.clear();
        
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        
        for(const auto& shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};
                if(index.vertex_index >= 0)
                {
                    vertex.position =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    
                    vertex.color =
                    {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }
                
                if(index.normal_index >= 0)
                {
                    vertex.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                
                if(index.texcoord_index >= 0)
                {
                    vertex.uv =
                    {
                        attrib.texcoords[2 * index.normal_index + 0],
                        attrib.texcoords[2 * index.normal_index + 1]
                    };
                }
                
                if(uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                    _vertices.push_back(vertex);
                }
                _indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
                               
        
}
