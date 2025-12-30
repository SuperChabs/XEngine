#include "scene/Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    gpuMesh = std::make_unique<GPUMesh>(vertices, indices);
    material = std::make_unique<Material>(textures);    
}

Mesh::Mesh(GPUMesh* gpuMeshPtr, Material* materialPtr)
{
    gpuMesh = std::unique_ptr<GPUMesh>(gpuMeshPtr);
    material = std::unique_ptr<Material>(materialPtr);
}

void Mesh::Draw(Shader& shader)
{  
    material->Bind(shader);
    gpuMesh->Draw();
    material->Unbind();
}

