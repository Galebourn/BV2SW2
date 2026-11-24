#pragma once

#include <cstdint>
#include <cstdlib>

// Menu-only stick filtering: gameplay continues to read the original axes.
class MenuNavigation
{
public:
    enum class Direction { None, Up, Down, Left, Right };
    enum Dpad { Left = 1, Up = 2, Right = 4, Down = 8 };

    Direction update(int x, int y, unsigned dpad, std::uint32_t now)
    {
        Direction next = Direction::None;
        const int ax = std::abs(x), ay = std::abs(y);
        if (dpad)
        {
            // Resolve diagonals to one direction; opposite buttons cancel.
            const int dy = !!(dpad & Down) - !!(dpad & Up);
            const int dx = !!(dpad & Right) - !!(dpad & Left);
            if (dy) next = dy > 0 ? Direction::Down : Direction::Up;
            else if (dx) next = dx > 0 ? Direction::Right : Direction::Left;
        }
        else
        {
            const int threshold = held == Direction::None ? 12000 : 8000;
            if (ax >= threshold || ay >= threshold)
            {
                // Keep the current axis near a diagonal to avoid wobbling
                // between menu navigation and changing a slider's value.
                bool vertical = ay >= ax;
                if ((held == Direction::Up || held == Direction::Down) &&
                    ay >= threshold && ax * 4 < ay * 5)
                    vertical = true;
                else if ((held == Direction::Left || held == Direction::Right) &&
                         ax >= threshold && ay * 4 < ax * 5)
                    vertical = false;
                next = vertical ? (y < 0 ? Direction::Up : Direction::Down)
                                : (x < 0 ? Direction::Left : Direction::Right);
            }
        }

        if (next != held)
        {
            held = next;
            lastPulse = now;
            repeating = false;
            return next;
        }
        if (held != Direction::None && now - lastPulse >= (repeating ? 140u : 350u))
        {
            lastPulse = now;
            repeating = true;
            return held;
        }
        return Direction::None;
    }

private:
    Direction held = Direction::None;
    std::uint32_t lastPulse = 0;
    bool repeating = false;
};
