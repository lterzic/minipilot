import numpy as np
from abc import ABC, abstractmethod
from sim.core.state import State

class Sensor(ABC):
    def __init__(self,
                 period: float,
                 dimension: int,
                 noise_density: float = 0.0,
                 bias: np.ndarray = 0.0):
        self.period = period
        self.dimension = dimension
        self.noise_std = noise_density * np.sqrt(period)
        self.bias = bias

        self.last_sample_time = 0.0
        self.last_sample = np.zeros(dimension)
    
    def update(self, state: State, t: float) -> None:
        if t < self.last_sample_time + self.period:
            return
        
        awgn = np.random.normal(0, self.noise_std, self.dimension)
        self.last_sample = self.derive(state, t - self.last_sample_time) + self.bias + awgn
        self.last_sample_time = t

    def read(self) -> np.ndarray:
        return self.last_sample

    @abstractmethod
    def derive(self, state: State, dt: float) -> np.ndarray:
        """Derive the sensor measurement based on the state without noise"""
        pass