module;

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <utility>

export module XEngine.Resource.Shader.ShaderManager;

import XEngine.Resource.Shader.ShaderLoader;
import XEngine.Resource.Shader.ShaderCompiler;

struct Shader 
{
    GLuint ID;
    std::string name;
    ShaderSource source;

    bool IsValid() const { return ID != 0; }
};

export class ShaderManager
{
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

    GLuint currentShader = 0;

public:
    ~ShaderManager() { ClearAll(); }

    Shader* LoadShaderByName(const std::string& name, bool hasGeometry = false) 
    {
        ShaderSource source = LoadShader(name.c_str(), hasGeometry);

        GLuint glID = 0;
        if (hasGeometry)
            glID = CompileShader(source.vertex, source.fragment, source.geometry);
        else
            glID = CompileShader(source.vertex, source.fragment, std::string());

        auto shader = std::make_unique<Shader>();
        shader->ID = glID;
        shader->name = name;
        shader->source = source;

        Shader* shaderPtr = shader.get();
        shaders[name] = std::move(shader);

        return shaderPtr;
    }

    void UnLoad(const std::string& name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
        {
            GLuint id = it->second->ID;

            if (currentShader == id)
                UnBind();

            if (id != 0)
                glDeleteProgram(id);

            shaders.erase(it);
        }
    }

    void Bind(const std::string& name) 
    {
        Shader* shader = GetShader(name);
        if (shader && shader->IsValid())
        {
            glUseProgram(shader->ID);
            currentShader = shader->ID;
        }
    }

    void UnBind() 
    {
        glUseProgram(0);
        currentShader = 0;
    }

    void ClearAll()
    {
        for (auto& [name, shader] : shaders)
            if (shader->IsValid())
                glDeleteProgram(shader->ID);

        shaders.clear();
        currentShader = 0;
    }

    Shader* GetShader(const std::string& name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
            return it->second.get();

        return nullptr;
    }

    GLuint GetCurrentShader()
    {
        return currentShader;
    }

    size_t GetCount()
    {
        return shaders.size();
    }
};