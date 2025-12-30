#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene/Model.h"
#include "scene/Transform.h"
#include <memory>
#include <vector>

struct SceneObject 
{
    std::string name;
    Model* model;
    Transform transform;
    bool isActive;
    
    SceneObject(const std::string& objName, Model* mdl) 
        : name(objName), model(mdl), isActive(true) {}
        
    SceneObject(const std::string& objName, Model* mdl, const Transform& trans)  
        : name(objName), model(mdl), transform(trans), isActive(true) {}
};

class SceneManager 
{
private:
    std::vector<std::unique_ptr<SceneObject>> objects;
    
public:
    SceneManager() = default;
    ~SceneManager() = default;
    
    SceneObject* AddObject(const std::string& name, Model* model);
    SceneObject* AddObject(const std::string& name, Model* model, const Transform& transform);
    
    void RemoveObject(SceneObject* object);
    void Clear();
    
    void RenderAll(Shader& shader);
    void Update(float deltaTime);
    
    const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};

#endif