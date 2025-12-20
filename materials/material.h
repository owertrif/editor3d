#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class Material{
public:
    unsigned int diffuse;
    unsigned int specular;
    unsigned int emission;
    float shininess;

    Material(unsigned int diffuse = 0, unsigned int specular = 0, unsigned int emission = 0, float shininess = 0.0f);
};

#endif