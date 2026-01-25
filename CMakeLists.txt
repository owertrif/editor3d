cmake_minimum_required(VERSION 3.10)
project(editor3d)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---Sources---


set(GLFW_ROOT "C:/vcpkg/installed/x64-windows/share/glfw3")
set(SOURCES
	main.cpp
	glad/glad.c
    shader/shader.cpp
    materials/material.cpp
    Buffers/VBO.cpp
    Buffers/EBO.cpp
    Buffers/VAO.cpp
    std_img/stb_image_implementation.cpp
    Textures/texture.cpp
    Camera/Camera.cpp
    Mesh/Mesh.cpp
    Mesh/SkyBox/Skybox.cpp
    Model/Model.cpp
)

#---Gen root dir---

configure_file(
	${CMAKE_SOURCE_DIR}/root_directory.h.in
	${CMAKE_BINARY_DIR}/root_directory.h
)

#---Include directories---

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/glad
	${CMAKE_SOURCE_DIR}/glm
    ${CMAKE_SOURCE_DIR}/shader
    ${CMAKE_SOURCE_DIR}/materials
    ${CMAKE_SOURCE_DIR}/Buffers
    ${CMAKE_SOURCE_DIR}/std_img
    ${CMAKE_SOURCE_DIR}/Textures
    ${CMAKE_SOURCE_DIR}/Camera
    ${CMAKE_SOURCE_DIR}/Mesh
    ${CMAKE_SOURCE_DIR}/Mesh/SkyBox
    ${CMAKE_SOURCE_DIR}/json
    ${CMAKE_SOURCE_DIR}/Model
    
)

find_package(OpenGl REQUIRED)
find_package(glfw3 REQUIRED PATHS ${GLFW_ROOT} NO_DEFAULT_PATH)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

#---EXECUTABLE---
add_executable(${PROJECT_NAME} ${SOURCES} "Mesh/SkyBox/Skybox.h")

#---link libs---
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glm::glm
    assimp::assimp
    ${OPENGL_LIBRARIES}
)

file(COPY
    ${CMAKE_SOURCE_DIR}/shader_files
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/vertex.vs
    ${CMAKE_CURRENT_BINARY_DIR}shader_files/vertex.vs
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/skybox.vs
    ${CMAKE_CURRENT_BINARY_DIR}shader_files/skybox.vs
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/fragment.fs
    ${CMAKE_CURRENT_BINARY_DIR}/shader_files/fragment.fs
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/skybox.fs
    ${CMAKE_CURRENT_BINARY_DIR}/shader_files/skybox.fs
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/normals.fs
    ${CMAKE_CURRENT_BINARY_DIR}/shader_files/normals.fs
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/geometry.gs
    ${CMAKE_CURRENT_BINARY_DIR}/shader_files/geometry.gs
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/shader_files/normals.gs
    ${CMAKE_CURRENT_BINARY_DIR}/shader_files/normals.gs
    COPYONLY
)

file(COPY
    ${CMAKE_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)


