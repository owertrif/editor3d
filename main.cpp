#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <std_img/stb_image.h>

#include <Model.h>

#include <ctime>

#define W_HEIGHT 720
#define W_WIDTH  1280

int main(){
    //GLFW window init begin
    glfwInit();
    
    //Telling that we are using opengl 3.3 (opengl (Major).(minor))
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Telling that we are using core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creating window
    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Fucking 3d editor or pehapse 3d viewer", NULL, NULL);

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
    glDepthFunc(GL_LESS);

    Shader shader("vertex.vs","fragment.fs");
    
    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 2.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    fs::path parentDir = fs::current_path();
    fs::path modelPath = parentDir / "Resources" / "models" / "trees" / "scene.gltf";

    Model trees(modelPath.string().c_str());

    fs::path modelPath2 = parentDir / "Resources" / "models" / "ground" / "scene.gltf";
    Model ground(modelPath2.string().c_str());

    //main loop 
    while(!glfwWindowShouldClose(window)){
        //take care of all glfw events
        glfwPollEvents();

        glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.f);
        shader.use();

        shader.setVec4("lightColor", lightColor);
        shader.setVec3("lightPos", lightPos);
        
        trees.Draw(shader, camera);
        ground.Draw(shader, camera);

        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

