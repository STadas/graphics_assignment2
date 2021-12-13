/** mesh.h
 * vertex, material, mesh structs for easy mesh handling
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Model-Loading/Mesh
 */
#include "mesh.h"

/**
 * construcor
 */
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
           std::vector<Texture> textures, Material material)
    : vertices(vertices)
    , indices(indices)
    , textures(textures)
    , material(material)
{
    setupMesh();
}

/**
 * set up the mesh (similar to legacy meshes' make())
 */
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],
                 GL_STATIC_DRAW);

    /* vertex positions */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    /* vertex normals */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    /* vertex texture coords */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, texCoords));
    /* vertex tangents */
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tangent));

    glBindVertexArray(0);
}

/**
 * draw the mesh (similar to legacy meshes' draw())
 */
void Mesh::draw(Shader *shader)
{
    GLuint diffuseNumber = 1;
    GLuint normalNumber = 1;
    for (int i = 0; i < textures.size(); i++)
    {
        /* activate proper texture unit before binding */
        glActiveTexture(GL_TEXTURE0 + i);

        /* retrieve texture number (the N in material.diffuse_textureN) */
        std::string number;
        std::string name = textures[i].type;
        /* std::cout << name << std::endl; */
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNumber++);
        else if (name == "texture_normal")
            number = std::to_string(normalNumber++);

        /* std::cout << "material." + name + number << std::endl; */
        shader->setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    shader->setVec3("material.diffuse", material.diffuse);
    shader->setVec3("material.specular", material.specular);
    shader->setVec3("material.ambient", material.ambient);
    shader->setFloat("material.shininess", material.shininess);
    shader->setFloat("material.opacity", material.opacity);

    /* draw mesh */
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
