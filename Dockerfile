FROM saheru/fbthrift:latest

RUN mkdir -p /app 

WORKDIR /app
COPY . /app

RUN cd /app/src \
    && mkdir -p build \
    && cd build \
    && cmake .. \
    && make
