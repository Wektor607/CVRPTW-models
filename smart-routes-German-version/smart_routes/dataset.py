import typing
from pydantic import BaseModel

from problem import Problem
from instance import Instance
from parameters import GenerationParameters
from smart_routes.utils.generator import generate_batch


class Dataset(BaseModel):
    problem: Problem
    generation_parameters: GenerationParameters = GenerationParameters()
    instances: typing.List[Instance] = None
    generated: bool = False

    def generate_instances(self, batch_size=None):
        self.generated = True
        if batch_size is None:
            batch_size = self.generation_parameters.n_samples
        result = generate_batch(self.problem, batch_size, self.generation_parameters)
        self.instances = [Instance(generation_parameters=self.generation_parameters, **dict(zip(result, t))) for t in
                          zip(*result.values())]

    def visualize(self):
        raise NotImplementedError


if __name__ == '__main__':
    pr = Problem()
    dataset = Dataset(problem=pr)
    print(dataset.dict())
    dataset.generate_instances(2)
    print(dataset.dict())
