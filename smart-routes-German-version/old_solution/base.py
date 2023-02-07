"""
Base classes
"""
import numpy as np
from typing import List, Any
from dataclasses import dataclass


@dataclass
class Base:
    @property
    def json(self) -> dict:
        result = {}
        for item in self.__annotations__:
            value = getattr(self, item)
            if hasattr(value, "json"):
                value = value.json
            result[item] = value
        return result


@dataclass
class RouteInstance(Base):
    # constraints basically optional
    coordinates: np.ndarray
    distance_matrix: np.ndarray = None
    vehicle_capacities: np.ndarray = None
    nodes_demands: np.ndarray = None
    time_windows: np.ndarray = None
    full_time_windows: np.ndarray = None  # working day time windows
    service_durations: np.ndarray = None
    time_factors: np.ndarray = None
    #
    fleet_size: int = 10
    active_vehicle_number: int = 3
    depot_num: int = 1


@dataclass
class Dataset(Base):
    instances: List[RouteInstance]
    generated: bool = True


@dataclass
class Solution(Base):
    algorithm: str
    cost: List[float]
    data: Dataset
    routes: List[np.ndarray]
    metrics: dict

    metric_aggr: List[str] = None
    graphs_path: str = None


@dataclass
class GenerationParameters(Base):
    mu: float = 15
    std: float = 10
    capacity_min: float = 1
    capacity_max: float = 42
    n_samples: int = 1000
    n_customer: int = 20
    service_window: int = 1000
    service_duration: int = 10
    time_factor: float = 100.0
    tw_expansion: float = 3.0
    depot_num: int = 1
    rnds: Any = None


if __name__ == '__main__':
    pass
