import numpy as np

# Must match with constants.hpp
NORTH       = np.array([1, 0, 0], dtype=np.float32)
EAST        = np.array([0, 1, 0], dtype=np.float32)
DOWN        = np.array([0, 0, 1], dtype=np.float32)
SOUTH       = -NORTH
WEST        = -EAST
UP          = -DOWN
FORWARD     = NORTH
BACKWARD    = SOUTH
RIGHT       = EAST
LEFT        = WEST

G = 9.80665
GV = G * DOWN

class tf1:
    def __init__(self, a: float, y: float = 0):
        self.a = a
        self.y = y

    def update(self, dt: float, x: np.ndarray) -> np.ndarray:
        k = self.a / (self.a + dt)
        self.y = k * self.y + (1.0 - k) * x
        return self.y