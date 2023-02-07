FROM python:3.9-alpine

# RUN apt-get update && apt-get install -y build-essential
# RUN pip install --upgrade pip setuptools wheel twine
RUN pip install --upgrade pip
WORKDIR /opt/app

COPY ./requirements.txt .

RUN pip install -r requirements.txt
COPY . .
