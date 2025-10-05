import numpy as np

# Direction vectors
FORWARD =   np.array([1, 0, 0])
LEFT =      np.array([0, 1, 0])
UP =        np.array([0, 0, 1])
RIGHT =     -LEFT
DOWN =      -UP
BACK =      -FORWARD

# Gravity constant
GRAVITY = 9.80665