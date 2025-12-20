#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include <Mesh.h>

using json = nlohmann::json;

class Model{

public:
    Model(const char* file);

    void Draw(Shader& shader_programm, Camera& camera);

};

#endif