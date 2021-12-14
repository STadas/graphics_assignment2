/** shader.h
 * shader struct for easy shader handling
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Getting-started/Shaders
 */
#include "shader.h"

/**
 * constructor
 * retrieve, compile and link shaders
 */
Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
    /* retrieve the vertex/fragment source code from file paths */
    std::string vShaderCode = readFile(vertexPath);
    std::string fShaderCode = readFile(fragmentPath);
    std::string gShaderCode;
    if (geometryPath != nullptr)
        gShaderCode = readFile(geometryPath);

    /* compile shaders */
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
    GLuint geometry;
    if (geometryPath != nullptr)
        geometry = compileShader(GL_GEOMETRY_SHADER, gShaderCode);

    /* link shaders */
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if (geometryPath != nullptr)
        glAttachShader(id, geometry);
    glLinkProgram(id);

    /* print linking errors if any */
    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *infoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(id, infoLogLength, NULL, infoLog);

        glGetProgramInfoLog(id, infoLogLength, NULL, infoLog);
        std::cerr << "Error linking shaders" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }

    /* delete the shaders as they're already linked into our program */
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

/**
 * start using the current shader
 */
void Shader::use()
{
    glUseProgram(id);
}

/**
 * set bool (int) uniform in the shader
 */
void Shader::setBool(const std::string &name, GLboolean value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * set unsigned int uniform in the shader
 */
void Shader::setInt(const std::string &name, GLint value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * set unsigned int uniform in the shader
 */
void Shader::setUInt(const std::string &name, GLuint value) const
{
    glUniform1ui(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * set float uniform in the shader
 */
void Shader::setFloat(const std::string &name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * set vec3 uniform in the shader
 */
void Shader::setVec3(const std::string &name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

/**
 * set vec4 uniform in the shader
 */
void Shader::setVec4(const std::string &name, glm::vec4 value) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

/**
 * set mat3 uniform in the shader
 */
void Shader::setMat3(const std::string &name, glm::mat3 value) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/**
 * set mat4 uniform in the shader
 */
void Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/**
 * compile shader, this is used in the constructor
 */
GLuint Shader::compileShader(GLenum shaderType, const std::string &shaderCode)
{
    GLuint shader = glCreateShader(shaderType);
    const char *cStrShaderCode = shaderCode.c_str();
    glShaderSource(shader, 1, &cStrShaderCode, NULL);
    glCompileShader(shader);

    /* print compile errors if any */
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog = new GLchar[infoLogLength + 1];

        const char *strShaderType = NULL;
        switch (shaderType)
        {
            case GL_VERTEX_SHADER:
                strShaderType = "vertex";
                break;
            case GL_GEOMETRY_SHADER:
                strShaderType = "geometry";
                break;
            case GL_FRAGMENT_SHADER:
                strShaderType = "fragment";
                break;
        }

        glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
        std::cerr << "Compile error in " << strShaderType << " shader" << std::endl
                  << infoLog << std::endl;
        delete[] infoLog;
    }

    return shader;
}
