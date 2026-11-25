#include "input_device/SpatialMenu.h"
#include <cassert>
int main()
{
    // Deliberately shuffled declaration order: Yes/No must follow layout.
    std::vector<MenuRect> dialog{{30,0,200,400,600,440}, {20,0,440,300,520,340}, {10,0,280,300,360,340}};
    assert(spatialMenuNext(dialog,10,1,0)==20);
    assert(spatialMenuNext(dialog,20,-1,0)==10);
    assert(spatialMenuNext(dialog,10,0,1)==30);
    assert(spatialMenuNext(dialog,10,-1,0)==10);
    // Two columns of options: down stays in the same column.
    std::vector<MenuRect> grid{{1,0,20,20,350,60},{2,0,420,20,750,60},
                             {3,1,20,100,350,250},{4,1,420,100,750,250},
                             {5,2,20,300,230,330},{6,2,420,300,630,330}};
    assert(spatialMenuNext(grid,1,0,1)==3);
    assert(spatialMenuNext(grid,3,1,0)==4);
    assert(spatialMenuNext(grid,4,0,1)==6);
    assert(spatialMenuNext(grid,6,0,-1)==4);
    assert(spatialMenuNext({},1,1,0)==-1);
}
