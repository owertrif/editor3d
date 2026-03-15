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

	void AddObject(std::string path, std::string name);
	void DeleteObject(std::string ID);
	void SetObjectName(std::string name);

	Scene(std::vector<Model> models, const fs::path facesCubemap[]);
	~Scene();

	std::string selected_objID;
private:
	std::vector<Model> models;
	Model* selectedObj;
	Skybox* skybox;
	std::unordered_map<std::string, unsigned short> nameCount;
};

#endif