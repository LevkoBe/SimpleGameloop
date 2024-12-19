//#include "GameState.h"
//
//GameState::GameState(ResourceManager& resourceManager, Rectangle worldBounds)
//    : resourceManager(resourceManager), worldBounds(worldBounds), quadtree(worldBounds) {}
//
//void GameState::Register(std::shared_ptr<SceneNode> node, std::shared_ptr<SceneNode> parent) {
//    if (parent) parent->AttachChild(std::move(node));
//    else sceneNodes.push_back(std::move(node));
//}
//
//void GameState::MoveNode(SceneNode& nodeToMove, SceneNode& newParent) {
//    if (!nodeToMove.parent) throw std::runtime_error("Node to move has no parent and cannot be moved.");
//
//    std::shared_ptr<SceneNode> detachedNode = nodeToMove.parent->DetachChild(nodeToMove);
//    newParent.AttachChild(std::move(detachedNode));
//}
//
//void GameState::Update(float deltaTime, int screenWidth, int screenHeight) {
//    quadtree.Clear();
//    for (auto& node : sceneNodes) quadtree.Insert(node);
//
//    for (auto& node : sceneNodes) {
//        Rectangle bounds = node->GetBounds();
//        auto nearbyNodes = quadtree.Retrieve(bounds);
//
//        for (const auto& nearbyNode : nearbyNodes) {
//            if (node != nearbyNode && node->IsCollidable() && nearbyNode->IsCollidable()) {
//                Vector2 pos1 = node->GetGlobalPosition();
//                Vector2 pos2 = nearbyNode->GetGlobalPosition();
//                Vector2 size1 = node->GetSize();
//                Vector2 size2 = nearbyNode->GetSize();
//
//                if (node->GetShape() == ShapeType::Circular) {
//                    if (nearbyNode->GetShape() == ShapeType::Circular &&
//                        CheckCollisionCircles(pos1, size1.x / 2, pos2, size2.x / 2)) {
//                        HandleCircularCollision(*node, *nearbyNode);
//                    }
//                    else if (nearbyNode->GetShape() == ShapeType::Rectangular &&
//                        CheckCollisionCircleRec(pos1, size1.x / 2, nearbyNode->GetBounds())) {
//                        HandleCircleRectCollision(*node, *nearbyNode);
//                    }
//                }
//                else if (node->GetShape() == ShapeType::Rectangular) {
//                    if (nearbyNode->GetShape() == ShapeType::Circular &&
//                        CheckCollisionCircleRec(pos2, size2.x / 2, bounds)) {
//                        HandleCircleRectCollision(*nearbyNode, *node);
//                    }
//                    else if (nearbyNode->GetShape() == ShapeType::Rectangular &&
//                        CheckCollisionRecs(nearbyNode->GetBounds(), bounds)) {
//                        HandleRectangularCollision(*node, *nearbyNode);
//                    }
//                }
//            }
//        }
//        node->Update(deltaTime, screenWidth, screenHeight);
//    }
//}
//
//void GameState::HandleCircularCollision(SceneNode& node1, SceneNode& node2) {
//    Vector2 direction = { node2.GetGlobalPosition().x - node1.GetGlobalPosition().x,
//                            node2.GetGlobalPosition().y - node1.GetGlobalPosition().y };
//    float angle = atan2(direction.y, direction.x);
//
//    node1.SetVelocity(CalculateCollisionResponse(node1.GetVelocity(), angle + M_PI));
//    node2.SetVelocity(CalculateCollisionResponse(node2.GetVelocity(), angle));
//}
//
//void GameState::HandleCircleRectCollision(SceneNode& circleNode, SceneNode& rectNode) {
//    Vector2 direction = { circleNode.GetGlobalPosition().x - rectNode.GetGlobalPosition().x,
//                            circleNode.GetGlobalPosition().y - rectNode.GetGlobalPosition().y };
//    float angle = atan2(direction.y, direction.x);
//
//    circleNode.SetVelocity(CalculateCollisionResponse(circleNode.GetVelocity(), angle));
//    rectNode.SetVelocity(CalculateCollisionResponse(rectNode.GetVelocity(), angle + M_PI));
//}
//
//void GameState::HandleRectangularCollision(SceneNode& node1, SceneNode& node2) {
//    Vector2 direction = { node2.GetGlobalPosition().x - node1.GetGlobalPosition().x,
//                            node2.GetGlobalPosition().y - node1.GetGlobalPosition().y };
//    float angle = atan2(direction.y, direction.x);
//
//    node1.SetVelocity(CalculateCollisionResponse(node1.GetVelocity(), angle + M_PI));
//    node2.SetVelocity(CalculateCollisionResponse(node2.GetVelocity(), angle));
//}
//
//Vector2 GameState::CalculateCollisionResponse(const Vector2& velocity, float angle) {
//    float magnitude = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
//    return { magnitude * cos(angle), magnitude * sin(angle) };
//}
//
//void GameState::Draw() const {
//    for (const auto& node : sceneNodes) node->Draw();
//}
//
//void GameState::Save(std::ofstream& file) const {
//    size_t nodeCount = sceneNodes.size();
//    file.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
//    for (const auto& node : sceneNodes) node->Save(file);
//}
//
//void GameState::Load(std::ifstream& file) {
//    std::vector<std::shared_ptr<SceneNode>> previousState = sceneNodes;
//
//    try {
//        size_t nodeCount;
//        file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));
//
//        sceneNodes.clear();
//        for (size_t i = 0; i < nodeCount; ++i) {
//            auto node = std::make_shared<SceneNode>(resourceManager);
//            node->Load(file);
//            sceneNodes.push_back(std::move(node));
//        }
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error loading game state: " << e.what() << std::endl;
//        sceneNodes = previousState;
//    }
//}