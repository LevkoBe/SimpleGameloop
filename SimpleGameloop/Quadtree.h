#pragma once
#include <vector>
#include "raylib.h"
#include "SceneNode.h"

class Quadtree {
private:
    Rectangle bounds;
    int capacity;
    std::vector<SceneNode*> objects;
    bool divided;

    std::shared_ptr<Quadtree> northwest;
    std::shared_ptr<Quadtree> northeast;
    std::shared_ptr<Quadtree> southwest;
    std::shared_ptr<Quadtree> southeast;

    void Subdivide();

public:
    Quadtree(Rectangle bounds, int capacity);

    void Insert(SceneNode* object);
    std::vector<SceneNode*> Query(Rectangle area);

    void Clear();
    void DrawDebug() const;
};
