#ifndef MESH_CLASS_H
#define MESH_CALSS_H

#include <string>
#include <vector>

#include <Buffers/VAO.h>
#include <Buffers/EBO.h>
#include <Camera/Camera.h>
#include <Textures/texture.h>

class Mesh{

public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    VAO mesh_VAO;

    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,std::vector<Texture>& textures);

    void Draw(Shader& shader_programm, Camera& camera);
};

#endif