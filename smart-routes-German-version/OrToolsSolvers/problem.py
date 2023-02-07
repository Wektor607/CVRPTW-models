from pydantic import BaseModel


class Problem(BaseModel):
    capacity: bool = False,
    time_windows: bool = False,
    pdp: bool = False,
    dist_prob_type = 'const'


if __name__ == '__main__':
    rp = Problem()
    print(rp.dict())
