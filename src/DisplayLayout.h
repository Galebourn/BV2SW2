#pragma once
#include <algorithm>

// Presentation only. Never use display dimensions as physics/world bounds.
struct DisplayLayout {
    float scale, left, top;
    DisplayLayout(float width,float height)
        : scale(std::min(width/800.0f,height/600.0f)),
          left((width-800.0f*scale)/2.0f),
          top((height-600.0f*scale)/2.0f) {}
    float screenX(float worldX) const { return left+worldX*scale; }
    float screenY(float worldY) const { return top+worldY*scale; }
    float worldX(float screenX) const { return (screenX-left)/scale; }
    float worldY(float screenY) const { return (screenY-top)/scale; }
    bool contains(float x,float y) const {
        return x>=left && x<left+800*scale && y>=top && y<top+600*scale;
    }
};

