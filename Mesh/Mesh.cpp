#include <Mesh.h>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures, unsigned int instancing, std::vector<glm::mat4> instanceMatrix){
    this->textures = textures;
    this->indices = indices;
    this->vertices = vertices;
    this->instancing = instancing;

    
    mesh_VAO.Bind();
    
    VBO instanceVBO(instanceMatrix);
    VBO VBO(vertices);
    EBO EBO(indices);

    mesh_VAO.LinkAttrib(VBO,0,3,GL_FLOAT,sizeof(Vertex),(void*)0);
    mesh_VAO.LinkAttrib(VBO,1,3,GL_FLOAT,sizeof(Vertex),(void*)(3*sizeof(float)));
    mesh_VAO.LinkAttrib(VBO,2,3,GL_FLOAT,sizeof(Vertex),(void*)(6*sizeof(float)));
    mesh_VAO.LinkAttrib(VBO,3,2,GL_FLOAT,sizeof(Vertex),(void*)(9*sizeof(float)));
    if (instancing != 1) {
        instanceVBO.Bind();
        mesh_VAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
        mesh_VAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        mesh_VAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        mesh_VAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
    }

   mesh_VAO.Unbind();
    VBO.Unbind();
    instanceVBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(
    Shader& shader_programm, 
    Camera& camera,
    glm::mat4 matrix,
    glm::vec3 translation,
    glm::quat rotation,
    glm::vec3 scale
    ){
    shader_programm.use();
    mesh_VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for(unsigned int i = 0; i< textures.size(); i++){
        std::string num;
        std::string type = textures[i].type;

        if(type == "diffuse"){
            num = std::to_string(numDiffuse++);
        }
        else if(type == "specular"){
            num = std::to_string(numSpecular++);
        }
        textures[i].textUnit(shader_programm,(type+num).c_str(), i);
        textures[i].Bind();
    }
    shader_programm.setVec3("cameraPos", camera.Position);
    camera.Matrix(shader_programm, "cameraMatrix");
    if (instancing == 1)
    {
        glm::mat4 trans = glm::mat4(1.0f);
        glm::mat4 rot = glm::mat4(1.0f);
        glm::mat4 sca = glm::mat4(1.0f);

        trans = glm::translate(trans, translation);
        rot = glm::mat4_cast(rotation);
        sca = glm::scale(sca, scale);

        shader_programm.setMat4("translation", trans);
        shader_programm.setMat4("rotation", rot);
        shader_programm.setMat4("scale", sca);
        shader_programm.setMat4("model", matrix);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glm::mat4 trans = glm::mat4(1.0f);
        glm::mat4 rot = glm::mat4(1.0f);
        glm::mat4 sca = glm::mat4(1.0f);

        trans = glm::translate(trans, translation);
        rot = glm::mat4_cast(rotation);
        sca = glm::scale(sca, scale);

        shader_programm.setMat4("translation", trans);
        shader_programm.setMat4("rotation", rot);
        shader_programm.setMat4("scale", sca);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
    }
}