FROM saheru/fbthrift:latest

RUN mkdir -p /app && cd /app
COPY . /app

# compile and install code
RUN cd /app/src \
    && mkdir -p build \
    && cd build \
    && cmake .. \
    && make -j$(nproc) \
    && make install
