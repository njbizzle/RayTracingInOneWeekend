cmake_minimum_required(VERSION 3.5.0)
project(RayTracingInOneWeekend VERSION 0.1.0 LANGUAGES C CXX)

add_executable(RayTracingInOneWeekend main.cpp)

set_target_properties(RayTracingInOneWeekend PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

include(CTest)
enable_testing()
