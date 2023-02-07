from smart_routes.algorithm import Algorithm
from smart_routes.dataset import Dataset

from config import load_pkl, train_parser
from train import train


class JAMPR(Algorithm):

    def __init__(self):
        self.name = 'jampr'

    def fit(self, data: Dataset):
        cfg = load_pkl(train_parser().path)
        train(cfg, data)

    def predict(self):
        pass
