import numpy as np


def main():
    X = []
    V = []
    C = []

    for i in range(500):
        X.append((500*np.sin(i), 0, 500*np.cos(i)))
        V.append((150*np.sin(i + np.pi/2), 0, 150*np.cos(i + np.pi/2)))
        C.append((255, 0, 0, 255))

    for i in range(500):
        X.append((550*np.sin(i), 0, 550*np.cos(i)))
        V.append((200*np.sin(i + np.pi/2), 0, 200*np.cos(i + np.pi/2)))
        C.append((0, 255, 0, 255))

    for i in range(1000):
        X.append((100*np.sin(i), 100*np.cos(i), 0*np.cos(i)))
        V.append((50*np.sin(i + np.pi/2), 50*np.cos(i + np.pi/2), 0*np.cos(i + np.pi/2)))
        C.append((0, 0, 255, 255))

    return X, V, C
