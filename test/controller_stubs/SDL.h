#pragma once
#include <cstdint>
static std::uint32_t fakeTicks=0;
inline std::uint32_t SDL_GetTicks() { return fakeTicks; }
