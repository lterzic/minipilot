#pragma once

#include "math.hpp"

namespace mp {

// Global (inertial) coordinate system

inline const vector3f NORTH     = {1, 0, 0};
inline const vector3f EAST      = {0, 1, 0};
inline const vector3f DOWN      = {0, 0, 1};
inline const vector3f SOUTH     = -NORTH;
inline const vector3f WEST      = -EAST;
inline const vector3f UP        = -DOWN;

// Local (body) coordinate system

inline auto& FORWARD    = NORTH;
inline auto& BACKWARD   = SOUTH;
inline auto& RIGHT      = EAST;
inline auto& LEFT       = WEST;

// Gravity const G = 9.80665
inline constexpr float G = 9.80665f;
// Gravity vector = G * DOWN
inline const vector3f GV = DOWN * G;

}