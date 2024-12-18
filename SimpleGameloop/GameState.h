#pragma once
#include <vector>
#include <memory>
#include "Sprite.h"
#include "Player.h"
#include "Background.h"

enum class SpriteType { Player, Background };

class GameState : public Saveable {
private:
    std::vector<std::unique_ptr<Sprite>> sprites;
    ResourceManager& resourceManager;

public:
    GameState(ResourceManager& resourceManager);

    void Register(std::unique_ptr<Sprite> sprite);
    void Update(float deltaTime, int screenWidth, int screenHeight);
    void Draw() const;

    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
