#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



#include <Scene.h>
#include <FBO.h>

#include <ctime>
#define NOMINMAX
#include <windows.h>

#define W_HEIGHT 1080
#define W_WIDTH  1920

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


GLfloat lightVertices[] =
{ //     COORDINATES     //
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
};

std::string getFilePath(const char* );
const char* filter = "GLTF Files\0*.gltf\0All Files\0*.*\0\0";

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

    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //Imgui
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    //End imgui


    Shader shader("shader_files/vertex.vs","shader_files/fragment.fs", "shader_files/geometry.gs");
    Shader outlining_shader("shader_files/outliner_vertex.vs","shader_files/outliner_fragment.fs");
    Shader skybox_shader("shader_files/skybox.vs", "shader_files/skybox.fs");
    Shader framebuffer_shader("shader_files/framebuffer.vs", "shader_files/framebuffer.fs");

    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 20.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 1.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(4.0f));
    
    shader.use();
    shader.setVec4("lightColor", lightColor);
    shader.setVec3("lightPos", lightPos);
    
    skybox_shader.use();
    skybox_shader.setInt("skyboxTexture", 0);
    framebuffer_shader.use();
    framebuffer_shader.setInt("screenTexture", 0);

    fs::path parentDir = fs::current_path();
    fs::path modelPath = parentDir / "Resources" / "models" / "crow" / "scene.gltf";
    fs::path modelPath1 = parentDir / "Resources" / "models" / "sword" / "scene.gltf";

    std::vector<Model> models;
    models.push_back(Model(modelPath.string().c_str(), "Crow"));
    models.push_back(Model(modelPath1.string().c_str(), "Sword"));

    fs::path facesCubemap[6] =
    {
        parentDir / "Resources" / "models" / "skybox" / "right.png",
        parentDir / "Resources" / "models" / "skybox" / "left.png",
        parentDir / "Resources" / "models" / "skybox" / "top.png",
        parentDir / "Resources" / "models" / "skybox" / "bottom.png",
        parentDir / "Resources" / "models" / "skybox" / "front.png",
        parentDir / "Resources" / "models" / "skybox" / "back.png"
    };

    Scene scene1(models, facesCubemap);

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
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT,0,GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

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
    
    postProcessingFBO.Unbind();
    // Error checking framebuffer
    fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;


    //light
    VAO lightVAO;
    lightVAO.Bind();
    VBO lightVBO(lightVertices, sizeof(lightIndices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //main loop 
    while(!glfwWindowShouldClose(window)){
        
        //take care of all glfw events
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        int display_w, display_h;
        glBindFramebuffer(GL_FRAMEBUFFER, rectFBO.ID);
        glViewport(0, 0, W_WIDTH, W_HEIGHT);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Draw your model
        scene1.Draw(shader,outlining_shader, skybox_shader, camera);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, rectFBO.ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO.ID);
        glBlitFramebuffer(
            0, 0, W_WIDTH, W_HEIGHT,
            0, 0, W_WIDTH, W_HEIGHT,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        //Outliner
        {
            ImGui::Begin("Outliner");
            unsigned int n = 1;
            for (auto& obj : scene1.GetObjects()) {
                std::string temp_name = std::to_string(n) + " " + obj.Name;
                if (ImGui::Button(temp_name.c_str()))
                {
                    if (scene1.selected_objID == obj.Name)
                        scene1.selected_objID = "";
                    else {
                        scene1.selected_objID = obj.Name;
                    }
                }
                n++;
            }

            std::string temp_name = std::to_string(n) + " " + scene1.GetSkybox().Name;
            ImGui::Button(temp_name.c_str());

            ImGui::End();
        }
        //Tools
        {
            ImGui::Begin("Tools");
            if (ImGui::Button("Add object")) {
                std::string path = getFilePath(filter);
                scene1.AddObject(path, std::filesystem::path(path).filename().string());
            }
            if (ImGui::Button("Add cube")) {
                fs::path cubePath = parentDir / "Resources" / "models" / "cube" / "scene.gltf";
                scene1.AddObject(cubePath.string(), "cube");
            }
            if (ImGui::Button("Add sphere")) {
                fs::path spherePath = parentDir / "Resources" / "models" / "sphere" / "scene.gltf";
                scene1.AddObject(spherePath.string(), "sphere");
            }
            ImGui::End();
        }

        //Properties
        {
            ImGui::Begin("Properties");
            static float f = 10;
            ImGui::Text("Selected objects");
            ImGui::Text(scene1.selected_objID.c_str());

            ImVec2 size = ImGui::GetWindowSize();
            
            if (scene1.selected_objID != "" && scene1.GetObjectByID(scene1.selected_objID))
            {
                Model* obj = scene1.GetObjectByID(scene1.selected_objID);
                static char buf[255]{};
                if (ImGui::InputText("Name", buf, ImGuiInputTextFlags_EnterReturnsTrue));
                if (ImGui::IsItemDeactivatedAfterEdit()) 
                {
                    scene1.SetObjectName(buf);
                    memset(buf, 0, sizeof(buf)); 
                }
                
                ImGui::DragFloat3("Pos", (float*) & obj->translation);
                ImGui::DragFloat3("Rotation", (float*) & obj->rotation);
                ImGui::DragFloat3("Scale", (float*) & obj->scale, 0.01f, 0.01f);
                obj->scale.x = std::max(obj->scale.x, 0.01f);
                obj->scale.y = std::max(obj->scale.y, 0.01f);
                obj->scale.z = std::max(obj->scale.z, 0.01f);

                if (ImGui::Button("Delete")) {
                    scene1.DeleteObject(scene1.selected_objID);
                }
            }

            ImGui::End();
        }

        //View port
        {
            ImGui::Begin("Viewport");
            ImVec2 size = ImGui::GetContentRegionAvail();
            
            camera.updateMatrix(90.0f, 0.1f, 500.f, glm::vec2(size.x, size.y));
            if (ImGui::IsWindowFocused()) {
                camera.Inputs(window, glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));
            }

            ImGui::Image(
                (void*)(intptr_t)postProcessingTexture,
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    shader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

std::string getFilePath(const char* filter = "GLTF Files\0*.gltf\0All Files\0*.*\0\0") {
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(filePath);
    }
    return "";
}