/** shader.h
 * shader struct for easy shader handling
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Getting-started/Shaders
 */
#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "utils.h"

struct Shader
{
    GLuint id;

    Shader(const char *vertexPath, const char *fragmentPath);
    void use();
    /* uniform setters */
    void setBool(const std::string &name, GLboolean value) const;
    void setInt(const std::string &name, GLint value) const;
    void setUInt(const std::string &name, GLuint value) const;
    void setFloat(const std::string &name, GLfloat value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec4(const std::string &name, glm::vec4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;

private:
    GLuint compileShader(GLenum shaderType, const std::string &shaderCode);
};
