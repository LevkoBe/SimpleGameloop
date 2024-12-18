#include "Quadtree.h"

Quadtree::Quadtree(Rectangle bounds, int capacity)
    : bounds(bounds), capacity(capacity), divided(false) {}

void Quadtree::Subdivide() {
    float x = bounds.x;
    float y = bounds.y;
    float width = bounds.width / 2.0f;
    float height = bounds.height / 2.0f;

    northwest = std::make_shared<Quadtree>(Rectangle{ x, y, width, height }, capacity);
    northeast = std::make_shared<Quadtree>(Rectangle{ x + width, y, width, height }, capacity);
    southwest = std::make_shared<Quadtree>(Rectangle{ x, y + height, width, height }, capacity);
    southeast = std::make_shared<Quadtree>(Rectangle{ x + width, y + height, width, height }, capacity);

    divided = true;
}

void Quadtree::Insert(SceneNode* object) {
    Rectangle objBounds = object->GetBounds();

    if (!CheckCollisionRecs(bounds, objBounds)) {
        return;
    }

    if (objects.size() < capacity && !divided) {
        objects.push_back(object);
    }
    else {
        if (!divided) {
            Subdivide();
        }

        northwest->Insert(object);
        northeast->Insert(object);
        southwest->Insert(object);
        southeast->Insert(object);
    }
}

std::vector<SceneNode*> Quadtree::Query(Rectangle area) {
    std::vector<SceneNode*> found;

    if (!CheckCollisionRecs(bounds, area)) {
        return found;
    }

    for (SceneNode* object : objects) {
        if (CheckCollisionRecs(area, object->GetBounds())) {
            found.push_back(object);
        }
    }

    if (divided) {
        auto nwResults = northwest->Query(area);
        auto neResults = northeast->Query(area);
        auto swResults = southwest->Query(area);
        auto seResults = southeast->Query(area);

        found.insert(found.end(), nwResults.begin(), nwResults.end());
        found.insert(found.end(), neResults.begin(), neResults.end());
        found.insert(found.end(), swResults.begin(), swResults.end());
        found.insert(found.end(), seResults.begin(), seResults.end());
    }

    return found;
}

void Quadtree::Clear() {
    objects.clear();

    if (divided) {
        northwest->Clear();
        northeast->Clear();
        southwest->Clear();
        southeast->Clear();

        northwest.reset();
        northeast.reset();
        southwest.reset();
        southeast.reset();

        divided = false;
    }
}

void Quadtree::DrawDebug() const {
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GREEN);

    if (divided) {
        northwest->DrawDebug();
        northeast->DrawDebug();
        southwest->DrawDebug();
        southeast->DrawDebug();
    }
}
