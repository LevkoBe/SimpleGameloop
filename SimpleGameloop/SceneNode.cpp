#include "SceneNode.h"
#include <stdexcept>
#include "Player.h"
#include "Background.h"

enum class SpriteType {
    PlayerSprite,
    BackgroundSprite
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

bool SceneNode::IsCollidable() const {
    return sprite->collidable;
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
    return { globalPosition.x - sprite->size.x / 2, globalPosition.y - sprite->size.y / 2, sprite->size.x, sprite->size.y };
}

ShapeType SceneNode::GetShape() const {
    return sprite->shape;
}

Vector2 SceneNode::GetSize() const {
    return sprite->size;
}

Vector2 SceneNode::GetVelocity() const {
    return sprite->velocity;
}

void SceneNode::SetVelocity(const Vector2& velocity) {
    sprite->velocity = velocity;
}

void SceneNode::Save(std::ofstream& file) const {
    if (dynamic_cast<Player*>(sprite.get())) {
        SpriteType type = SpriteType::PlayerSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else if (dynamic_cast<Background*>(sprite.get())) {
        SpriteType type = SpriteType::BackgroundSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else {
        throw std::runtime_error("Unknown sprite type during saving");
    }

    sprite->Save(file);

    size_t childCount = children.size();
    file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (const auto& child : children) {
        child->Save(file);
    }
}

void SceneNode::Load(std::ifstream& file) {
    SpriteType type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));

    switch (type) {
    case SpriteType::PlayerSprite:
        sprite = std::make_shared<Player>(resourceManager, Vector2{ 0, 0 }, Vector2{ 0, 0 });
        break;
    case SpriteType::BackgroundSprite:
        sprite = std::make_shared<Background>(resourceManager);
        break;
    default:
        throw std::runtime_error("Unknown sprite type during loading");
    }

    sprite->Load(file);

    size_t childCount;
    file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    for (size_t i = 0; i < childCount; ++i) {
        auto child = std::make_shared<SceneNode>(resourceManager);
        child->Load(file);
        AttachChild(std::move(child));
    }
}

