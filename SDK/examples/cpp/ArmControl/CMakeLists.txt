cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example main.cpp)

# Add include dirs
target_include_directories(example PRIVATE
  /.doly/libs/sdk/include
  /.doly/libs/spdlog/include
)

# Add link dirs
target_link_directories(example PRIVATE
	/.doly/libs/sdk/lib
  /.doly/libs/spdlog/lib/
)

# Link library
target_link_libraries(example PRIVATE
  ArmControl
	ServoMotor
	Analog
  Gpio
	Helper
  spdlog
  pthread
)
