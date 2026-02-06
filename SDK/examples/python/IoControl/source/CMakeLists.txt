cmake_minimum_required(VERSION 3.18)
project(doly_tts_py LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)

pybind11_add_module(doly_io bindings.cpp)

# Add include dirs
target_include_directories(doly_io PRIVATE
    /.doly/libs/sdk/include
)

# Add link dirs
target_link_directories(doly_io PRIVATE
  /.doly/libs/sdk/lib
)

# Link library
target_link_libraries(doly_io PRIVATE
   	IoControl
    Helper
    Threads::Threads
)

install(TARGETS doly_io LIBRARY DESTINATION .)
