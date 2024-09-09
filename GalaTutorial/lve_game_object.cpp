#include "lve_game_object.hpp"

namespace lve
{
    glm::mat4 TransformComponent::mat4() {
        const float c3 = glm::cos(_rotation.z);
        const float s3 = glm::sin(_rotation.z);
        const float c2 = glm::cos(_rotation.x);
        const float s2 = glm::sin(_rotation.x);
        const float c1 = glm::cos(_rotation.y);
        const float s1 = glm::sin(_rotation.y);
        return glm::mat4{
            {
                _scale.x * (c1 * c3 + s1 * s2 * s3),
                _scale.x * (c2 * s3),
                _scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                _scale.y * (c3 * s1 * s2 - c1 * s3),
                _scale.y * (c2 * c3),
                _scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                _scale.z * (c2 * s1),
                _scale.z * (-s2),
                _scale.z * (c1 * c2),
                0.0f,
            },
            {_translation.x, _translation.y, _translation.z, 1.0f}
        };
    }
    
    glm::mat3 TransformComponent::normalMatrix() {
        const float c3 = glm::cos(_rotation.z);
        const float s3 = glm::sin(_rotation.z);
        const float c2 = glm::cos(_rotation.x);
        const float s2 = glm::sin(_rotation.x);
        const float c1 = glm::cos(_rotation.y);
        const float s1 = glm::sin(_rotation.y);
        const glm::vec3 invScale = 1.0f / _scale;

        return glm::mat3{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1)
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3)
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2)
            }
        };
    }

    LveGameObject::LveGameObject(id_t objId)
    : _id{objId}
    {}
    
    id_t LveGameObject::getId()
    {
        return _id;
    }
}
