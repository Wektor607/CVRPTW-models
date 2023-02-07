import numpy as np
import itertools

class Distancer:
    def __init__(self):
        pass

    def distance(self, points: np.ndarray) -> np.ndarray:
        raise NotImplementedError


class EuclidianDistancer(Distancer):
    def __init__(self):
        super(EuclidianDistancer, self).__init__()

    def distance(self, points: np.ndarray) -> np.ndarray:
        return np.sqrt(np.sum((points[0] - points[1])**2))

    def distance_v2(self, x1, x2):
        return np.sqrt(np.sum((x1 - x2) ** 2))

    def distance_matrix(self, points):
        n = len(points)
        indices = itertools.product(list(range(n)), list(range(n)))
        result = np.zeros(shape=(n, n), dtype=float)
        for pair in indices:
            result[pair] = self.distance_v2(points[pair[0]], points[pair[1]])
        return result

    def batched_distance(self, points: np.ndarray) -> np.ndarray:
        return np.sqrt(np.sum((points[:, 0] - points[:, 1])**2, axis=1))

    def batched_distance_matrix(self, points):
        m = len(points)
        n = len(points[0])
        result = np.zeros(shape=(m, n, n), dtype=float)
        for i in range(m):
            indices = itertools.product(list(range(n)), list(range(n)))
            for pair in indices:
                result[i][pair] = self.distance_v2(points[i][pair[0]], points[i][pair[1]])
        return result


if __name__ == '__main__':
    a = EuclidianDistancer()
    b = np.array([[1, 2], [3, 4]])
    print(a.batched_distance_matrix(b[np.newaxis, ...]))