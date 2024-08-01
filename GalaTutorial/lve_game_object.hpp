#pragma once

#include "lve_model.hpp"
#include <memory>

namespace lve
{

struct Transform2dComponent
{
    
    glm::vec2 translation{};
    glm::vec2 scale{1.f, 1.f};
    float rotation;
    
    glm::mat2 mat2()
    {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);
        glm::mat2 rotMatrix{{c, s}, {-s, c}};
        // takes columns, not rows.
        glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
        return rotMatrix * scaleMat;
        
    }
};

class LveGameObject
{
    public:
    
    using id_t = unsigned int;
    
    static LveGameObject createGameObject()
    {
        static id_t currentId = 0;
        return LveGameObject{currentId++};
    }
    
    LveGameObject(const LveGameObject& o) = delete;
    LveGameObject& operator=(const LveGameObject& o) = delete;
    LveGameObject(LveGameObject&& o) = default;
    LveGameObject& operator=(LveGameObject&& o) = default;
    
    id_t getId()
    {
        return id;
    }
    
    std::shared_ptr<LveModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d{};
    
    
    private:
    
    LveGameObject(id_t objId) : id{objId}
    {
        
    }
    
    id_t id;
    
};



}