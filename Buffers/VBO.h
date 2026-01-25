#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

struct Vertex{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

class VBO{
public:
    GLuint ID;
    VBO(std::vector<Vertex>& vertices);
    VBO(std::vector<glm::mat4>& mat4s);

    void Bind();
    void Unbind();
    void Delete();
};

#endif 