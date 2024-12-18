#include "SceneNode.h"
#include <stdexcept>

SceneNode::SceneNode()
    : sprite(nullptr), parent(nullptr) {}

SceneNode::SceneNode(std::shared_ptr<Sprite> sprite)
    : sprite(sprite), parent(nullptr) {}

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

void SceneNode::Update(float deltaTime) {
    if (sprite) sprite->Update(deltaTime, 0, 0);
    for (const auto& child : children) child->Update(deltaTime);
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
    sprite->Save(file);

    size_t childCount = children.size();
    file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (const auto& child : children) child->Save(file);
}

void SceneNode::Load(std::ifstream& file) {
    sprite = std::make_shared<Sprite>();
    sprite->Load(file);

    size_t childCount;
    file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    for (size_t i = 0; i < childCount; ++i) {
        auto child = std::make_shared<SceneNode>(sprite);
        child->Load(file);
        AttachChild(std::move(child));
    }
}
