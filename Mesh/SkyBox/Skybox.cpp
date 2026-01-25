#include "Skybox.h"

void Skybox::Draw(Shader& shader, Camera& camera)
{
    shader.use();
    skyboxVAO.Bind();
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::mat4(glm::mat3(camera.view));
    shader.setMat4("view", view);
    shader.setMat4("projection", camera.projection);
    shader.setInt("skyboxTexture", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

Skybox::Skybox(std::vector<Vertex>& skyboxVertices, std::vector<GLuint>& skyboxIndices, const fs::path facesCubemap[])
{
    skyboxVAO.Bind();
    VBO skyboxVBO(skyboxVertices);
    EBO skyboxEBO(skyboxIndices);

    skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    skyboxVAO.Unbind();
    skyboxVBO.Unbind();
    skyboxEBO.Unbind();

    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].string().c_str(), &height, &width, &nrChannels, 0);
        if (data) {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }

    }
}
