#pragma once
#include "Saveable.h"
#include "raylib.h"
#include <fstream>
#include "ShapeType.h"

class Sprite : public Saveable {
public:
    Vector2 velocity;
    bool collidable;
    Vector2 position;
    Vector2 size;
    float rotation;
    ShapeType shape;

    Sprite(Vector2 initialPosition = { 0, 0 }, Vector2 size = { 0, 0 }, float initialRotation = 0.0f, Vector2 initialVelocity = { 0, 0 }, ShapeType shape = Circular, bool collidable = true);

    virtual void Update(float deltaTime, int screenWidth, int screenHeight);
    virtual void OnCollision() const;
    virtual void Draw(int global_x, int global_y) const;

    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
