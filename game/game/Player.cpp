#include "Player.h"
#include <GL/gl.h>
#include <GLES3/gl3.h> // holds all OpenGL type declarations

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


#include <iostream>
#include <string>
#include <vector>
#include "../renderer/Camera.h"
#include "../renderer/renderer.h"

#include "Collider.h"

Player::Player(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 direction)
    : headHitBox(position + glm::vec3(0, HEADHITBOX_VOFFESET, 0), glm::vec3(0.14f,HEADHITBOX_VDIM,0.14f), 0) 
    , bodyHitBox(position, glm::vec3(0.14f,BODYHITBOX_VDIM,0.14f), 1)

{
    m_hp = base_hp;
    Player::setPosition(position);
    Player::setVelocity(velocity);
    Player::setDirection(direction);
}

glm::vec3 Player::getPosition()
{
    return m_position;
}

void Player::setPosition(const glm::vec3 position)
{
    m_position = position;
    headHitBox.m_position = glm::vec3(position.x, position.y+HEADHITBOX_VOFFESET, position.z);
    bodyHitBox.m_position = position;
}

void Player::setVelocity(const glm::vec3 velocity)
{
    m_velocity = velocity;
}

glm::vec3 Player::getDirection()
{
    return m_direction;
}

void Player::setDirection(const glm::vec3 direction)
{
    m_direction = direction;
}


ClientPlayer::ClientPlayer(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 direction)  
: Player(position, velocity, direction)
{
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

void ClientPlayer::processMouseMovement(float xoffset, float yoffset)
{
    camera.ProcessMouseMovement(xoffset, yoffset);
    m_direction = camera.Front;
}

void ClientPlayer::processMovement(Movement_Direction relativeDirection, float deltaTime)
{
    glm::vec3 front(m_direction.x, 0, m_direction.z);
    if (relativeDirection == FORWARD){
        m_velocity += front * accelleration * deltaTime;
        b_isMoving = true;
    }
    else if (relativeDirection == BACKWARD){
        m_velocity -= front * accelleration * deltaTime;
        b_isMoving = true;
    }
    else if (relativeDirection == LEFT){
        m_velocity -= camera.Right * accelleration * deltaTime;
        b_isMoving = true;
    }
    else if (relativeDirection == RIGHT){
        m_velocity += camera.Right * accelleration * deltaTime;
        b_isMoving = true;
    }
    else if (relativeDirection == JUMP && m_position.y <= 0.05)
        m_velocity.y = jump_velocity;
}



void ClientPlayer::updatePosition(float deltaTime)
{
    
    glm::vec2 horizontalVelocity(m_velocity.x, m_velocity.z);
    if (glm::length(horizontalVelocity) > max_velocity)
    {
        horizontalVelocity = glm::normalize(horizontalVelocity) * max_velocity;
        m_velocity = glm::vec3(horizontalVelocity.x, m_velocity.y, horizontalVelocity.y);
    }

    
    m_velocity.y -= deltaTime * gravity;

    checkCollission(deltaTime);
    m_position += m_velocity * deltaTime;

    if (m_position.y < 0) {
        m_position.y = 0;
        m_velocity.y = 0;
    }


    if(!b_isMoving){
        float friction = m_position.y < 0.3 ? walking_friction : aereal_friction;

    
        float velocityMagnitude = glm::length(horizontalVelocity); 
        velocityMagnitude -= friction * deltaTime;
        if (velocityMagnitude < 0.2) velocityMagnitude = 0;

        if (velocityMagnitude != 0){
            horizontalVelocity = glm::normalize(horizontalVelocity) * velocityMagnitude;
            m_velocity = glm::vec3(horizontalVelocity.x, m_velocity.y, horizontalVelocity.y);
            }
        else
            m_velocity = glm::vec3(.0f, m_velocity.y, .0f);
            
    }
    b_isMoving = false;

   

    camera.Position = glm::vec3(m_position.x, m_position.y+0.2, m_position.z);
    //std::cout << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
}

void ClientPlayer::checkCollission(float deltaTime)
{ 
    if(map.empty()) return;
    {
        glm::vec2 roundedPosition((int)(m_position.x + (m_velocity.x*deltaTime)), ((int)m_position.z));
        if(map[roundedPosition.x+(((roundedPosition.y))*map_width)] == 1) {
            m_velocity.x = 0;
            if (m_position.x - ((int)m_position.x) > 0.998f) m_position.x = ((int)m_position.x) + 0.998f;
            else if (m_position.x - ((int)m_position.x) < 0.001f) m_position.x = ((int)m_position.x) + 0.001f;

        }
    }

    glm::vec2 roundedPosition((int)m_position.x, (int)(m_position.z + (m_velocity.z*deltaTime)));
    if(map[roundedPosition.x+((roundedPosition.y)*map_width)] == 1){
        m_velocity.z = 0;
        if (m_position.z - ((int)m_position.z) > 0.998f) m_position.z = ((int)m_position.z) + 0.998f;
        else if (m_position.z - ((int)m_position.z) < 0.001f) m_position.z = ((int)m_position.z) + 0.001f;

    } 
}
