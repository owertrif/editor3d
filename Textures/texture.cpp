#include <texture.h>

Texture::Texture(const char* image, const char* type, GLuint slot, GLenum format, GLenum pixelType){
    this->type = type;

    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    std::cout<<"loading "<<image<<std::endl;
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    if(bytes){
        std::cout<<"loaded"<<std::endl;

        glGenTextures(1,&ID);
        glActiveTexture(GL_TEXTURE0 + slot);
        unit = slot;
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(bytes);
    }
    else{
        std::cout<<"ERROR::LOADING::IMAGE"<<std::endl;
    }
    glBindTexture(GL_TEXTURE_2D,0);
}

void Texture::textUnit(Shader &shader, const char* uniform, GLuint unit){
    shader.setInt(uniform,unit);
}

void Texture::Bind(){
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D,ID);
}

void Texture::Unbind(){
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete(){
    glDeleteTextures(1, &ID);
}