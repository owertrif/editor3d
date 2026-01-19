#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include <Mesh.h>

using json = nlohmann::json;

class Model{

public:
    Model(const char* file);

    void Draw(Shader& shader_programm,Camera& camera);

private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    std::vector<Mesh> meshes;
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationMeshes;
    std::vector<glm::vec3> scaleMashes;
    std::vector<glm::mat4> matricesMeshes;

    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    void loadMeshes(unsigned int indMeshes);
    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();



    std::vector<Vertex> assembleVertices(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
        );

    std::vector<glm::vec2> groupFloatVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatVec4(std::vector<float> floatVec);
};

#endif