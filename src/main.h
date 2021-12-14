/** main.h
 * main file
 * Tadas Saltenis November 2021
 */
#pragma once

/* #define GLM_ENABLE_EXPERIMENTAL */
#define GLEW_STATIC
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/* #include <glm/gtx/string_cast.hpp> */
#include <iostream>
#include <stack>
#include <string>
#include <map>

#include "common/camera.h"
#include "common/light.h"
#include "common/model.h"
#include "common/wrapper_glfw.h"

void init(GLWrapper *glw);
void render(Shader *overrideShader = nullptr);
void reshape(GLFWwindow *window, int w, int h);
void keyCallback(GLFWwindow *window, int key, int action, int mods);

void drawStatic(std::stack<glm::mat4> &model, Shader *overrideShader);
void drawMainLightSphere(std::stack<glm::mat4> &model);

int main();
