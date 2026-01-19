#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;


out vec3 currentPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoords;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main(){
    currentPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));

    Normal = aNormal;

    Color = aColor;
    
    TexCoords = mat2(0.0, -1.0, 1.0, 0.0) * aTexCoords;

    gl_Position = cameraMatrix * vec4(currentPos,1.0);    

}