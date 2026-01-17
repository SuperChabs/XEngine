module;

#include <entt.hpp>
#include <glm/glm.hpp>

#include <string>

export module XEngine.ECS.Systems;

import XEngine.ECS.Components; 
import XEngine.ECS.ECSWorld;

import XEngine.Core.Camera;
import XEngine.Core.Logger;

import XEngine.Resource.Shader.ShaderManager; 

import XEngine.Rendering.Light;

export class RenderSystem 
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& name, Camera& camera) 
    {
        shaderManager.Bind(name);
        
        world.Each<TransformComponent, MeshComponent, MaterialComponent, VisibilityComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                MeshComponent& meshComp,
                MaterialComponent& matComp,
                VisibilityComponent& vis) 
        {
            if (!vis.isActive || !vis.visible) return;
            
            shaderManager.SetMat4(name, "model", transform.GetModelMatrix());
        
            if (matComp.material)
            {
                static int frameCount = 0;
                if (frameCount % 300 == 0)
                    Logger::Log(LogLevel::DEBUG, LogCategory::RENDERING, 
                        "Binding material: " + matComp.material->GetName());
                frameCount++;
                
                if (meshComp.mesh) 
                    meshComp.mesh->Draw(shaderManager, name, matComp.material);
            }
            else if (world.HasComponent<ColorComponent>(entity)) 
            {
                auto& color = world.GetComponent<ColorComponent>(entity);
                if (meshComp.mesh) 
                    meshComp.mesh->SetColor(color.color);
                
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
            else
            {
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
        });

        shaderManager.Unbind();
    }
};

export class RotationSystem 
{
public:
    void Update(ECSWorld& world, float deltaTime) 
    {
        world.Each<TransformComponent, RotationComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                RotationComponent& rotation) 
        {
            if (rotation.autoRotate) 
                transform.rotation += rotation.axis * rotation.speed * deltaTime;
        });
    }
};

export class LightSystem
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& shaderName)
    {
        shaderManager.Bind(shaderName);
        
        int lightIndex = 0;
        const int maxLights = 8;
        
        world.Each<LightComponent, TransformComponent>(
            [&](entt::entity entity, LightComponent& light, TransformComponent& transform)
            {
                if (!light.isActive || lightIndex >= maxLights)
                    return;
                
                light.SyncWithTransform(transform);
                
                std::string base = "lights[" + std::to_string(lightIndex) + "]";
                
                shaderManager.SetInt(shaderName, base + ".type", static_cast<int>(light.type));
                
                shaderManager.SetVec3(shaderName, base + ".position", light.position);
                shaderManager.SetVec3(shaderName, base + ".direction", light.direction);
                
                shaderManager.SetVec3(shaderName, base + ".ambient", light.ambient * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".diffuse", light.diffuse * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".specular", light.specular);
                
                if (light.type == LightType::POINT || light.type == LightType::SPOT)
                {
                    shaderManager.SetFloat(shaderName, base + ".constant", light.constant);
                    shaderManager.SetFloat(shaderName, base + ".linear", light.linear);
                    shaderManager.SetFloat(shaderName, base + ".quadratic", light.quadratic);
                }
                
                if (light.type == LightType::SPOT)
                {
                    shaderManager.SetFloat(shaderName, base + ".innerCutoff", 
                        glm::cos(glm::radians(light.innerCutoff)));
                    shaderManager.SetFloat(shaderName, base + ".outerCutoff", 
                        glm::cos(glm::radians(light.outerCutoff)));
                }
                
                lightIndex++;
            });

        shaderManager.SetInt(shaderName, "numLights", lightIndex);
        
        shaderManager.Unbind();
    }
};