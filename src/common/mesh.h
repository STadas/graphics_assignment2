/** mesh.h
 * vertex, material, mesh structs for easy mesh handling
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Model-Loading/Mesh
 */
#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string texPath;
};

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    GLfloat shininess;
    GLfloat opacity;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    Material material;

    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
         std::vector<Texture> textures, Material material);

    void draw(Shader *shader);

private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};
