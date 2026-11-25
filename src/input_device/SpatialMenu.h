#pragma once
#include <vector>
#include <cmath>
#include <limits>

struct MenuRect
{
    int id, kind;
    float x1, y1, x2, y2;
};

// Geometry rather than declaration order determines controller focus.
inline int spatialMenuNext(const std::vector<MenuRect>& items, int current, int dx, int dy)
{
    const MenuRect* from = nullptr;
    for (const auto& item : items) if (item.id == current) from = &item;
    if (!from) return items.empty() ? -1 : items.front().id;
    int result = current;
    float best = std::numeric_limits<float>::max();
    for (const auto& item : items)
    {
        if (item.id == current) continue;
        const float x = (item.x1 + item.x2 - from->x1 - from->x2) * 0.5f;
        // Tall list panels align with the controls beside their first row,
        // rather than treating their middle as an unrelated navigation row.
        const float y = item.y1 + std::fmin(item.y2-item.y1,40.0f)*0.5f
                      - from->y1 - std::fmin(from->y2-from->y1,40.0f)*0.5f;
        const float forward = dx ? x * dx : y * dy;
        if (forward <= 1) continue;
        const float overlap = dx ? std::fmin(item.y2, from->y2) - std::fmax(item.y1, from->y1)
                                 : std::fmin(item.x2, from->x2) - std::fmax(item.x1, from->x1);
        const float sideways = dx ? std::fabs(y) : std::fabs(x);
        // Prefer aligned rows/columns before diagonally placed controls.
        const float score = (overlap < 0 ? 10000.0f : 0.0f) + forward + sideways * 2;
        if (score < best) { best = score; result = item.id; }
    }
    return result;
}

