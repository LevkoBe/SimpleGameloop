#include "SceneNode.h"
#include <stdexcept>
#include "Player.h"
#include "Background.h"
#include "Wall.h"
#include "Platform.h"

enum class SpriteType {
    PlayerSprite,
    WallSprite,
    BackgroundSprite,
    PlatformSprite
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
    if (sprite) {
        sprite->Update(deltaTime, screenWidth, screenHeight);

        sprite->position.x += sprite->velocity.x * deltaTime;
        sprite->position.y += sprite->velocity.y * deltaTime;
    }
    if (sprite->collidable) ConstrainToBounds(screenWidth, screenHeight);

    for (const auto& child : children) child->Update(deltaTime, screenWidth, screenHeight);
}

void SceneNode::ConstrainToBounds(int screenWidth, int screenHeight) {
    if (sprite) {
        float halfWidth = sprite->size.x / 2.0f;
        float halfHeight = sprite->size.y / 2.0f;
        auto position = GetGlobalPosition();
        auto offset_x = position.x - sprite->position.x;
        auto offset_y = position.y - sprite->position.y;

        if (position.x - halfWidth < 0) {
            sprite->position.x = halfWidth - offset_x;
            sprite->velocity.x = -sprite->velocity.x;
            sprite->OnCollision();
        }
        if (position.x + halfWidth > screenWidth) {
            sprite->position.x = screenWidth - halfWidth - offset_x;
            sprite->velocity.x = -sprite->velocity.x;
            sprite->OnCollision();
        }
        if (position.y - halfHeight < 0) {
            sprite->position.y = halfHeight - offset_y;
            sprite->velocity.y = -sprite->velocity.y;
            sprite->OnCollision();
        }
        if (position.y + halfHeight > screenHeight) {
            sprite->position.y = screenHeight - halfHeight - offset_y;
            sprite->velocity.y = -sprite->velocity.y;
            sprite->OnCollision();
        }
    }
}

void SceneNode::Draw() const {
    auto pos = GetGlobalPosition();
    if (sprite) sprite->Draw(pos.x, pos.y);
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

void SceneNode::SaveScene(std::ofstream& file) const {
    size_t childCount = children.size();
    file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (const auto& child : children) child->SaveScene(file);
}

void SceneNode::LoadScene(std::ifstream& file) {
    size_t childCount;
    file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    for (size_t i = 0; i < childCount; ++i) {
        auto child = std::make_shared<SceneNode>(resourceManager);
        child->LoadScene(file); 
        AttachChild(std::move(child));
    }
}

void SceneNode::SaveSprite(std::ofstream& file) const {
    if (dynamic_cast<Player*>(sprite.get())) {
        SpriteType type = SpriteType::PlayerSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else if (dynamic_cast<Wall*>(sprite.get())) {
        SpriteType type = SpriteType::WallSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else if (dynamic_cast<Background*>(sprite.get())) {
        SpriteType type = SpriteType::BackgroundSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else if (dynamic_cast<Platform*>(sprite.get())) {
        SpriteType type = SpriteType::PlatformSprite;
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    else {
        throw std::runtime_error("Unknown sprite type during saving");
    }
    sprite->Save(file);
    for (const auto& child : children) child->SaveSprite(file);
}

void SceneNode::LoadSprite(std::ifstream& file) {
    SpriteType type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));

    switch (type) {
    case SpriteType::PlayerSprite:
        sprite = std::make_shared<Player>(resourceManager, Vector2{ 0, 0 }, Vector2{ 0, 0 });
        break;
    case SpriteType::WallSprite:
        sprite = std::make_shared<Wall>(resourceManager, Vector2{ 0, 0 }, Vector2{ 0, 0 });
        break;
    case SpriteType::PlatformSprite:
        sprite = std::make_shared<Platform>(resourceManager, Vector2{ 0, 0 }, Vector2{ 0, 0 }, Vector2{ 0, 0 });
        break;
    case SpriteType::BackgroundSprite:
        sprite = std::make_shared<Background>(resourceManager);
        break;
    default:
        throw std::runtime_error("Unknown sprite type during loading");
    }

    sprite->Load(file);
    for (const auto& child : children) child->LoadSprite(file);
}
