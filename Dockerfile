FROM saheru/fbthrift:latest

RUN mkdir -p /app && cd /app
COPY . /app

# compile and install code
RUN cd /app/src \
    && mkdir -p build \
    && cd build \
    && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. \
    && make -j$(nproc)
