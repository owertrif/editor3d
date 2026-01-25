#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <Model.h>
#include <Skybox.h>

#include <ctime>

#define W_HEIGHT 768
#define W_WIDTH  1360

std::vector<Vertex> skyboxVertices =
{
    //   Coordinates
    Vertex(glm::vec3(-1.0f, -1.0f, 1.0f)) ,//        7--------6
    Vertex(glm::vec3(1.0f, -1.0f, 1.0f))  ,//       /|       /|
    Vertex(glm::vec3(1.0f, -1.0f, -1.0f)) ,//      4--------5 |
    Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),//      | |      | |
    Vertex(glm::vec3(-1.0f,  1.0f,  1.0f)),//      | 3------|-2
    Vertex(glm::vec3(1.0f,  1.0f,  1.0f)) ,//      |/       |/
    Vertex(glm::vec3(1.0f,  1.0f, -1.0f)) ,//      0--------1
    Vertex(glm::vec3(-1.0f,  1.0f, -1.0f))
};

std::vector<GLuint> skyboxIndices =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};

float randf()
{
    return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main(){
    //GLFW window init begin
    glfwInit();
    
    //Telling that we are using opengl 3.3 (opengl (Major).(minor))
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Telling that we are using core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creating window
    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "3d editor or pehapse 3d viewer", NULL, NULL);

    //checking if the window is created
    if(window == NULL){
        std::cout<<"ERROR::WINDOW::NOT::CREATED"<<std::endl;
        return -1;
    }

    //Adjjusting context to the created window
    glfwMakeContextCurrent(window);

    //Loading glad so it is configure opengl
    gladLoadGL();

    //specifies the viewport of Opengl in the Window
    glViewport(0,0,W_WIDTH,W_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    //For faceculling (optimization)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //For outlining
    //glEnable(GL_STENCIL_TEST);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);



    Shader shader("shader_files/vertex.vs","shader_files/fragment.fs", "shader_files/geometry.gs");
    Shader skybox_shader("shader_files/skybox.vs", "shader_files/skybox.fs");
    Shader asteroid_shader("shader_files/asteroid.vs", "shader_files/fragment.fs");

    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 20.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    
    shader.use();
    shader.setVec4("lightColor", lightColor);
    shader.setVec3("lightPos", lightPos);
    skybox_shader.use();
    skybox_shader.setInt("skyboxTexture", 0);
    asteroid_shader.use();
    asteroid_shader.setVec4("lightColor", lightColor);
    asteroid_shader.setVec3("lightPos", lightPos);

    fs::path parentDir = fs::current_path();
    fs::path modelPath_jupiter = parentDir / "Resources" / "models" / "jupiter" / "scene.gltf";
    fs::path modelPath_asteroid = parentDir / "Resources" / "models" / "asteroid" / "scene.gltf";

    Model model_jupiter(modelPath_jupiter.string().c_str());

    fs::path facesCubemap[6] =
    {
        parentDir / "Resources" / "models" / "skybox" / "right.png",
        parentDir / "Resources" / "models" / "skybox" / "left.png",
        parentDir / "Resources" / "models" / "skybox" / "top.png",
        parentDir / "Resources" / "models" / "skybox" / "bottom.png",
        parentDir / "Resources" / "models" / "skybox" / "front.png",
        parentDir / "Resources" / "models" / "skybox" / "back.png"
    };

    Skybox space(skyboxVertices, skyboxIndices, facesCubemap);

    double prevTime = 0.0f;
    double lastframe = 0.0f;
    double currTime = 0.0f;
    double timeDiff;
    double deltaTime;
    unsigned int counter = 0;
    double FPS = 0.0f;

    //Vsync off/on
    glfwSwapInterval(0);

    const unsigned int number = 25000;
    // Radius of circle around which asteroids orbit
    float radius = 100.0f;
    // How much ateroids deviate from the radius
    float radiusDeviation = 25.0f;

    std::vector<glm::mat4> instanceMatrix;
    std::vector<float> distance;
    for (unsigned int i = 0; i < number; i++) {
        float x = randf();
        float finalRadius = radius + randf() * radiusDeviation;
        float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

        glm::vec3 tempTranslation;
        glm::quat tempRotation;
        glm::vec3 tempScale;
        if (randf() > 0.5f) {
            tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
        }
        else {
            tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
        }

        tempRotation = glm::quat(1.0f, randf(), randf(), randf());
        // Generates random scales
        tempScale = 0.1f * glm::vec3(randf(), randf(), randf());

        glm::mat4 trans = glm::mat4(1.0f);
        glm::mat4 rot = glm::mat4(1.0f);
        glm::mat4 sca = glm::mat4(1.0f);

        trans = glm::translate(trans, tempTranslation);
        rot = glm::mat4_cast(tempRotation);
        sca = glm::scale(sca, tempScale);

        instanceMatrix.push_back(trans * rot * sca);
    }


    Model model_asteroid(modelPath_asteroid.string().c_str(), number, instanceMatrix);

    //main loop 
    while(!glfwWindowShouldClose(window)){
        //take care of all glfw events
        glfwPollEvents();

        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
        deltaTime = currTime - lastframe;
        lastframe = currTime;
        counter++;

        if (timeDiff >= 1.0f / 165.f) {
            FPS = (1.0 / timeDiff) * counter;
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle = "3d editor or pehapse 3d viewer - " + std::to_string(FPS) + " FPS / " + ms + " ms ";
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = currTime;
            counter = 0;
            camera.Inputs(window);
        }


        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        camera.updateMatrix(90.0f, 0.1f, 500.f);

        model_jupiter.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f * deltaTime);
        model_jupiter.Draw(shader, camera);
        
        model_asteroid.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 2.5f * 10.0f * deltaTime);
        model_asteroid.Draw(asteroid_shader, camera);

        glDepthFunc(GL_LEQUAL);
        space.Draw(skybox_shader, camera);
        glDepthFunc(GL_LESS);

        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}