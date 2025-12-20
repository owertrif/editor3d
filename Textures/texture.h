#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <std_img/stb_image.h>

#include <shader/shader.h>

class Texture{
public:
    GLuint ID;
    const char* type;
    GLuint unit;

    Texture(const char* image, const char* type, GLuint slot, GLenum format, GLenum pixelType);

    void textUnit(Shader &shader,const char* uniform,GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};

#endif