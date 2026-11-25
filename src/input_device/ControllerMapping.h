#pragma once
#include <cstdint>

// Raw Nintendo stick coordinates: Y is positive upwards. Single Joy-Cons
// are rotated in opposite directions, with their rail buttons on top.
enum class ControllerLayout { Full, LeftSingle, RightSingle };
struct ControllerFrame {
    int x = 0, y = 0; // screen coordinates, Y positive down
    unsigned dpad = 0;
    bool select = false, back = false, pause = false, join = false;
};
inline ControllerFrame mapController(ControllerLayout layout, int lx, int ly,
                                     int rx, int ry, std::uint64_t buttons) {
    auto pressed = [buttons](int bit) { return (buttons & (std::uint64_t(1) << bit)) != 0; };
    ControllerFrame out;
    if (layout == ControllerLayout::LeftSingle) {
        out.x = -ly; out.y = -lx;
        out.select = pressed(15); out.back = pressed(12);
        out.join = pressed(24) && pressed(25);
    } else if (layout == ControllerLayout::RightSingle) {
        out.x = ry; out.y = rx;
        out.select = pressed(2); out.back = pressed(0);
        out.join = pressed(26) && pressed(27);
    } else {
        out.x = lx; out.y = -ly;
        out.select = pressed(0); out.back = pressed(1);
        out.join = pressed(6) && pressed(7);
        for (int i=0;i<4;++i) if (pressed(12+i)) out.dpad |= 1u << i;
    }
    out.pause = pressed(10) || pressed(11);
    return out;
}
