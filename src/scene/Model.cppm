module;

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

export module XEngine.Scene.Model;

import XEngine.Scene.Mesh;
import XEngine.Core.Shader;
import XEngine.Rendering.MeshData;

export class Model 
{
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(void* mat, int type, const std::string& typeName);

public:
    Model(const char* path) 
    {
        loadModel(path);
    }

    Model(Mesh* mesh, std::string name = "") 
    { 
        meshes.push_back(std::move(*mesh)); 
    }

    void Draw(Shader &shader);

    void SetColor(const glm::vec3& color);
    void SetTextures(const std::vector<Texture>& textures);
    
    size_t getMeshCount() const;
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);