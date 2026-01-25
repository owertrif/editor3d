#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <Model.h>
#include <Skybox.h>
#include <FBO.h>

#include <ctime>

#define W_HEIGHT 768
#define W_WIDTH  1360

#define SAMPLES 8

std::vector<float> rectangleVertices=
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
    //glfwWindowHint(GLFW_SAMPLES, SAMPLES);
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
    
    glEnable(GL_MULTISAMPLE);
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
    Shader framebuffer_shader("shader_files/framebuffer.vs", "shader_files/framebuffer.fs");

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
    framebuffer_shader.use();
    framebuffer_shader.setInt("screenTexture", 0);

    fs::path parentDir = fs::current_path();
    fs::path modelPath = parentDir / "Resources" / "models" / "crow" / "scene.gltf";

    Model model(modelPath.string().c_str());

    fs::path facesCubemap[6] =
    {
        parentDir / "Resources" / "models" / "skybox" / "right.png",
        parentDir / "Resources" / "models" / "skybox" / "left.png",
        parentDir / "Resources" / "models" / "skybox" / "top.png",
        parentDir / "Resources" / "models" / "skybox" / "bottom.png",
        parentDir / "Resources" / "models" / "skybox" / "front.png",
        parentDir / "Resources" / "models" / "skybox" / "back.png"
    };

    Skybox skyBox(facesCubemap);

    double prevTime = 0.0f;
    double lastframe = 0.0f;
    double currTime = 0.0f;
    double timeDiff;
    double deltaTime;
    unsigned int counter = 0;
    double FPS = 0.0f;

    //Vsync off/on
    glfwSwapInterval(0);
    
    VAO rectVAO;
    rectVAO.Bind();
    VBO rectVBO(rectangleVertices);
    rectVBO.Bind();
    rectVAO.LinkAttrib(rectVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    rectVAO.LinkAttrib(rectVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2*sizeof(float)));
    rectVAO.Unbind();
    rectVBO.Unbind();

    FBO rectFBO;

    unsigned int framebufferTexture;
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLES, GL_RGB, W_WIDTH, W_HEIGHT, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLES, GL_DEPTH24_STENCIL8, W_WIDTH, W_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // Error checking framebuffer
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer error: " << fboStatus << std::endl;

    // Create Frame Buffer Object
    FBO postProcessingFBO;

    // Create Framebuffer Texture
    unsigned int postProcessingTexture;
    glGenTextures(1, &postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

    // Error checking framebuffer
    fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;

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

        rectFBO.Bind();
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        camera.updateMatrix(90.0f, 0.1f, 500.f);

        model.Draw(shader, camera);

        glDepthFunc(GL_LEQUAL);
        skyBox.Draw(skybox_shader, camera);
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, rectFBO.ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO.ID);
        glBlitFramebuffer(0, 0, W_WIDTH, W_HEIGHT, 0, 0, W_WIDTH, W_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        rectFBO.Unbind();
        
        framebuffer_shader.use();
        rectVAO.Bind();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}