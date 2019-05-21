cmake_minimum_required(VERSION 3.1)
project(PMCAnalyzer)

add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE RELEASE)

find_package(Boost 1.56 REQUIRED COMPONENTS
             filesystem iostreams system)

add_executable(PMC_analyzer src/analyzer.cpp)

target_link_libraries(PMC_analyzer
  Boost::filesystem
  Boost::system
  Boost::iostreams
  )
