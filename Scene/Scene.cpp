#include<Scene.h>

Scene::Scene(std::vector<Model> models, const fs::path facesCubemap[]) {
	this->models = models;
	this->skybox = new  Skybox(facesCubemap);
	skybox->Name = "Default_skybox";
}

Scene::~Scene()
{
	delete skybox;
}

void Scene::Draw(Shader& shader, Shader& outlining_shader, Shader& skybox_shader, Camera& camera) {
	
	if (skybox != NULL) {
		glDepthFunc(GL_LEQUAL);
		skybox->Draw(skybox_shader, camera);
		glDepthFunc(GL_LESS);
	}

	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	for (auto& model : models) {
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		model.Draw(shader, camera);
	}

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	for (auto& model : models) {
		if (selected_objID == model.Name)
			model.DrawOutlining(outlining_shader, camera);
	}

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

std::vector<Model> Scene::GetObjects()
{
	return models;
}

Skybox Scene::GetSkybox()
{
	return *skybox;
}

Model* Scene::GetObjectByID(std::string ID)
{
	Model* obj = NULL;
	for (auto& m : models)
	{
		if (m.Name == ID)
			obj = &m;
	}
	return obj;
}

void Scene::AddObject(std::string path, std::string name)
{
	

	std::string temp_name = name;
	unsigned short num = 0;
	if (nameCount.count(name) > 0)
	{
			temp_name = name + std::to_string(nameCount[name]);
	}
	nameCount[name]++;

	models.push_back(Model(path.c_str(), temp_name));
}

void Scene::DeleteObject(std::string ID)
{
	models.erase(std::remove_if(models.begin(), models.end(), [&ID](const Model& obj) {return obj.Name == ID; }), models.end());
	if (selected_objID == ID)
		selected_objID = "";
}

void Scene::SetObjectName(std::string name) {
	if (nameCount.count(name) > 0) {
		GetObjectByID(selected_objID)->Name = name + std::to_string(nameCount[name]);
		selected_objID = name + std::to_string(nameCount[name]);
	}
	else
	{
		GetObjectByID(selected_objID)->Name = name;
		selected_objID = name;
	}
	
	nameCount[name]++;
	
}