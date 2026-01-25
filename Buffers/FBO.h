#ifndef FBO_CLASS_H
#define FBO_CLASS_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

class FBO {
public:
    GLuint ID;
    FBO();

    void Bind();
    void Unbind();
    void Delete();
};

#endif 