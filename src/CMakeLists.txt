cmake_minimum_required(VERSION 3.16)
project(cs4212_project CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

# 1. Define utility library
add_library(cs4212-util
  ArgumentParsing.cpp
  ArgumentParsing.h
  Framebuffer.cpp
  Framebuffer.h
  color.h
  handleGraphicsArgs.cpp
  handleGraphicsArgs.h
  model_obj.cpp
  model_obj.h
  vec3.h
)

target_compile_definitions(cs4212-util PUBLIC HAS_GLM)

target_link_libraries(cs4212-util 
  PUBLIC 
    glm::glm
  PRIVATE 
    Boost::program_options
)

# 2. Define executable
add_executable(mainCode
  mainCode.cpp
)

target_link_libraries(mainCode
  PRIVATE
    cs4212-util
    PNG::PNG
    ZLIB::ZLIB
)