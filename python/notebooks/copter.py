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
from mp.sim.rigidbody import *
from mp.utils.numeric import *

# %%
copter_mass = 1.2
copter_inertia = np.diag([0.01, 0.01, 0.02])
copter_lin_drag = 0.4
copter_ang_drag = 5e-3
motor_tc = 0.1


# %%
def simulate(model_f, end, dt = 0.01):
    states = [state_s()]
    states[-1].acceleration = GV
    
    rb_props = RigidbodyProperties(copter_mass, copter_inertia)
    rb = Rigidbody(rb_props, states[-1])
    
    t_arr = np.arange(0, end, dt)
    for t in t_arr:
        state = states[-1]
        force, torque = model_f(state, t, dt)
        rb.update(dt, force, torque)
        states.append(rb.get_state())
    return t_arr, states[:-1]


# %%
#pid_params = copter_controller_pid_params_s(20, 8, 1.4, 3, 1, 0.2, 12)
pid_params = copter_controller_pid_params_s(12, 4, 1.4, 16, 10, 1.2, 2.2, 0, 1.6)
copter_params = copter_params_s(copter_mass * 1.1, copter_inertia * 1.2, copter_lin_drag, copter_ang_drag)
pid = copter_controller_pid(pid_params, copter_params)

thrust_tf = tf1(motor_tc)
torque_tf = tf1(motor_tc)

def copter_model(state, t, dt):
    # controls = angular_controls_s(np.array([0.5 * (t > 1), -0.7 * (t > 2), 0]), 0)
    controls = linear_controls_s(np.array([5 * (t > 5 and t < 10), 0, 0]), 0)
    actuation = pid.update(controls, state, dt)
    thrust = thrust_tf.update(dt, actuation.thrust)
    torque = torque_tf.update(dt, actuation.torque)
    
    vel = state.velocity
    rot = state.rotation
    ang = state.angular_velocity

    copter_force = copter_mass * GV - copter_lin_drag * vel + thrust * rot.rotate_vector(UP)
    copter_torque = torque - ang * copter_ang_drag
    return copter_force, copter_torque

# with copter_ostream_redirect():
t, res = simulate(copter_model, 15)

# %%
acc = np.array([s.acceleration for s in res])
vel = np.array([s.velocity for s in res])
rot = np.array([s.rotation.as_vector() for s in res])
ang = np.array([s.angular_velocity for s in res])

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
