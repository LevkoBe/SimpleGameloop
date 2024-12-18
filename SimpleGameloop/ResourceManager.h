#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <filesystem>

class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;
    Sound defaultSound;

public:
    ResourceManager();
    Texture2D GetTexture(const std::string& path, int width = 100, int height = 100);
    Sound GetSound(const std::string& path);
    void SaveResourceKey(std::ofstream& file, const std::string& path);
    std::string LoadResourceKey(std::ifstream& file);
    void UnloadAll();
    ~ResourceManager();
};
