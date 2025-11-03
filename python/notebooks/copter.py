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
import numpy as np
import matplotlib.pyplot as plt
from mp.binds.copter import *
from mp.binds.math import *
from mp.sim.rigidbody import *
from mp.utils.numeric import *

# %%
copter_mass = 1
copter_inertia = np.diag([0.01, 0.01, 0.02])
motor_delay = 0.1


# %%
def simulate(controller, input_f, end, dt = 0.01, motor_delay = 0):
    rb_props = RigidbodyProperties(copter_mass * 1.1, copter_inertia)
    rb = Rigidbody(rb_props)
    
    states = [state_s()]
    states[-1].acceleration = vector3f(0, 0, 9.80665)
    t = 0
    
    motor_coeff = dt / (dt + motor_delay)
    force = np.zeros(3)
    torque = np.zeros(3)
    while t < end:
        state = states[-1]
        actuation = controller.update(input_f(t), state, dt)
        
        force_v = actuation.thrust * rot_v(state.rotationq.as_vector(), np.array([0, 0, -1]))
        force = motor_coeff * force_v + (1 - motor_coeff) * force
        torque = motor_coeff * actuation.torque.get_base() + (1 - motor_coeff) * torque
        gravity = rb_props.mass * np.array([0, 0, 9.80665])
        states.append(rb.update(dt, force + gravity, torque))
        
        t += dt
    return np.arange(0, end, dt), states[:-2]


# %%
pid_params = copter_controller_pid_params_s(20, 8, 1.4, 3, 1, 0.2, 12)
copter_params = copter_params_s(copter_mass, matrix3f(copter_inertia), 0)
pid = copter_controller_pid(pid_params, copter_params)

input_f = lambda t: linear_controls_s(vector3f(t > 2, t > 6, 0), 0)
# with copter_ostream_redirect():
t, res = simulate(pid, input_f, 10, motor_delay=motor_delay)

# %%
acc = np.array([s.acceleration.get_base() for s in res])
vel = np.array([s.velocity.get_base() for s in res])
rot = np.array([s.rotationq.as_vector() for s in res])
ang = np.array([s.angular_velocity.get_base() for s in res])
drift = np.array([s.gyroscope_drift.get_base() for s in res])

fig, ax = plt.subplots(2, 2, figsize=[12, 9])

ax[0,0].plot(t, vel[:, 0], label="x")
ax[0,0].plot(t, vel[:, 1], label="y")
ax[0,0].plot(t, vel[:, 2], label="z")
ax[0,0].legend()

ax[0,1].plot(t, acc[:, 0], label="x")
ax[0,1].plot(t, acc[:, 1], label="y")
ax[0,1].plot(t, acc[:, 2], label="z")
ax[0,1].legend()

ax[1,0].plot(t, rot[:, 1], label="x")
ax[1,0].plot(t, rot[:, 2], label="y")
ax[1,0].plot(t, rot[:, 3], label="z")
ax[1,0].legend()

ax[1,1].plot(t, ang[:, 0], label="x")
ax[1,1].plot(t, ang[:, 1], label="y")
ax[1,1].plot(t, ang[:, 2], label="z")
ax[1,1].legend()

# %%

# %%
