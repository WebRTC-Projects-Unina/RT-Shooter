#include "Collider.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>


Collider::Collider(glm::vec3 position, glm::vec3 scale, int type)
{
    m_position = position;
    m_scale = scale;
    m_type = type;
}


glm::vec3 Collider::getBoxMin()
{   
    return m_position - glm::vec3(m_scale.x * 0.5f, 0, m_scale.z * 0.5f);
}

glm::vec3 Collider::getBoxMax()
{   
    return m_position + glm::vec3(m_scale.x * 0.5f, m_scale.y, m_scale.z * 0.5f);
}
