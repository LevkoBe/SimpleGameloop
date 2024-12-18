#pragma once
#include "SceneNode.h"
#include <vector>
#include <memory>
#include <fstream>

class GameState {
private:
    ResourceManager& resourceManager;
    Rectangle worldBounds;
    std::vector<std::shared_ptr<SceneNode>> sceneNodes;

public:
    GameState(ResourceManager& resourceManager, Rectangle worldBounds)
        : resourceManager(resourceManager), worldBounds(worldBounds) {}

    void Register(std::shared_ptr<SceneNode> node) {
        sceneNodes.push_back(std::move(node));
    }

    void Update(float deltaTime, int screenWidth, int screenHeight) {
        for (auto& node : sceneNodes) node->Update(deltaTime);
    }

    void Draw() const {
        for (const auto& node : sceneNodes) node->Draw();
    }

    void Save(std::ofstream& file) const {
        size_t nodeCount = sceneNodes.size();
        file.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
        for (const auto& node : sceneNodes) node->Save(file);
    }

    void Load(std::ifstream& file) {
        size_t nodeCount;
        file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

        sceneNodes.clear();
        for (size_t i = 0; i < nodeCount; ++i) {
            auto node = std::make_shared<SceneNode>();
            node->Load(file);
            sceneNodes.push_back(std::move(node));
        }
    }
};
