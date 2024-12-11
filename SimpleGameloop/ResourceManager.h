#pragma once
#include <vector>
#include "Saveable.h"
#include <iostream>

class ResourceManager {
    std::vector<Saveable*> resources;

public:
    void Register(Saveable* resource) {
        resources.push_back(resource);
    }

    void SaveAll(const char* filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            for (auto* resource : resources) {
                resource->Save(file);
            }
            file.close();
        }
    }

    void LoadAll(const char* filename) {
        try
        {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                for (auto* resource : resources) {
                    resource->Load(file);
                }
                file.close();
            }
        }
        catch (const std::exception&)
        {
            std::cout << "The file could not be read." << std::endl;
        }
    }
};
