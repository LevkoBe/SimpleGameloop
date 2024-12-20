#pragma once
#include "SceneNode.h"
#include <vector>
#include <memory>
#include <fstream>
#include "Quadtree.h"
#include <numbers>
#include <iostream>

const double M_PI = std::numbers::pi_v<double>;

class GameState {
private:
    ResourceManager& resourceManager;
    Rectangle worldBounds;
    std::unordered_map<int, std::shared_ptr<SceneNode>> sceneNodeMap;
    int nextId = 0;
    Quadtree quadtree;

public:
    GameState(ResourceManager& resourceManager, Rectangle worldBounds)
        : resourceManager(resourceManager), worldBounds(worldBounds), quadtree(worldBounds) {}

    int RegisterEntity(std::shared_ptr<SceneNode> node, int parentId = -1) {
        int id = nextId++;
        if (parentId == -1)
            sceneNodeMap[id] = std::move(node);
        else {
            auto parentIt = sceneNodeMap.find(parentId);
            if (parentIt == sceneNodeMap.end()) throw std::runtime_error("Parent ID not found.");
            parentIt->second->AttachChild(std::move(node));
        }
        return id;
    }

    std::shared_ptr<SceneNode> GetEntityById(int id) {
        auto it = sceneNodeMap.find(id);
        if (it != sceneNodeMap.end())
            return it->second;
        return nullptr;
    }

    void RemoveEntity(int id) {
        auto node = GetEntityById(id);
        if (node) {
            if (node->parent)
                node->parent->DetachChild(*node);
            else
                sceneNodeMap.erase(id);
        }
    }

    void MoveNode(SceneNode& nodeToMove, SceneNode& newParent) {
        if (!nodeToMove.parent)
            throw std::runtime_error("Node to move has no parent and cannot be moved.");

        std::shared_ptr<SceneNode> detachedNode = nodeToMove.parent->DetachChild(nodeToMove);
        newParent.AttachChild(std::move(detachedNode));
    }

    void InsertNodeRecursively(std::shared_ptr<SceneNode> node) {
        quadtree.Insert(node);

        for (const auto& child : node->GetChildren()) InsertNodeRecursively(child);
    }

