#pragma once
#include "raylib.h"
#include <random>

class TextureManager {
public:
    static Texture2D LoadTextureWithFallback(const char* path, int width = 0, int height = 0) {
        Texture2D texture = LoadTexture(path);
        if (texture.id == 0) {
            if (width == 0 || height == 0) {
                width = 100;
                height = 100;
            }
            Image img = GenImageColor(width, height, Color{
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                255
                });
            texture = LoadTextureFromImage(img);
            UnloadImage(img);
        }
        return texture;
    }
};