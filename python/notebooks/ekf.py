# ---
# jupyter:
#   jupytext:
#     formats: ipynb,py:percent
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.18.1
#   kernelspec:
#     display_name: Python 3 (ipykernel)
#     language: python
#     name: python3
# ---

# %%
from mp.utils.symbolic import *
import sympy as sy

sy.init_printing()

# %% [markdown]
# ## Derivations
# ### State transition

# %%
dt = sy.symbols("dt")

# %%
v_next = v + dt * a
v_next.jacobian(v), v_next.jacobian(a)

# %%
b = sympy.Matrix([[0, -wx, -wy, -wz], [wx, 0, wz, -wy], [wy, -wz, 0, wx], [wz, wy, -wx, 0]])
qv_next = qv + (dt/2) * (b * qv)

# %%
qv_next.jacobian(qv), qv_next.jacobian(w)

# %% [markdown]
# ### Observations
# #### Accelerometer

# %% [markdown]
# Expected accelerometer reading is the body's acceleration in the inertial frame relative to the body in free-fall. Therefore, at rest, the accelerometer will display the acceleration due to gravity upwards. Since the reading is provided in the body frame, the final expected vector, assuming no noise and bias, is $\vec{a}_{exp} = (\vec{a} - \vec{g})^{q^*}$.

# %%
a_exp = rot_v(a - g*DOWN, q.inverse())

# %%
a_exp.jacobian(a), sy.simplify(a_exp.jacobian(qv))

# %% [markdown]
# ## Implementation testing

# %%
import numpy as np
import matplotlib.pyplot as plt
from mp.binds.state import *


# %%
def simulate(estimator, sensors, end, dt = 0.01):
    states = [state_s()]
    t = 0
    while t < end:
        states.append(estimator.update(dt, sensors(states[-1], t)))
        t += dt
    return np.arange(0, end, dt), states[:-2]


# %%
# Based on LSM6DSO datasheet, in SI units
acc_nd = 1.1e-3
gyro_nd = 7e-5
imu_fs = 204

acc_std = np.sqrt((acc_nd ** 2) * imu_fs)
gyro_std = np.sqrt((gyro_nd ** 2) * imu_fs)

# %%
q_matrix = lambda v, a, q, w, wd: np.diag(np.array([v,v,v,a,a,a,q,q,q,q,w,w,w,wd,wd,wd], dtype=np.float32))

model_acc = lambda v, a, q: a * 0.98
model_ang = lambda v, w, q: w * -0.8
model_q = lambda: q_matrix(1, 8e-2, 1e-4, 1e-8, 2e-10)
cm = custom_model(model_acc, model_ang, model_q)
est = ekf(cm)

def sensors_f(state, t):
    acc_sim = np.array([0, 0, -9.81 - 0.7 * (t > 5)]) + np.random.randn(3) * acc_std
    gyro_sim = np.array([0, 0, 0.1 * (t > 2)]) + np.random.randn(3) * gyro_std
    
    s = sensors_s()
    s.accelerometer = (acc_sim, acc_nd)
    s.gyroscope = (gyro_sim, gyro_nd)
    return s
t, res = simulate(est, sensors_f, 10)

# %%
acc = np.array([s.acceleration for s in res])
vel = np.array([s.velocity for s in res])
rot = np.array([s.rotationq.as_vector() for s in res])
ang = np.array([s.angular_velocity for s in res])
drift = np.array([s.gyroscope_drift for s in res])

fig, ax = plt.subplots(2, 2, figsize=[12, 9])

ax[0,0].plot(t, acc[:, 0], label="x")
ax[0,0].plot(t, acc[:, 1], label="y")
ax[0,0].plot(t, acc[:, 2], label="z")
ax[0,0].legend()

ax[0,1].plot(t, rot[:, 1], label="x")
ax[0,1].plot(t, rot[:, 2], label="y")
ax[0,1].plot(t, rot[:, 3], label="z")
ax[0,1].legend()

ax[1,0].plot(t, ang[:, 0], label="x")
ax[1,0].plot(t, ang[:, 1], label="y")
ax[1,0].plot(t, ang[:, 2], label="z")
ax[1,0].legend()

ax[1,1].plot(t, drift[:, 0], label="x")
ax[1,1].plot(t, drift[:, 1], label="y")
ax[1,1].plot(t, drift[:, 2], label="z")
ax[1,1].legend()

# %%

# %%
