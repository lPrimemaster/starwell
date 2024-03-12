import numpy as np


def main():
    X = []
    V = []
    C = []

    # G1 ring
    for i in range(500):
        X.append((200*np.sin(i) - 700, 0, 200*np.cos(i)))
        V.append((150*np.sin(i + np.pi/2), 0, 150*np.cos(i + np.pi/2) + 150))
        C.append((255, 0, 0, 255))

    # G1 center
    for i in range(1000):
        X.append((100*np.sin(i) - 700, 0, 100*np.cos(i)))
        V.append((50*np.sin(i + np.pi/2), 0, 50*np.cos(i + np.pi/2) + 150))
        C.append((255, 0, 0, 255))

    # G2 ring
    for i in range(500):
        X.append((200*np.sin(i) + 700, 0, 200*np.cos(i)))
        V.append((150*np.sin(i + np.pi/2), 0, 150*np.cos(i + np.pi/2) - 150))
        C.append((0, 255, 0, 255))

    # G2 center
    for i in range(1000):
        X.append((100*np.sin(i) + 700, 0, 100*np.cos(i)))
        V.append((50*np.sin(i + np.pi/2), 0, 50*np.cos(i + np.pi/2) - 150))
        C.append((0, 255, 0, 255))

    # G3 ring
    for i in range(500):
        X.append((200*np.sin(i), 0, 200*np.cos(i) - 700))
        V.append((150*np.sin(i + np.pi/2) - 150, 0, 150*np.cos(i + np.pi/2)))
        C.append((0, 0, 255, 255))

    # G3 center
    for i in range(1000):
        X.append((100*np.sin(i), 0, 100*np.cos(i) - 700))
        V.append((50*np.sin(i + np.pi/2) - 150, 0, 50*np.cos(i + np.pi/2)))
        C.append((0, 0, 255, 255))

    # G4 ring
    for i in range(500):
        X.append((200*np.sin(i), 0, 200*np.cos(i) + 700))
        V.append((150*np.sin(i + np.pi/2) + 150, 0, 150*np.cos(i + np.pi/2)))
        C.append((255, 255, 0, 255))

    # G4 center
    for i in range(1000):
        X.append((100*np.sin(i), 0, 100*np.cos(i) + 700))
        V.append((50*np.sin(i + np.pi/2) + 150, 0, 50*np.cos(i + np.pi/2)))
        C.append((255, 255, 0, 255))

    return X, V, C
