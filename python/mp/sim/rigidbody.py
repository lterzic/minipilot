from mp.binds.math import matrix16f, vector3f
from mp.binds.state import state_s, sensors_s, ekf, custom_model
import numpy as np

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
        self.state = state

        self.force: np.ndarray = np.zeros(3)
        self.torque: np.ndarray = np.zeros(3)
        model_lin_acc = lambda v, a, q: vector3f(self.force / self.props.mass)
        model_ang_acc = lambda v, w, q: vector3f(self.props.inertia_inv @ (self.torque - np.cross(w.get_base(), self.props.inertia @ w.get_base())))
        self.model = custom_model(model_lin_acc, model_ang_acc, lambda: matrix16f(0,0,0,0,0))
        self.ekf = ekf(self.model)

    def update(self, dt, force, torque):
        self.force = force
        self.torque = torque

        return self.ekf.update(dt, sensors_s())


    