#pragma once

#include "lve_model.hpp"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace lve
{

struct TransformComponent
{
    glm::vec3 _translation{};
    glm::vec3 _scale{1.f, 1.f, 1.f};
    glm::vec3 _rotation{};
    
    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4();
    glm::mat3 normalMatrix();
    
};
//
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
    
    id_t getId();
    
    std::shared_ptr<LveModel> _model{};
    
    glm::vec3 _color{};
    
    TransformComponent _transformComp{};
    
    private:
    
    LveGameObject(id_t objId);
    
    id_t _id;
    
};



}
