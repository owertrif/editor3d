#include<filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad/glad.h"
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <std_img/stb_image.h>

#include <Mesh.h>

#include <ctime>

#define W_HEIGHT 720
#define W_WIDTH  1280

const float W_COEF = float(W_HEIGHT)/float(W_WIDTH);

Vertex vertices[] = { //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

GLuint indices[] = {
	0, 1, 2, // Bottom side
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
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

glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
glm::vec3 lightPos{0.0f, 0.35f, 0.0f};

struct objProps{
    glm::vec3 pos;
    float radius;
    float speed_scale;
    float size_scale;
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



    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();

    Texture textures[]{
        Texture((parentDir + "/planks.png").c_str(),"diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture((parentDir + "/planksSpec.png").c_str(),"specular", 1, GL_RED, GL_UNSIGNED_BYTE)
    };

    //specifies the viewport of Opengl in the Window
    glViewport(0,0,W_WIDTH,W_HEIGHT);

    glEnable(GL_DEPTH_TEST);

        std::cout<<"main shader\n";
    Shader shader("vertex.vs","fragment.fs");   

    std::vector<Vertex> verts(vertices, vertices + sizeof(vertices)/sizeof(Vertex));
    std::vector<GLuint> inds(indices, indices + sizeof(indices)/sizeof(GLuint));
    std::vector<Texture> texs(textures, textures + sizeof(textures)/sizeof(Texture));
    
    Mesh floor(verts, inds, texs);

        std::cout<<"light shader\n";
    Shader lightShader("lightvertex.vs", "lightfragment.fs");
    
    std::vector<Vertex> vertsLight(lightVertices, lightVertices + sizeof(lightVertices)/sizeof(Vertex));
    std::vector<GLuint> indsLight(lightIndices, lightIndices + sizeof(lightIndices)/sizeof(GLuint));

    Mesh light(vertsLight, indsLight, texs);
    
    Camera camera(W_WIDTH, W_HEIGHT, glm::vec3(0.0f, 2.0f, 2.0f));

        std::cout<<"Starting main loop\n";
    //main loop 
    while(!glfwWindowShouldClose(window)){
        //take care of all glfw events
        glfwPollEvents();

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.f);
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f,0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f/W_COEF, 1.0f));
        shader.setMat4("model", model);
        shader.setVec3("lightPos", lightPos);
        shader.setVec4("lightColor", lightColor);

        floor.Draw(shader,camera);

        lightShader.use();
        glm::mat4 lightmodel = glm::mat4(1.0f);
        lightmodel = glm::translate(lightmodel, lightPos);
        lightmodel = glm::scale(lightmodel, glm::vec3(1.0f, 1.0f/W_COEF, 1.0f));
        lightShader.setMat4("model", lightmodel);
        lightShader.setVec4("lightColor", lightColor);

        light.Draw(lightShader, camera);
        //swap back buffer with the front buffer
        glfwSwapBuffers(window);
    }

    shader.~Shader();
    lightShader.~Shader();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

