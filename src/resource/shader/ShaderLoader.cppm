module;

#include <string>
#include <fstream>
#include <sstream>
#include <utility>

export module XEngine.Resource.Shader.ShaderLoader;

import XEngine.Core.Logger;

std::string FileToString(const std::string& path);

export struct ShaderSource
{
    std::string vertex;
    std::string fragment;
    std::string geometry;
    std::string name;

    bool HasGeometry() { return geometry.empty(); }
};

export ShaderSource LoadShader(const char* shaderName, bool isGeometry = false)
{
    ShaderSource source;
    source.name = shaderName;

    const std::string name(shaderName);

    const std::string vs = "assets/shaders/vertex/"   + name + ".vs";
    const std::string fs = "assets/shaders/fragment/" + name + ".fs";

    if (isGeometry)
    {
        const std::string gs = "assets/shaders/geometry/" + name + ".gs";
        source = 
        {
            .vertex = FileToString(vs),
            .fragment = FileToString(fs),
            .geometry = FileToString(gs)
        };
    }
    else
    {
        source = 
        {
            .vertex = FileToString(vs),
            .fragment = FileToString(fs)
        };
    }

    return source;
}

std::string FileToString(const std::string& path)
{
    std::ifstream vFile(path.c_str());
    std::stringstream stream;

    stream << vFile.rdbuf();
    return std::move(stream.str());
}