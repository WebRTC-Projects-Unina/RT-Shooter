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
   

    Collider(glm::vec3 position, glm::vec3 scale, int type);

    glm::vec3 getBoxMin();
    glm::vec3 getBoxMax();
    glm::vec3 m_position;
    glm::vec3 m_scale;
    int m_type;

};

#endif