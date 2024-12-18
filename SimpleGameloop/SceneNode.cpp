#include "SceneNode.h"
#include <stdexcept>
#include "Player.h"
#include "Background.h"

enum class SpriteType {
    Player,
    Background
};

SceneNode::SceneNode(ResourceManager& resourceManager)
    : resourceManager(resourceManager), sprite(nullptr), parent(nullptr) {}

SceneNode::SceneNode(std::shared_ptr<Sprite> sprite, ResourceManager& resourceManager)
    : resourceManager(resourceManager), sprite(sprite), parent(nullptr) {}

void SceneNode::AttachChild(std::shared_ptr<SceneNode> child) {
    child->parent = this;
    children.push_back(std::move(child));
}

std::shared_ptr<SceneNode> SceneNode::DetachChild(const SceneNode& node) {
    auto found = std::find_if(children.begin(), children.end(),
        [&node](const std::shared_ptr<SceneNode>& child) { return child.get() == &node; });

    if (found == children.end()) throw std::runtime_error("Node to detach not found!");

    std::shared_ptr<SceneNode> result = std::move(*found);
    result->parent = nullptr;
    children.erase(found);
    return result;
}

const std::vector<std::shared_ptr<SceneNode>>& SceneNode::GetChildren() const {
    return children;
}

void SceneNode::Update(float deltaTime, int screenWidth, int screenHeight) {
    if (sprite) sprite->Update(deltaTime, screenWidth, screenHeight);
    for (const auto& child : children) child->Update(deltaTime, screenWidth, screenHeight);
}

void SceneNode::Draw() const {
    if (sprite) sprite->Draw();
    for (const auto& child : children) child->Draw();
}

Vector2 SceneNode::GetGlobalPosition() const {
    if (parent) {
        Vector2 parentPosition = parent->GetGlobalPosition();
        return { parentPosition.x + sprite->position.x, parentPosition.y + sprite->position.y };
    }
    return sprite->position;
}

float SceneNode::GetGlobalRotation() const {
    if (parent) return parent->GetGlobalRotation() + sprite->rotation;
    return sprite->rotation;
}

Rectangle SceneNode::GetBounds() const {
    Vector2 globalPosition = GetGlobalPosition();
    return { globalPosition.x, globalPosition.y, sprite->size.x, sprite->size.y };
}

void SceneNode::Save(std::ofstream& file) const {
    // Save the type of the sprite (if exists)
    if (dynamic_cast<Player*>(sprite.get())) {
        SpriteType type = SpriteType::Player;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else if (dynamic_cast<Background*>(sprite.get())) {
        SpriteType type = SpriteType::Background;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else {
        throw std::runtime_error("Unknown sprite type during saving");
    }

    // Save the sprite itself
    sprite->Save(file);

    // Save the number of children
    size_t childCount = children.size();
    file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    // Save each child node recursively
    for (const auto& child : children) {
        child->Save(file);
    }
}

void SceneNode::Load(std::ifstream& file) {
    // Read the type of the sprite
    SpriteType type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));

    // Create the correct type of sprite
    switch (type) {
    case SpriteType::Player:
        sprite = std::make_shared<Player>(Vector2{ 0, 0 }, "", Vector2{ 0, 0 }, "", resourceManager);
        break;
    case SpriteType::Background:
        sprite = std::make_shared<Background>("", resourceManager);
        break;
    default:
        throw std::runtime_error("Unknown sprite type during loading");
    }

    // Load the sprite data
    sprite->Load(file);

    // Read the number of children
    size_t childCount;
    file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    // Load each child node recursively
    for (size_t i = 0; i < childCount; ++i) {
        auto child = std::make_shared<SceneNode>(resourceManager);
        child->Load(file);
        AttachChild(std::move(child));
    }
}
