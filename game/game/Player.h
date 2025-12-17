#ifndef PLAYER_H
#define PLAYER_H

#include <GL/gl.h>
#include <GLES3/gl3.h> // holds all OpenGL type declarations

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "../renderer/Camera.h"
#include "Collider.h"
#include <iostream>
#include <string>
#include <vector>

#define HEADHITBOX_VOFFESET 0.15f
#define HEADHITBOX_VDIM 0.07f
#define BODYHITBOX_VDIM 0.1499f


class Player {
public:
    inline static float gravity = 9.81;
    inline static float jump_velocity = 4;
    inline static float max_velocity = 3;
    inline static float max_vertical_velcity = 4;
    inline static float walking_friction = 70;
    inline static float aereal_friction = 5;
    inline static float accelleration = 60;
    inline static float max_hp = 100;
    inline static float base_hp = 80;

   // Collider collider = 0;

    Player(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 direction);
    
    glm::vec3 getPosition();
    void setPosition(const glm::vec3 position);
    
    glm::vec3 getVelocity();
    void setVelocity(const glm::vec3 velocity);

    glm::vec3 getDirection();
    void setDirection(const glm::vec3 direction);


    bool isColliding();

    int getHp();

    Collider headHitBox;
    Collider bodyHitBox; 

protected:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_direction;
    bool b_isMoving = false; 
    int m_hp;


    



};


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Movement_Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    JUMP,
    UP
};


class ClientPlayer : public Player {
public:
    Camera camera;
    ClientPlayer(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 direction);
    void processMouseMovement(float xoffset, float yoffset);
    void processMovement(Movement_Direction relativeDirection, float deltaTime);
    void updatePosition(float deltaTime);
private:
    void checkCollission(float deltaTime);

};

class RemotePlayer : public Player {
    public: 
    float Yaw = 0;
    using Player::Player;
};
#endif