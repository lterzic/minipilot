import numpy as np

def rot_v(q, v):
    """
    Rotate 3D vector(s) v by unit quaternion(s) q.
    
    Parameters
    ----------
    q : array-like, shape (..., 4)
        Quaternion(s) in [w, x, y, z] format.
    v : array-like, shape (..., 3)
        Vector(s) to rotate. Must be broadcastable with q.
    
    Returns
    -------
    v_rot : ndarray, shape (..., 3)
        Rotated vector(s).
    """
    w = q[0]
    qv = q[1:]

    t = 2.0 * np.cross(qv, v)
    return v + (w * t) + np.cross(qv, t)