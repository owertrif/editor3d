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

//for framebuffer whatch
float rectangleVertices[] =
{
    // Coords    // texCoords
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
};

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



    Shader shader("shader_files/vertex.vs","shader_files/fragment.fs");
    Shader framebuffer_shader("shader_files/framebuffer.vs", "shader_files/framebuffer.fs");

    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 20.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    
    shader.use();
    shader.setVec4("lightColor", lightColor);
    shader.setVec3("lightPos", lightPos);

    framebuffer_shader.use();
    framebuffer_shader.setInt("screenTexture", 0);

    fs::path parentDir = fs::current_path();
    fs::path modelPath = parentDir / "Resources" / "models" / "crow" / "scene.gltf";

    Model model(modelPath.string().c_str());

    double prevTime = 0.0f;
    double currTime = 0.0f;
    double timeDiff;
    unsigned int counter = 0;
    double FPS = 0.0f;

    //Vsync off/on
   //glfwSwapInterval(0);

    unsigned int rectVAO, rectVBO;
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int framebufferTexture;
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W_WIDTH, W_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer incomplete: " << fboStatus << std::endl;
    
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

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        camera.updateMatrix(90.0f, 0.1f, 100.f);

        model.Draw(shader, camera);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        framebuffer_shader.use();
        glBindVertexArray(rectVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}