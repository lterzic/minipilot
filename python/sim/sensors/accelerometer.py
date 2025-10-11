import numpy as np
from sim.core.constants import *
from sim.core.state import State
from sim.sensors.sensor import Sensor
from utils.quaternion import *

class Accelerometer(Sensor):
    def __init__(self, period, noise_density = 0, bias = 0):
        super().__init__(period, 3, noise_density, bias)
        
        self.last_velocity = np.zeros(self.dimension)

    def derive(self, state: State, dt: float):
        # Body acceleration in inertial coordinates
        acc_pure = (state.velocity - self.last_velocity) / dt
        # Body coordinates minus the gravity vector
        acc_read = q_rot(state.quaternion, acc_pure - GRAVITY * DOWN)
        
        self.last_velocity = state.velocity
        return acc_read