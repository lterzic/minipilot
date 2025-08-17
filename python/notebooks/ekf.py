# ---
# jupyter:
#   jupytext:
#     formats: py:percent
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.17.2
#   kernelspec:
#     display_name: Python 3 (ipykernel)
#     language: python
#     name: python3
# ---

# %%
import numpy as np
import matplotlib.pyplot as plt

from src.state import *

# %%
# Assuming a perfect (no noise and bias) accelerometer, during
# free-fall the accelerometer would return 0 on all axes (if no rotation),
# therefore the read acceleration is the actual acceleration - gravity
# mapped to the local coordinate frame
a_exp = rot_v(a - gv, q.inverse())

# %%
a_exp.jacobian(a)

# %%
a_exp.jacobian(qv)

# %%
