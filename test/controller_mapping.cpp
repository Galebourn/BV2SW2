#include "input_device/ControllerMapping.h"
#include <cassert>
#include <initializer_list>
int main() {
    using L=ControllerLayout;
    auto f=mapController(L::Full,20000,12000,0,0,(1ULL<<0)|(1ULL<<12));
    assert(f.x==20000 && f.y==-12000 && f.select && f.dpad==1);
    // Rails up: left Joy-Con rotates clockwise, right counterclockwise.
    f=mapController(L::LeftSingle,0,20000,0,0,(1ULL<<15)|(1ULL<<24)|(1ULL<<25));
    assert(f.x==-20000 && f.y==0 && f.select && f.join && !f.dpad);
    f=mapController(L::LeftSingle,20000,0,0,0,1ULL<<12);
    assert(f.y==-20000 && f.back && !f.select);
    f=mapController(L::RightSingle,0,0,0,20000,(1ULL<<2)|(1ULL<<26)|(1ULL<<27));
    assert(f.x==20000 && f.y==0 && f.select && f.join);
    f=mapController(L::RightSingle,0,0,20000,0,1ULL<<0);
    assert(f.y==20000 && f.back && !f.select);
    for(auto layout:{L::Full,L::LeftSingle,L::RightSingle}) {
        assert(mapController(layout,0,0,0,0,1ULL<<10).pause);
        assert(mapController(layout,0,0,0,0,1ULL<<11).pause);
        assert(!mapController(layout,0,0,0,0,0).join);
    }
}
