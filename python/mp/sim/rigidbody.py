import numpy as np
from mp.binds.state import state_s, quaternionf

class RigidbodyProperties:
    def __init__(self, mass: float = 1, inertia: np.ndarray = np.eye(3)) -> None:
        # Mass of the body in kilograms
        self.mass = mass
        # Moment of inertia matrix in kg*m^2
        self.inertia = inertia
        self.inertia_inv = np.linalg.inv(self.inertia)

class Rigidbody:
    """6 DOF rigidbody using RK4 integration"""

    def __init__(self, properties: RigidbodyProperties, state: state_s = state_s()) -> None:
        self.props = properties
        self.state = Rigidbody.state_from_mp(state)

    @staticmethod
    def state_from_mp(state: state_s) -> np.ndarray:
        return np.concatenate([
            state.position,
            state.velocity,
            state.acceleration,
            state.rotation.as_vector(),
            state.angular_velocity
        ])
    
    @staticmethod
    def state_from_py(state: np.ndarray) -> state_s:
        s = state_s()
        s.position = state[0:3]
        s.velocity = state[3:6]
        s.acceleration = state[6:9]
        s.rotation = quaternionf(*state[9:13])
        s.angular_velocity = state[13:16]
        return s

    @staticmethod
    def derivative(state: np.ndarray, lin_acc: np.ndarray, ang_acc: np.ndarray) -> np.ndarray:
        w1, w2, w3 = state[13:16]
        b = np.array([
            [0, -w1, -w2, -w3],
            [w1, 0, w3, -w2],
            [w2, -w3, 0, w1],
            [w3, w2, -w1, 0]
        ])

        dp_dt = state[3:6]
        dv_dt = state[6:9]
        da_dt = np.zeros(3)
        dq_dt = b @ (state[9:13] * 0.5)
        dw_dt = ang_acc

        return np.concatenate([dp_dt, dv_dt, da_dt, dq_dt, dw_dt])

    def update(self, dt: float, force: np.ndarray, torque: np.ndarray):
        lin_acc = force / self.props.mass
        self.state[6:9] = lin_acc
        def ang_acc(s):
            w = s[13:16]
            return self.props.inertia_inv @ (torque - np.cross(w, self.props.inertia @ w))

        s1 = self.state
        k1 = Rigidbody.derivative(s1, lin_acc, ang_acc(s1))

        s2 = s1 + 0.5 * dt * k1
        k2 = self.derivative(s2, lin_acc, ang_acc(s2))

        s3 = s2 + 0.5 * dt * k2
        k3 = self.derivative(s3, lin_acc, ang_acc(s3))

        s4 = s3 + dt * k3
        k4 = self.derivative(s4, lin_acc, ang_acc(s4))

        self.state = s1 + (dt / 6.0) * (k1 + 2*k2 + 2*k3 + k4)
        self.state[9:13] /= np.linalg.norm(self.state[9:13])

    def get_state(self) -> state_s:
        return Rigidbody.state_from_py(self.state)

    