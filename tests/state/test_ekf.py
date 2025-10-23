import mpbind

sm = mpbind.model_simple(0, 0)
ekf = mpbind.ekf(sm)

s = mpbind.sensors_s()
s.accelerometer = (mpbind.vector3f(0, 0, 9.81), 0)
s.gyroscope = (mpbind.vector3f(0, 0, 0), 0)

import pdb
pdb.set_trace()