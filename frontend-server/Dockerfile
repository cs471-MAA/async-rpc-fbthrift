FROM python:3.9-slim

# install deps
RUN pip install flask

RUN mkdir -p /app
WORKDIR /app

CMD ["python3", "server.py"]