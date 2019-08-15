cmake_minimum_required (VERSION 3.1)
project (heightmap)

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenCV REQUIRED)

add_executable(heightmap
               main.cpp
               volcano.h
               volcano.cpp
               volcanoDataSet.h
               volcanoDataSet.cpp
               PerlinNoise.h
               PerlinNoise.cpp
               utils.h
               utils.cpp)

target_compile_options(heightmap PUBLIC -O3 -fomit-frame-pointer -std=c++14 -I/usr/include -L/usr/lib -lnoise)
target_link_libraries(heightmap PUBLIC ${OpenCV_LIBS} -L/usr/lib -lnoise)
