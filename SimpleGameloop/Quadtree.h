#pragma once
#include "SceneNode.h"
#include <vector>
#include <memory>
#include <algorithm>

class Quadtree {
private:
    static const int MAX_OBJECTS = 5;
    static const int MAX_LEVELS = 5;

    Rectangle bounds;
    std::vector<std::shared_ptr<SceneNode>> objects;
    std::unique_ptr<Quadtree> children[4];

    int GetIndex(const Rectangle& rect) const {
        float verticalMidpoint = bounds.x + bounds.width / 2.0f;
        float horizontalMidpoint = bounds.y + bounds.height / 2.0f;

        bool top = rect.y + rect.height < horizontalMidpoint;
        bool bottom = rect.y > horizontalMidpoint;
        bool left = rect.x + rect.width < verticalMidpoint;
        bool right = rect.x > verticalMidpoint;

        if (top && left) return 0;
        if (top && right) return 1;
        if (bottom && left) return 2;
        if (bottom && right) return 3;

        return -1; // Overlaps multiple quadrants
    }

    void Split() {
        float subWidth = bounds.width / 2.0f;
        float subHeight = bounds.height / 2.0f;

        children[0] = std::make_unique<Quadtree>(Rectangle{ bounds.x, bounds.y, subWidth, subHeight });
        children[1] = std::make_unique<Quadtree>(Rectangle{ bounds.x + subWidth, bounds.y, subWidth, subHeight });
        children[2] = std::make_unique<Quadtree>(Rectangle{ bounds.x, bounds.y + subHeight, subWidth, subHeight });
        children[3] = std::make_unique<Quadtree>(Rectangle{ bounds.x + subWidth, bounds.y + subHeight, subWidth, subHeight });
    }

public:
    Quadtree(Rectangle bounds) : bounds(bounds) {}

    void Clear() {
        objects.clear();
        for (auto& child : children) child.reset();
    }

    void Insert(std::shared_ptr<SceneNode> node) {
        if (children[0]) {
            int index = GetIndex(node->GetBounds());
            if (index != -1) {
                children[index]->Insert(node);
                return;
            }
        }

        objects.push_back(node);

        if (objects.size() > MAX_OBJECTS && !children[0]) {
            Split();

            auto it = objects.begin();
            while (it != objects.end()) {
                int index = GetIndex((*it)->GetBounds());
                if (index != -1) {
                    children[index]->Insert(*it);
                    it = objects.erase(it);
                }
                else ++it;
            }
        }
    }

    std::vector<std::shared_ptr<SceneNode>> Retrieve(const Rectangle& rect) const {
        std::vector<std::shared_ptr<SceneNode>> result;

        int index = GetIndex(rect);
        if (index != -1 && children[0]) {
            auto childResult = children[index]->Retrieve(rect);
            result.insert(result.end(), childResult.begin(), childResult.end());
        }

        result.insert(result.end(), objects.begin(), objects.end());
        return result;
    }
};
