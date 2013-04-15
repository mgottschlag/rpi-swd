
project(rpi-swd)
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual -std=c++0x")

set(SRC
	DebugPort.cpp
	DebugPort.hpp
	main.cpp
	MemoryAccessPort.cpp
	MemoryAccessPort.hpp
	STM32.cpp
	STM32.hpp
	SWD.cpp
	SWD.hpp
)

add_executable(rpi-swd ${SRC})
target_link_libraries(rpi-swd wiringPi)
