#include "DisplayLayout.h"
#include <cassert>
#include <cmath>
static bool near(float a,float b) { return std::abs(a-b)<0.001f; }
int main() {
    DisplayLayout hd(1920,1080);
    assert(near(hd.scale,1.8f));
    assert(near(hd.screenX(0),240) && near(hd.screenX(800),1680));
    assert(near(hd.screenX(400),960) && near(hd.screenY(600),1080));
    // Equal scaling keeps the ball round and preserves relative distances.
    assert(near(hd.screenX(100)-hd.screenX(0),hd.screenY(100)-hd.screenY(0)));
    assert(!hd.contains(239,500) && hd.contains(240,500));
    assert(hd.contains(1679,500) && !hd.contains(1680,500));
    for(int y=0;y<=600;y+=20) for(int x=0;x<=800;x+=20) {
        assert(near(hd.worldX(hd.screenX(x)),float(x)));
        assert(near(hd.worldY(hd.screenY(y)),float(y)));
    }
    DisplayLayout original(800,600), smaller(1280,720);
    assert(near(original.scale,1) && near(original.left,0));
    assert(near(smaller.left,160) && near(smaller.scale,1.2f));
}

