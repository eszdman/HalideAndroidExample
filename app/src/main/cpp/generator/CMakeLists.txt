cmake_minimum_required(VERSION 3.22.1)

project(halideGenerators)

set(CMAKE_CXX_STANDARD 17)

set(HALIDE_TARGET arm-64-android-arm_dot_prod-arm_fp16)

find_package(Halide REQUIRED)

add_halide_generator(sampleGenerator SOURCES sampleGenerator.cpp)

add_halide_library(sample
                   FROM sampleGenerator
                   GENERATOR sample
                   TARGETS ${HALIDE_TARGET}
                   AUTOSCHEDULER Halide::Mullapudi2016
)

add_library(halideGenerators dummy.cpp)
add_dependencies(halideGenerators sample sample.runtime)