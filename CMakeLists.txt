cmake_minimum_required(VERSION 3.0)
project(OpenCV_Homework2)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add executable target
add_executable(main src/homework2.cpp)

# Link OpenCV libraries
target_link_libraries(main ${OpenCV_LIBS})
