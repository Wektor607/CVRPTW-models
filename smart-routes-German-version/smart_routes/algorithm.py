import typing

import numpy as np
from pydantic import BaseModel

from OrToolsSolvers.instance import Instance
from smart_routes.dataset import Dataset

names = typing.Literal['jampr', 'or-tools', 'lkh']


class Algorithm(BaseModel):
    name: names = 'jampr'

    def predict(self, data: typing.Union[typing.List[Instance], Instance, None]) -> np.ndarray:
        raise NotImplementedError

    def fit(self, data: Dataset):
        raise NotImplementedError


if __name__ == '__main__':
    from OrToolsSolvers.problem import Problem
    from OrToolsSolvers.parameters import GenerationParameters

    gp = GenerationParameters()
    rp = Problem()
    ins = Instance(problem=rp, generation_parameters=gp, coordinates=np.array([[0, 1]]))
    alg = Algorithm()
    alg.predict(ins)
