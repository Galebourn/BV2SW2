#include "input_device/MenuNavigation.h"
#include <cassert>
#include <cstdint>

int main()
{
    using D = MenuNavigation::Direction;
    MenuNavigation nav;
    // Slight diagonal tilt must keep the intended vertical direction.
    assert(nav.update(15000, 28000, 0, 0) == D::Down);
    assert(nav.update(19000, 27000, 0, 50) == D::None);
    assert(nav.update(22000, 21000, 0, 349) == D::None);
    assert(nav.update(22000, 21000, 0, 350) == D::Down);
    assert(nav.update(15000, 28000, 0, 489) == D::None);
    assert(nav.update(15000, 28000, 0, 490) == D::Down);
    assert(nav.update(0, 0, 0, 500) == D::None);
    assert(nav.update(-15000, -28000, 0, 510) == D::Up);
    // Deliberate horizontal movement still adjusts menu values.
    assert(nav.update(-30000, -1000, 0, 520) == D::Left);
    assert(nav.update(30000, 1000, 0, 530) == D::Right);
    assert(nav.update(0, 0, 0, 540) == D::None);
    assert(nav.update(11000, 11000, 0, 550) == D::None);
    // D-pad overrides a tilted stick; each cardinal direction is distinct.
    assert(nav.update(30000, 0, MenuNavigation::Down, 560) == D::Down);
    assert(nav.update(30000, 0, MenuNavigation::Up, 570) == D::Up);
    assert(nav.update(0, 30000, MenuNavigation::Left, 580) == D::Left);
    assert(nav.update(0, 30000, MenuNavigation::Right, 590) == D::Right);
    assert(nav.update(0, 0, MenuNavigation::Up | MenuNavigation::Down, 600) == D::None);
    // Dead-zone hysteresis prevents extra presses around the activation edge.
    assert(nav.update(0, 12500, 0, 610) == D::Down);
    assert(nav.update(0, 11000, 0, 620) == D::None);
    assert(nav.update(0, 7000, 0, 630) == D::None);
    assert(nav.update(0, 12500, 0, 640) == D::Down);
    // Unsigned timing also handles SDL's tick counter wrapping.
    MenuNavigation wrap;
    const std::uint32_t nearWrap = UINT32_MAX - 100;
    assert(wrap.update(0, 30000, 0, nearWrap) == D::Down);
    assert(wrap.update(0, 30000, 0, 249) == D::Down);
}
