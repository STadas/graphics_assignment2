/** model.h
 * model class for easy model loading
 * Tadas Saltenis November 2021
 * NOTE: modified example from https://learnopengl.com/Model-Loading/Model
 */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "model.h"

/**
 * constructor
 */
Model::Model(const char *modelPath)
{
    loadModel(modelPath);
}

/**
 * draw all of the meshes in the model
 */
void Model::draw(Shader *shader, OrbitCamera *camera, Light *light, glm::mat4 &m4Model, float farPlane)
{
    shader->use();

    shader->setVec4("light.position", light->position);
    shader->setFloat("light.ambient", light->ambient);
    shader->setFloat("light.diffuse", light->diffuse);
    shader->setFloat("light.specular", light->specular);

    shader->setMat4("transform.view", camera->view);
    shader->setMat4("transform.projection", camera->projection);
    shader->setMat4("transform.model", m4Model);

    shader->setFloat("far_plane", farPlane);

    for (Mesh mesh : meshes)
    {
        mesh.draw(shader);
    }

    glUseProgram(0);
}

/**
 * load the model from the file path. this is called in the contructor
 */
void Model::loadModel(std::string modelPath)
{
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = modelPath.substr(0, modelPath.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

/**
 * recursively process the model's meshes
 */
void Model::processNode(aiNode *node, const aiScene *scene)
{
    /* process all the node's meshes */
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    /* std::cout << "Processing model node: " << node->mName.C_Str() << std::endl; */

    /* recursively continue to children */
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

/**
 * process a single mesh
 */
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    Material matProps;

    /* std::cout << mesh->mName.C_Str() << std::endl; */
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        /* process vertex positions */
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;

        /* process vertex normals */
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.normal = normal;

        /* process vertex tangents */
        if (mesh->HasTangentsAndBitangents())
        {
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.tangent = tangent;
        }

        /* process vertex texture coordinates */
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 uv;
            uv.x = mesh->mTextureCoords[0][i].x;
            uv.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = uv;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.f, 0.f);
        }

        vertices.push_back(vertex);
    }

    /* process indices */
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    /* process material */
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps =
            loadTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> normalMaps =
            loadTextures(mat, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        matProps = loadMaterial(mat);
    }

    return Mesh(vertices, indices, textures, matProps);
}

std::vector<Texture> Model::loadTextures(aiMaterial *mat, aiTextureType type,
                                         std::string typeName)
{
    std::vector<Texture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);

        Texture texture;
        texture.id = textureFromFile(texturePath.C_Str(), directory);
        texture.type = typeName;
        texture.texPath = texturePath.C_Str();
        textures.push_back(texture);
    }

    return textures;
}

/**
 * load the properties of a material
 */
Material Model::loadMaterial(aiMaterial *mat)
{
    Material material;
    aiColor3D color(0.f, 0.f, 0.f);
    GLfloat shininess;
    GLfloat opacity;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material.diffuse = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material.ambient = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material.specular = glm::vec3(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;

    mat->Get(AI_MATKEY_OPACITY, opacity);
    material.opacity = opacity;

    return material;
}

GLuint textureFromFile(const char *fileName, const std::string &directory, bool gamma)
{
    std::string filePath = std::string(fileName);
    filePath = directory + '/' + filePath;

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, componentCount;
    unsigned char *data =
        stbi_load(filePath.c_str(), &width, &height, &componentCount, 0);
    if (data)
    {
        GLenum colorFormat;
        if (componentCount == 1)
            colorFormat = GL_RED;
        else if (componentCount == 4)
            colorFormat = GL_RGBA;
        else
            colorFormat = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << fileName << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
