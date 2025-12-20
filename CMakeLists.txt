cmake_minimum_required(VERSION 3.10)
project(editor3d)

set(CMAKE_CXX_STANDART 20)


#---Sources---

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
    ${CMAKE_SOURCE_DIR}/json
    ${CMAKE_SOURCE_DIR}/Model
    
)

include_directories(${ASSIMP_INCLUDE_DIR})
link_directories(${ASSIMP_LIB_DIR})

#---SYS PACKAGES---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

#---EXECUTABLE---
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
add_executable(${PROJECT_NAME} ${SOURCES})

#---link libs---
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
    dl
)

function(copy_shader SHADER_FILE)
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${SHADER_FILE}
            ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}
        DEPENDS ${CMAKE_SOURCE_DIR}/${SHADER_FILE}
    )
    list(APPEND SHADER_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME})
    set(SHADER_OUTPUTS "${SHADER_OUTPUTS}" PARENT_SCOPE)
endfunction()

copy_shader(vertex.vs)
copy_shader(fragment.fs)
copy_shader(lightvertex.vs)
copy_shader(lightfragment.fs)

add_custom_target(copy_shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(editor3d copy_shaders)
