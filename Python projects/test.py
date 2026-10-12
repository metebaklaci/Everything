import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import convolve2d

# -----------------------
# Input matrix
# -----------------------
A = np.array([
    [ 1,  0,  0,  0,  1,  0, -1],
    [-1,  0,  0, -1,  1,  0,  1],
    [ 1,  1,  0, -1,  1, -1,  1],
    [ 1,  1,  0, -1, -1,  1,  1],
    [-1, -1, -1,  1, -1,  1, -1],
    [-1,  1,  1, -1, -1,  1, -1]
])

# -----------------------
# Vertical convolution kernel
# -----------------------
kernel = np.array([
    [0, 0, 0, 0],
    [1, 1, 1, 1],
    [0, 0, 0, 0]
])

# Convolution (same size as input)
conv = convolve2d(A, kernel, mode="same", boundary="fill", fillvalue=0)

# -----------------------
# Visualization
# -----------------------
fig, axes = plt.subplots(1, 2, figsize=(12, 5))

titles = ["Original Matrix", "After Convolution"]
matrices = [A, conv]

for ax, M, title in zip(axes, matrices, titles):

    vmax = np.abs(M).max()

    im = ax.imshow(
        M,
        cmap="RdBu_r",
        vmin=-vmax,
        vmax=vmax
    )

    ax.set_title(title)
    ax.set_xticks(range(M.shape[1]))
    ax.set_yticks(range(M.shape[0]))

    # Draw grid
    ax.set_xticks(np.arange(-.5, M.shape[1], 1), minor=True)
    ax.set_yticks(np.arange(-.5, M.shape[0], 1), minor=True)
    ax.grid(which="minor", color="black", linewidth=1)
    ax.tick_params(which="minor", bottom=False, left=False)

    # Write values
    for i in range(M.shape[0]):
        for j in range(M.shape[1]):
            ax.text(j, i, f"{M[i,j]:.0f}",
                    ha="center", va="center",
                    fontsize=11)

    plt.colorbar(im, ax=ax, shrink=0.8)

plt.tight_layout()
plt.show()