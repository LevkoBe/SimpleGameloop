#pragma once
#include "Saveable.h"
#include "raylib.h"
#include <fstream>

class Sprite : public Saveable {
protected:
    Vector2 position;
    float rotation;
    Vector2 velocity;

public:
    Sprite(Vector2 initialPosition = { 0, 0 }, float initialRotation = 0.0f, Vector2 initialVelocity = { 0, 0 });

    virtual void Update(float deltaTime, int screenWidth, int screenHeight);
    virtual void Draw() const;

    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
