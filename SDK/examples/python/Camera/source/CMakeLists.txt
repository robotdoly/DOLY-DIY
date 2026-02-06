cmake_minimum_required(VERSION 3.18)
project(doly_color_py LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)

pybind11_add_module(doly_camera bindings.cpp)

# Add include dirs
target_include_directories(doly_camera PRIVATE
    /.doly/libs/sdk/include
    /.doly/libs/opencv/include/opencv4/
    /usr/include/libcamera/
)

# Add link dirs
target_link_directories(doly_camera PRIVATE
  /.doly/libs/sdk/lib
  /.doly/libs/opencv/lib
)

# Link library
target_link_libraries(doly_camera PRIVATE
	LCCV
  camera
  camera-base
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)

install(TARGETS doly_camera LIBRARY DESTINATION .)
