import numpy as np
from sim.core.state import State
from sim.sensors.sensor import Sensor

class Gyroscope(Sensor):
    def __init__(self, period, noise_density = 0, bias = 0):
        super().__init__(period, 3, noise_density, bias)
        # Add drift

    def derive(self, state: State, t: float, dt: float):
        return state.angular_velocity