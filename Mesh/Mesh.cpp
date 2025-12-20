#include <Mesh.h>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures){
    this->textures = textures;
    this->indices = indices;
    this->vertices = vertices;
    
    mesh_VAO.Bind();
    
    VBO VBO(vertices);
    EBO EBO(indices);

    mesh_VAO.LinkAttrib(VBO,0,3,GL_FLOAT,sizeof(Vertex),(void*)0);
    mesh_VAO.LinkAttrib(VBO,1,3,GL_FLOAT,sizeof(Vertex),(void*)(3*sizeof(float)));
    mesh_VAO.LinkAttrib(VBO,2,3,GL_FLOAT,sizeof(Vertex),(void*)(6*sizeof(float)));
    mesh_VAO.LinkAttrib(VBO,3,2,GL_FLOAT,sizeof(Vertex),(void*)(9*sizeof(float)));

   mesh_VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(Shader& shader_programm, Camera& camera){
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
    camera.Matrix(shader_programm,"cameraMatrix");

    glDrawElements(GL_TRIANGLES, indices.size(),GL_UNSIGNED_INT, 0);
}