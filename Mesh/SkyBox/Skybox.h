#ifndef SKYBOX_H
#define SKYBOX_H

#include<filesystem>
namespace fs = std::filesystem;

#include <Camera/Camera.h>
#include <std_img/stb_image.h>
#include <glm/glm/glm.hpp>
#include <VAO.h>
#include <EBO.h>

class Skybox {
public:
	void Draw(Shader& shader, Camera& camera);
	Skybox(std::vector<Vertex>& skyboxVertices, std::vector<GLuint>& skyboxIndices, const fs::path faceCubemap[]);
private:
	unsigned int skyboxTexture;
	VAO skyboxVAO;
};

#endif