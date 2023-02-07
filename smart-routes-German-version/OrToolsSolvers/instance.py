from problem import Problem
from pydantic import BaseModel
from parameters import GenerationParameters

import numpy as np
from typing import Optional


class Instance(BaseModel):
    problem: Problem
    generation_parameters: GenerationParameters
    coordinates: np.ndarray
    distance_matrix: Optional[np.ndarray]
    vehicle_capacities: Optional[np.ndarray]
    nodes_demands: Optional[np.ndarray]
    time_windows: Optional[np.ndarray]
    full_time_windows: Optional[np.ndarray]
    service_durations: Optional[np.ndarray]
    time_factors: Optional[np.ndarray]
    fleet_size: Optional[np.ndarray]
    depot_num: Optional[int]

    class Config:
        arbitrary_types_allowed = True

    def get_distance_matrix(self):
        raise NotImplementedError

    def set_distance_matrix(self, x):
        raise NotImplementedError


if __name__ == '__main__':
    import numpy as np

    gp = GenerationParameters()
    rp = Problem()

    ri = Instance(problem=rp, generation_parameters=gp, coordinates=np.array([[0, 1]]))
    print(ri.dict())
