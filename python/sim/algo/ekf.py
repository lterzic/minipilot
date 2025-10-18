from typing import Callable
import numpy as np

EKFCallback = Callable[[np.ndarray], np.ndarray]

class EKF:
    def __init__(self, state_dim: int):
        self.state = np.zeros(state_dim)
        self.state_predict = np.zeros(state_dim)
        self.p = np.zeros((state_dim, state_dim))
        self.p_predict = np.zeros((state_dim, state_dim))

    def predict(self, f: EKFCallback, F: EKFCallback, Q: np.ndarray) -> None:
        Fj = F(self.state)

        self.state_predict = f(self.state)
        self.p_predict = Fj @ self.p @ Fj.transpose() + Q

    def update(self,
               h: EKFCallback,
               H: EKFCallback,
               R: np.ndarray,
               obs: np.ndarray):
        divr = lambda A, B: np.linalg.solve(B.T, A.T).T

        Hj = H(self.state_predict)
        Hjt = Hj.transpose()

        diff = obs - h(self.state_predict)
        cov = Hj @ self.p_predict @ Hjt + R
        gain = divr(self.p_predict @ Hjt, cov)

        self.state = self.state_predict + gain @ diff
        self.p = self.p_predict - gain @ Hj @ self.p_predict

    def get_state(self):
        return self.state
