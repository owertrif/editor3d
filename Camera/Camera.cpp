#include <Camera.h>

Camera::Camera(int width, int height, glm::vec3 position){
    this->width = width;
    this->height = height;
    Position = position;
}



void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane){
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

    view = glm::lookAt(Position, Position + Orientation, Up);
    projection = glm::perspective(glm::radians(FOVdeg),static_cast<float>(width)/static_cast<float>(height),nearPlane, farPlane);

    cameraMatrix = projection * view;
}

void Camera::Inputs(GLFWwindow* window){
    //Keyboard inputs
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        Position += speed * Orientation;
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
        Position -= speed * Orientation;
    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
        Position -= speed * glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Position += speed * Up;
    if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        Position -= speed * Up;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window,GLFW_TRUE);

    //Mouse inputs
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if(firstClick){
            glfwSetCursorPos(window, (width/2), (height/2));
            firstClick = false;
        }

        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotx = sensativity * (float)(mouseY - (height/2))/height;
        float roty = sensativity * (float)(mouseX - (width/2))/width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotx), glm::normalize(glm::cross(Orientation, Up)));
        if(!glm::angle(newOrientation, Up) <= glm::radians(5.0f) || glm::angle(newOrientation, -Up) <= glm::radians(5.0f))
            Orientation = newOrientation;
        
        Orientation = glm::rotate(Orientation, glm::radians(-roty), Up);

        glfwSetCursorPos(window, (width/2), (height/2));
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}

void Camera::Matrix(Shader& shader, const char* uniform){
    shader.setMat4(uniform, cameraMatrix);
}