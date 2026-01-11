module;

#include <entt.hpp>
#include <glm/glm.hpp>

export module XEngine.Core.ECS.Systems;

import XEngine.Core.ECS.Components;
import XEngine.Core.ECS.ECSWorld;
import XEngine.Core.Shader;
import XEngine.Core.Camera;

export class RenderSystem 
{
public:
    void Update(ECSWorld& world, Shader& shader, Camera& camera) 
    {
        shader.use();
        
        world.Each<TransformComponent, MeshComponent, MaterialComponent, VisibilityComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                MeshComponent& meshComp,
                MaterialComponent& matComp,
                VisibilityComponent& vis) 
        {
            if (!vis.isActive || !vis.visible) return;
            
            shader.setMat4("model", transform.GetModelMatrix());
            
            if (world.HasComponent<ColorComponent>(entity)) 
            {
                auto& color = world.GetComponent<ColorComponent>(entity);
                if (meshComp.mesh) 
                {
                    meshComp.mesh->SetColor(color.color);
                }
            }
            
            if (meshComp.mesh) 
                meshComp.mesh->Draw(shader);
        });
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
            {
                transform.rotation += rotation.axis * rotation.speed * deltaTime;
            }
        });
    }
};