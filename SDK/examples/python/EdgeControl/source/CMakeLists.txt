cmake_minimum_required(VERSION 3.18)
project(doly_edge_py LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)

pybind11_add_module(doly_edge bindings.cpp)

# Add include dirs
target_include_directories(doly_edge PRIVATE
    /.doly/libs/sdk/include
)

# Add link dirs
target_link_directories(doly_edge PRIVATE
  /.doly/libs/sdk/lib
)

# Link library
target_link_libraries(doly_edge PRIVATE
   	EdgeControl
	Gpio
	Helper
    Threads::Threads
)

install(TARGETS doly_edge LIBRARY DESTINATION .)
