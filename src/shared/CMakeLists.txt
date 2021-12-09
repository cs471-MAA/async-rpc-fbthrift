
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(
    ServerStatsEncoder

    ServerStatsEncoder.cpp
)

target_link_libraries(
    ServerStatsEncoder
)