#define BLOBBY_SWITCH2
#include "input_device/SwitchControllers.h"
#include <cassert>
int main() {
    SwitchControllers pads;
    fakePads[0].style=HidNpadStyleTag_NpadJoyLeft;
    fakePads[3].style=HidNpadStyleTag_NpadJoyRight;
    pads.update(); pads.beginAssignment();
    fakePads[3].buttons=(1ULL<<26)|(1ULL<<27);
    pads.update(); pads.joinPlayers(2); assert(!pads.ready(2));
    fakePads[0].buttons=(1ULL<<24)|(1ULL<<25);
    pads.update(); pads.joinPlayers(2); assert(pads.ready(2));
    // Join order, not slot order, determines player and rumble routing.
    fakePads[3].sticks[1].y=20000; fakePads[3].buttons=1ULL<<2;
    pads.update(); assert(pads.input(LEFT_PLAYER).right && pads.input(LEFT_PLAYER).jump);
    pads.hit(LEFT_PLAYER); assert(vibrations.size()==1 && vibrations.back().id==3);
    fakeTicks=64; pads.update(); assert(vibrations.size()==1);
    fakeTicks=65; pads.update(); assert(vibrations.size()==2 && vibrations.back().amplitude==0);
    pads.setRumble(false); pads.hit(LEFT_PLAYER); assert(vibrations.size()==2);
    pads.setRumble(true); pads.hit(RIGHT_PLAYER); assert(vibrations.back().id==0);
    // A lost controller produces neutral input and stops its active pulse.
    fakePads[0].style=0; pads.update(); assert(!pads.ready(2));
    assert(!pads.input(RIGHT_PLAYER).jump && vibrations.back().amplitude==0);
    // Handheld is distinct from No1 and uses both rumble motors.
    pads.resetPlayers(); fakePads[32].style=HidNpadStyleTag_NpadHandheld;
    pads.update(); pads.ensurePlayer(LEFT_PLAYER); assert(pads.ready(1));
    auto before=vibrations.size(); fakeTicks=0xfffffff0u; pads.hit(LEFT_PLAYER);
    assert(vibrations.size()==before+2 && vibrations.back().id==32);
    fakeTicks=0x31; pads.update(); assert(vibrations.size()==before+4);
}
