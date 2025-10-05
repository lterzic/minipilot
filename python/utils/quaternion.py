import numpy as np

def q_mult(q1, q2):
    w1, x1, y1, z1 = q1
    w2, x2, y2, z2 = q2
    return np.array([
        w1*w2 - x1*x2 - y1*y2 - z1*z2,
        w1*x2 + x1*w2 + y1*z2 - z1*y2,
        w1*y2 - x1*z2 + y1*w2 + z1*x2,
        w1*z2 + x1*y2 - y1*x2 + z1*w2
    ])

def q_conj(q):
    w, x, y, z = q
    return np.array([w, -x, -y, -z])

def q_rot(q, v):
    vq = np.array([0.0, v[0], v[1], v[2]])
    # q * v * q^{-1}
    return q_mult(q_mult(q, vq), q_conj(q))[1:]

def q_normalized(q):
    return q / np.linalg.norm(q)