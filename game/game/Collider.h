#ifndef COLLIDER_H
#define COLLIDER_H

#include <GL/gl.h>
#include <GLES3/gl3.h> // holds all OpenGL type declarations

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <vector>

#define COLLIDER_DEFAULT_SCALE 1.f


class Collider {
public:
   
 


    Collider(glm::vec2 position);
    Collider(float positionX, float positionY);
    Collider(glm::vec2 position, glm::vec2 scale);
    Collider(float positionX, float positionY, glm::vec2 scale);
    Collider(glm::vec2 position, float scaleX, float scaleY);
    Collider(glm::vec2 position, float scale);




    static bool areColliding(Collider A, Collider B);
    bool isColling(Collider B);

    int getHp();


protected:
    glm::vec2 m_position;
    glm::vec2 m_scale; 
    glm::vec4 m_vertici;

    



};

#endif