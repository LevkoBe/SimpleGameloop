#include <vector>
#include <memory>
#include "SceneNode.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Background.h"
#include <cmath>
#include "Wall.h"
#include "Platform.h"

class SpriteFactory {
public:
    static std::vector<std::shared_ptr<SceneNode>> CreateSprites(
        const std::string& spriteType,
        int quantity,
        const Rectangle& boundaries,
        ResourceManager& resourceManager)
    {
        std::vector<std::shared_ptr<SceneNode>> sprites;

        float spacingX = static_cast<float>(boundaries.width) / (quantity + 1);
        float spacingY = static_cast<float>(boundaries.height) / (quantity + 1);

        for (int i = 0; i < quantity; ++i) {
            Vector2 position = {
                boundaries.x + (i + 1) * spacingX,
                boundaries.y + (i + 1) * spacingY
            };

            std::shared_ptr<SceneNode> spriteNode;

            if (spriteType == "Player") {
                spriteNode = std::make_shared<SceneNode>(
                    std::make_shared<Player>(resourceManager, position, Vector2{ 100, 100 }, ShapeType::Circular),
                    resourceManager
                );
            } else if (spriteType == "Background") {
                spriteNode = std::make_shared<SceneNode>(
                    std::make_shared<Background>(resourceManager),
                    resourceManager
                );
            } else if (spriteType == "Wall") {
                spriteNode = std::make_shared<SceneNode>(
                    std::make_shared<Wall>(resourceManager, position, Vector2{ 100, 100 }, ShapeType::Rectangular),
                    resourceManager
                );
            } else if (spriteType == "Platform") {
                spriteNode = std::make_shared<SceneNode>(
                    std::make_shared<Platform>(resourceManager, position, Vector2{ 100, 100 }, Vector2{ 100, 0 }, ShapeType::Rectangular),
                    resourceManager
                );
            }

            sprites.push_back(std::move(spriteNode));
        }

        return sprites;
    }
};
