FROM saheru/fbthrift:latest

RUN mkdir -p /app 

WORKDIR /app
COPY . /app

RUN cd /app/src \
    && mkdir -p build \
    && cd build \
    && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. \
    && make -j$(nproc)
