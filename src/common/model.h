/** model.h
 * model class for easy model loading
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Model-Loading/Model
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "light.h"

class Model
{
public:
    Model(const char *modelPath);
    std::vector<Mesh> meshes;

    void draw(Shader *shader);
    void draw(Shader *shader, OrbitCamera *camera, Light *light, glm::mat4 &m4Model);

private:
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadTextures(aiMaterial *mat, aiTextureType type,
                                      std::string typeName);
    Material loadMaterial(aiMaterial *mat);
};

GLuint textureFromFile(const char *fileName, const std::string &directory,
                       bool gamma = false);
