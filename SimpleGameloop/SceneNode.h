#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "raylib.h"
#include "Sprite.h"
#include "ResourceManager.h"

class SceneNode {
private:
    std::shared_ptr<Sprite> sprite;
    SceneNode* parent;
    std::vector<std::shared_ptr<SceneNode>> children;
    ResourceManager& resourceManager;

public:
    SceneNode(ResourceManager& resourceManager);
    SceneNode(std::shared_ptr<Sprite> sprite, ResourceManager& resourceManager);

    void AttachChild(std::shared_ptr<SceneNode> child);
    std::shared_ptr<SceneNode> DetachChild(const SceneNode& node);
    const std::vector<std::shared_ptr<SceneNode>>& GetChildren() const;

    void Update(float deltaTime, int screenWidth, int screenHeight);
    void Draw() const;

    Vector2 GetGlobalPosition() const;
    float GetGlobalRotation() const;

    Rectangle GetBounds() const;

    virtual void Save(std::ofstream& file) const;
    virtual void Load(std::ifstream& file);
};
