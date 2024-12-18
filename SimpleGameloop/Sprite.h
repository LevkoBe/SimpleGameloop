#pragma once
#include "Saveable.h"
#include "raylib.h"
#include <fstream>

class Sprite : public Saveable {
public:
    Vector2 velocity;
    bool collidable;
    Vector2 position;
    Vector2 size;
    float rotation;

    Sprite(Vector2 initialPosition = { 0, 0 }, Vector2 size = { 0, 0 }, float initialRotation = 0.0f, Vector2 initialVelocity = { 0, 0 }, bool collidable = true);

    virtual void Update(float deltaTime, int screenWidth, int screenHeight);
    virtual void Draw() const;

    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
