#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main(){
	vec3 scaledPos = aPos * 1.01f;
	vec3 currentPos = vec3(model * translation * rotation * scale * vec4(scaledPos, 1.0f));
	gl_Position = cameraMatrix * vec4(currentPos,1.0);
}
