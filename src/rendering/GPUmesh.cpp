#include "rendering/GPUMesh.h"

// GPUMesh implementation
GPUMesh::GPUMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    usedIndices = true;

    indexCount = indices.size();

    VAO = std::make_unique<VertexArray>();  
    // glGenVertexArrays(1, &VAO);
    VBO = std::make_unique<VertexBuffer>(); 
    // glGenBuffers(1, &VBO);
    EBO = std::make_unique<IndexBuffer>();  
    // glGenBuffers(1, &EBO);

    VAO->Bind(); 
    // glBindVertexArray(VAO);   

    VBO->Bind(); 
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    VBO->SetData(vertices.data(), vertices.size() * sizeof(Vertex), GL_STATIC_DRAW); 
    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); 

    EBO->Bind(); 
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    EBO->SetData(indices.data(), static_cast<unsigned int>(indices.size()), GL_STATIC_DRAW); 
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex Positions
    VAO->AddAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex Normals
    VAO->AddAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Normal));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex Texture Coords
    VAO->AddAttribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, TexCoords));
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Vertex Tangent
    VAO->AddAttribute(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Tangent));
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    // Vertex Bitangent
    VAO->AddAttribute(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Bitangent));
    // glEnableVertexAttribArray(4);
    // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    VAO->Unbind();
    // glBindVertexArray(0);
}

GPUMesh::GPUMesh(const float* data, size_t dataSize, const int stride)
{
    usedIndices = false;

    vertexCount = dataSize / (stride * sizeof(float));
    indexCount = 0; 
    
    VAO = std::make_unique<VertexArray>();
    VBO = std::make_unique<VertexBuffer>();
    
    VBO->SetData(data, dataSize);
    
    VAO->Bind();
    VBO->Bind();
    
    VAO->AddAttribute(0, 3, GL_FLOAT, false, stride * sizeof(float), 0);
    VAO->AddAttribute(1, 3, GL_FLOAT, false, stride * sizeof(float), 3 * sizeof(float));
    VAO->AddAttribute(2, 2, GL_FLOAT, false, stride * sizeof(float), 6 * sizeof(float));
    
    VAO->Unbind();
}

void GPUMesh::Draw()
{
    VAO->Bind();
    // glBindVertexArray(VAO);
    if (usedIndices)
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    else 
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    VAO->Unbind();
    // glBindVertexArray(0);
}