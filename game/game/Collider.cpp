#include "Collider.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>


Collider::Collider(glm::vec2 position, glm::vec2 scale)
{
    m_position = position;
    m_scale = scale;
    m_vertici = glm::vec4(position.x - (scale.x/2), position.y - (scale.y/2), position.x + (scale.x/2), position.y + (scale.y/2));
}

Collider::Collider(glm::vec2 position)
{
    Collider(position, glm::vec2(COLLIDER_DEFAULT_SCALE));
}

Collider::Collider(float positionX, float positionY)
{
    Collider(glm::vec2(positionX, positionY));
}


Collider::Collider(float positionX, float positionY, glm::vec2 scale)
{
    Collider(glm::vec2(positionX, positionY), scale);
}

Collider::Collider(glm::vec2 position, float scaleX, float scaleY)
{
    Collider(position, glm::vec2(scaleX, scaleY));
}

Collider::Collider(glm::vec2 position, float scale)
{
    Collider(position, glm::vec2(scale));
}

bool Collider::isColling(Collider ColliderB)
{
       
    glm::vec4 A = m_vertici;
    glm::vec4 B = ColliderB.m_vertici;

    return (A.x > B.x && A.x < B.z) && (A.y > B.y && A.y < B.w)
        || (A.x > B.x && A.x < B.z) && (A.w > B.y && A.w < B.w)
        || (A.z > B.x && A.z < B.z) && (A.y > B.y && A.y < B.w)
        || (A.z > B.x && A.z < B.z) && (A.w > B.y && A.w < B.w);
}


bool Collider::areColliding(Collider ColliderA, Collider ColliderB)
{
    return ColliderA.isColling(ColliderB);
}
