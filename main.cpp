#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <std_img/stb_image.h>

#include <Model.h>

#include <ctime>

#define W_HEIGHT 768
#define W_WIDTH  1360

const unsigned int numWindows = 100;
unsigned int orderDraw[numWindows];
glm::vec3 positionsWindows[numWindows];
float rotationsWindows[numWindows];

float distanceCamera[numWindows];

int compare(const void* a, const void* b) {
    double diff = distanceCamera[*(int*)b] - distanceCamera[*(int*)a];
    return (0 < diff) - (diff < 0);
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //For outlining
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);



    Shader shader("shader_files/vertex.vs","shader_files/fragment.fs");
    Shader shader_grass("shader_files/vertex.vs","shader_files/grass.fs");
    Shader shader_windows("shader_files/vertex.vs","shader_files/windows.fs");
    //Shader outliner_shader("outliner_vertex.vs", "outliner_fragment.fs");

    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 20.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    
    shader.use();
    shader.setVec4("lightColor", lightColor);
    shader.setVec3("lightPos", lightPos);

    shader_grass.use();
    shader_grass.setVec4("lightColor", lightColor);
    shader_grass.setVec3("lightPos", lightPos);

    shader_windows.use();
    shader_windows.setVec4("lightColor", lightColor);
    shader_windows.setVec3("lightPos", lightPos);

    fs::path parentDir = fs::current_path();
    fs::path modelPath = parentDir / "Resources" / "models" / "ground" / "scene.gltf";
    fs::path modelPath2 = parentDir / "Resources" / "models" / "grass" / "scene.gltf";
    fs::path modelPath3 = parentDir / "Resources" / "models" / "windows" / "scene.gltf";

    Model model(modelPath.string().c_str());
    Model model2(modelPath2.string().c_str());
    Model model3(modelPath3.string().c_str());

    double prevTime = 0.0f;
    double currTime = 0.0f;
    double timeDiff;
    unsigned int counter = 0;
    double FPS = 0.0f;

    //Vsync off/on
    glfwSwapInterval(0);

    for (unsigned int i = 0; i < numWindows; i++) {
        positionsWindows[i] = glm::vec3(
            -15.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (15.f - (-15.f)))),
            1.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (4.f - 1.f))),
            -15.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (15.f - (-15.f))))
        );
        rotationsWindows[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.f));
        orderDraw[i] = i;
    }
    
    //main loop 
    while(!glfwWindowShouldClose(window)){
        //take care of all glfw events
        glfwPollEvents();

        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        
        camera.updateMatrix(90.0f, 0.1f, 100.f);

        model.Draw(shader, camera);

        glDisable(GL_CULL_FACE);
        model2.Draw(shader_grass, camera);
        glEnable(GL_BLEND);

        for (unsigned int i = 0; i < numWindows; i++) {
            distanceCamera[i] = glm::length(camera.Position - positionsWindows[i]);
        }

        qsort(orderDraw, numWindows, sizeof(unsigned int), compare);

        for (unsigned int i = 0; i < numWindows; i++) {
            model3.Draw(shader_windows, camera, positionsWindows[orderDraw[i]], glm::quat(1.0f, 0.0f, rotationsWindows[orderDraw[i]], 0.0f));
        }
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    shader_grass.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

