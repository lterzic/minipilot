import numpy as np
from dataclasses import dataclass
from sim.core.state import State

@dataclass
class RigidbodyProperties:
    # Mass of the body in kilograms
    mass: float = 1
    # Moment of inertia matrix in kg*m^2
    inertia: np.ndarray = np.eye(3)

    def __post_init__(self):
        self.inertia_inv = np.linalg.inv(self.inertia)

class Rigidbody:
    """6 DOF rigidbody using RK4 integration"""

    def __init__(self, properties: RigidbodyProperties, state: State = State()) -> None:
        self.props = properties
        self.state = state

    def derivative(self, force: np.ndarray, torque: np.ndarray) -> np.ndarray:
        w = self.state.angular_velocity
        w1, w2, w3 = w
        b = np.array([
            [0, -w1, -w2, -w3],
            [w1, 0, w3, -w2],
            [w2, -w3, 0, w1],
            [w3, w2, -w1, 0]
        ])

        dp_dt = self.state.velocity
        dv_dt = force / self.props.mass
        dq_dt = b @ (self.state.quaternion * 0.5)
        dw_dt = self.props.inertia_inv @ (torque - np.cross(w, self.props.inertia @ w))

        return np.concatenate([dp_dt, dv_dt, dq_dt, dw_dt])

    def step_rk4(self, dt: float, force: np.ndarray, torque: np.ndarray) -> State:
        state_arr = self.state.to_array()

        k1 = self.derivative(force, torque)
        self.state = State.from_array(state_arr + 0.5 * dt * k1)

        k2 = self.derivative(force, torque)
        self.state = State.from_array(state_arr + 0.5 * dt * k2)

        k3 = self.derivative(force, torque)
        self.state = State.from_array(state_arr + dt * k3)

        k4 = self.derivative(force, torque)
        self.state = State.from_array(state_arr + (dt / 6.0) * (k1 + 2*k2 + 2*k3 + k4))

        return self.state