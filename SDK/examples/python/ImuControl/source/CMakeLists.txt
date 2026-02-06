cmake_minimum_required(VERSION 3.18)
project(doly_imu_py LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)

pybind11_add_module(doly_imu bindings.cpp)

# Add include dirs
target_include_directories(doly_imu PRIVATE
    /.doly/libs/sdk/include
)

# Add link dirs
target_link_directories(doly_imu PRIVATE
  /.doly/libs/sdk/lib
)

# Link library
target_link_libraries(doly_imu PRIVATE
   	ImuControl
    Helper
    Threads::Threads
)

install(TARGETS doly_imu LIBRARY DESTINATION .)
