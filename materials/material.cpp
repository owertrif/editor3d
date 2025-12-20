#include "material.h"


Material::Material(unsigned int diffuse, unsigned int specular,unsigned int emission, float shininess){
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
    this->emission = emission;
}