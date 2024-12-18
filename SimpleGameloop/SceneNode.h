#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "raylib.h"
#include "Sprite.h"

class SceneNode {
private:
    std::shared_ptr<Sprite> sprite;
    SceneNode* parent;
    std::vector<std::shared_ptr<SceneNode>> children;

public:
    SceneNode();
    SceneNode(std::shared_ptr<Sprite> sprite);

    void AttachChild(std::shared_ptr<SceneNode> child);
    std::shared_ptr<SceneNode> DetachChild(const SceneNode& node);
    const std::vector<std::shared_ptr<SceneNode>>& GetChildren() const;

    void Update(float deltaTime);
    void Draw() const;

    Vector2 GetGlobalPosition() const;
    float GetGlobalRotation() const;

    Rectangle GetBounds() const;

    virtual void Save(std::ofstream& file) const;
    virtual void Load(std::ifstream& file);
};
