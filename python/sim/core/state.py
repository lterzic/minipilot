import numpy as np
from dataclasses import dataclass

@dataclass
class State:
    # Position in inertial frame (m)
    position: np.ndarray = np.zeros(3)
    # Velocity in inertial frame (m/s)
    velocity: np.ndarray = np.zeros(3)
    # Quaternion (w, x, y, z) - inertial to body frame
    quaternion: np.ndarray = np.array([1.0, 0, 0, 0])
    # Angular velocity in body frame (rad/s)
    angular_velocity: np.ndarray = np.zeros(3)

    def __post_init__(self):
        # Normalize the quaternion
        self.quaternion /= np.linalg.norm(self.quaternion)

    def to_array(self) -> np.ndarray:
        """Convert state to array for integration"""
        return np.concatenate([
            self.position,
            self.velocity,
            self.quaternion,
            self.angular_velocity
        ])
    
    @staticmethod
    def from_array(arr: np.ndarray) -> 'State':
        """Create state from array"""
        return State(
            arr[0:3],
            arr[3:6],
            arr[6:10],
            arr[10:13]
        )
