#version 330 core

out vec4 FragColor;

in vec3 currentPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

vec4 pointLight(){
    
    vec3 lightVector = lightPos - currentPos;
    float distance = length(lightVector);
    float a = 0.2f;
    float b = 0.01f;
    float intencity = 1.0f/(a*distance*distance + b * distance + 1.0f);
    
    float ambient = 0.20f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVector);
    float diffuse = max(dot(normal,lightDirection), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(cameraPos - currentPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmmount = pow(max(dot(viewDirection, reflectionDirection),0.0f), 16);
    float specular = specAmmount * specularLight;


    return (texture(diffuse0,TexCoords) * (diffuse * intencity + ambient) + texture(specular0,TexCoords).r * specular * intencity) * lightColor;
}

vec4 directLight(){

    float ambient = 0.20f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal,lightDirection), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(cameraPos - currentPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmmount = pow(max(dot(viewDirection, reflectionDirection),0.0f), 16);
    float specular = specAmmount * specularLight;


    return (texture(diffuse0,TexCoords) * (diffuse + ambient) + texture(specular0,TexCoords).r * specular) * lightColor;
}

vec4 spotLight(){

    //center of the light is 1 so further from center the less value is 
    // outerCone is to clamp hard
    float outerCone = 0.90f;
    //innerCone is to clamp softly
    float innerCone = 0.95f;

    //Making light vector (from target to light scource)
    vec3 lightVector = lightPos - currentPos;

    float ambient = 0.20f;

    //Normilizing normal )))
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVector);
    float diffuse = max(dot(normal,lightDirection), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(cameraPos - currentPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmmount = pow(max(dot(viewDirection, reflectionDirection),0.0f), 16);
    float specular = specAmmount * specularLight;

    // Calculating angle between object and light scource
    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float intencity = clamp((angle - outerCone) / (innerCone - outerCone),0.0f, 1.0f);

    return (texture(diffuse0,TexCoords) * (diffuse * intencity + ambient) + texture(specular0,TexCoords).r * specular * intencity) * lightColor;
}


float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth){
    return (2.0f * near * far) / (far + near - (depth * 2.0f - 1.0f) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f){
    float zVal = linearizeDepth(depth);
    return (1 / (1+ exp(-steepness * (zVal - offset))));
}

void main(){

    //FragColor = directLight();
    float depth = logisticDepth(gl_FragCoord.z);
    FragColor = directLight();
    
}