    void Update(float deltaTime, int screenWidth, int screenHeight) {
        quadtree.Clear();
        for (auto& [id, node] : sceneNodeMap)
            InsertNodeRecursively(node);

        for (auto& node : quadtree.Retrieve(Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight })) {
            Rectangle bounds = node->GetBounds();
            auto nearbyNodes = quadtree.Retrieve(bounds);

            for (const auto& nearbyNode : nearbyNodes) {
                if (node != nearbyNode && node->IsCollidable() && nearbyNode->IsCollidable()) {
                    Vector2 pos1 = node->GetGlobalPosition();
                    Vector2 pos2 = nearbyNode->GetGlobalPosition();
                    Vector2 size1 = node->GetSize();
                    Vector2 size2 = nearbyNode->GetSize();

                    if (node->GetShape() == ShapeType::Circular) {
                        if (nearbyNode->GetShape() == ShapeType::Circular &&
                            CheckCollisionCircles(pos1, size1.x / 2, pos2, size2.x / 2)) {
                            HandleCircularCollision(*node, *nearbyNode);
                        }
                        else if (nearbyNode->GetShape() == ShapeType::Rectangular &&
                            CheckCollisionCircleRec(pos1, size1.x / 2, nearbyNode->GetBounds())) {
                            HandleCircleRectCollision(*node, *nearbyNode);
                        }
                    }
                    else if (node->GetShape() == ShapeType::Rectangular) {
                        if (nearbyNode->GetShape() == ShapeType::Circular &&
                            CheckCollisionCircleRec(pos2, size2.x / 2, bounds)) {
                            HandleCircleRectCollision(*nearbyNode, *node);
                        }
                        else if (nearbyNode->GetShape() == ShapeType::Rectangular &&
                            CheckCollisionRecs(nearbyNode->GetBounds(), bounds)) {
                            HandleRectangularCollision(*node, *nearbyNode);
                        }
                    }
                }
            }
            node->Update(deltaTime, screenWidth, screenHeight);
        }
    }

    void HandleCircularCollision(SceneNode& node1, SceneNode& node2) {
        Vector2 direction = { node2.GetGlobalPosition().x - node1.GetGlobalPosition().x,
                             node2.GetGlobalPosition().y - node1.GetGlobalPosition().y };
        float angle = atan2(direction.y, direction.x);

        node1.SetVelocity(CalculateCollisionResponse(node1.GetVelocity(), angle + M_PI));
        node2.SetVelocity(CalculateCollisionResponse(node2.GetVelocity(), angle));
    }

    void HandleCircleRectCollision(SceneNode& circleNode, SceneNode& rectNode) {
        Vector2 direction = { circleNode.GetGlobalPosition().x - rectNode.GetGlobalPosition().x,
                              circleNode.GetGlobalPosition().y - rectNode.GetGlobalPosition().y };
        float angle = atan2(direction.y, direction.x);

        circleNode.SetVelocity(CalculateCollisionResponse(circleNode.GetVelocity(), angle));
        rectNode.SetVelocity(CalculateCollisionResponse(rectNode.GetVelocity(), angle + M_PI));
    }

    void HandleRectangularCollision(SceneNode& node1, SceneNode& node2) {
        Vector2 direction = { node2.GetGlobalPosition().x - node1.GetGlobalPosition().x,
                              node2.GetGlobalPosition().y - node1.GetGlobalPosition().y };
        float angle = atan2(direction.y, direction.x);

        node1.SetVelocity(CalculateCollisionResponse(node1.GetVelocity(), angle + M_PI));
        node2.SetVelocity(CalculateCollisionResponse(node2.GetVelocity(), angle));
    }

    Vector2 CalculateCollisionResponse(const Vector2& velocity, float angle) {
        float magnitude = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        return { magnitude * cos(angle), magnitude * sin(angle) };
    }

    void Draw() const {
        for (const auto& [id, node] : sceneNodeMap)
            node->Draw();
    }
    // todo: remove
    void Save(std::ofstream& file) const {
        size_t nodeCount = sceneNodeMap.size();
        file.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
        for (const auto& [id, node] : sceneNodeMap)
            node->Save(file);
    }

    void Load(std::ifstream& file) {
        std::unordered_map<int, std::shared_ptr<SceneNode>> previousState = sceneNodeMap;

        try {
            size_t nodeCount;
            file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

            sceneNodeMap.clear();
            for (size_t i = 0; i < nodeCount; ++i) {
                auto node = std::make_shared<SceneNode>(resourceManager);
                node->Load(file);
                sceneNodeMap[nextId++] = std::move(node);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading game state: " << e.what() << std::endl;
            sceneNodeMap = previousState;
        }
    }

    void SaveSceneGraph(std::ofstream& sceneFile) const {
        size_t nodeCount = sceneNodeMap.size();
        sceneFile.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

        for (const auto& [id, node] : sceneNodeMap) {
            sceneFile.write(reinterpret_cast<const char*>(&id), sizeof(id));
            node->SaveScene(sceneFile);
        }
    }

    void LoadSceneGraph(std::ifstream& sceneFile) {
        size_t nodeCount;
        sceneFile.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

        sceneNodeMap.clear();
        for (size_t i = 0; i < nodeCount; ++i) {
            int id;
            sceneFile.read(reinterpret_cast<char*>(&id), sizeof(id));

            auto node = std::make_shared<SceneNode>(resourceManager);
            node->LoadScene(sceneFile);
            sceneNodeMap[id] = std::move(node);
        }
    }

    void SaveSprites(std::ofstream& spriteFile) const {
        for (const auto& [id, node] : sceneNodeMap) node->SaveSprite(spriteFile);
    }

    void LoadSprites(std::ifstream& spriteFile) {
        for (auto& [id, node] : sceneNodeMap) node->LoadSprite(spriteFile);
    }

    void SaveGameState(const std::string& sceneFilePath, const std::string& spriteFilePath) const {
        std::ofstream sceneFile(sceneFilePath, std::ios::binary);
        if (!sceneFile.is_open()) throw std::runtime_error("Failed to open scene file for saving.");
        SaveSceneGraph(sceneFile);
        sceneFile.close();

        std::ofstream spriteFile(spriteFilePath, std::ios::binary);
        if (!spriteFile.is_open()) throw std::runtime_error("Failed to open sprite file for saving.");
        SaveSprites(spriteFile);
        spriteFile.close();
    }

    void LoadGameState(const std::string& sceneFilePath, const std::string& spriteFilePath) {
        std::unordered_map<int, std::shared_ptr<SceneNode>> previousState = sceneNodeMap;

        try {
            std::ifstream sceneFile(sceneFilePath, std::ios::binary);
            if (!sceneFile.is_open()) throw std::runtime_error("Failed to open scene file for loading.");
            LoadSceneGraph(sceneFile);
            sceneFile.close();

            std::ifstream spriteFile(spriteFilePath, std::ios::binary);
            if (!spriteFile.is_open()) throw std::runtime_error("Failed to open sprite file for loading.");
            LoadSprites(spriteFile);
            spriteFile.close();
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading game state: " << e.what() << std::endl;
            sceneNodeMap = previousState;
        }
    }

};
