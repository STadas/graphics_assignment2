ADD_LIBRARY(wrapper_glfw STATIC "wrapper_glfw.cpp")
TARGET_INCLUDE_DIRECTORIES(wrapper_glfw INTERFACE ".")

ADD_LIBRARY(utils STATIC "utils.cpp")
ADD_LIBRARY(camera STATIC "camera.cpp")
ADD_LIBRARY(light STATIC "light.cpp")
ADD_LIBRARY(shader STATIC "shader.cpp")
ADD_LIBRARY(mesh STATIC "mesh.cpp")
ADD_LIBRARY(model STATIC "model.cpp")

# TARGET_LINK_LIBRARIES(model assimp)

TARGET_LINK_LIBRARIES("${PROJECT_NAME}"
    wrapper_glfw
    utils
    camera
    light
    shader
    mesh
    model
)
