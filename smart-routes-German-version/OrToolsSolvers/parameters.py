from pydantic import BaseModel
import numpy as np
import typing


class GenerationParameters(BaseModel):
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
    rnds: typing.Any = None





if __name__ == '__main__':
    GP = GenerationParameters()
    print(GP.dict())
    print(GP.mu)
