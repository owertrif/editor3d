#ifndef SCENE_H
#define SCENE_H

#include <Model.h>
#include <Skybox.h>

class Scene {
public:
	void Draw(Shader& shader, Shader& outlining_shader, Shader& skybox_shader, Camera& camera);
	std::vector<Model> GetObjects();
	Skybox GetSkybox();
	Model* GetObjectByID(std::string ID);
	void DeleteObject(std::string ID);
	Scene(std::vector<Model> models, const fs::path facesCubemap[]);
	std::string selected_objID;
	~Scene();
private:
	std::vector<Model> models;
	Skybox* skybox;
};

#endif