cmake_minimum_required(VERSION 3.16)
project(bcfab VERSION 0.9.0)
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
#include(FetchSavvyDependency.cmake)
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

set(CMAKE_CXX_STANDARD 20)
# FetchContent_Declare(
#   savvy
#   GIT_REPOSITORY https://github.com/CreRecombinase/savvy.git
#   GIT_TAG vendor_shrinkwrap
#   )
# FetchContent_MakeAvailable(savvy)
FetchContent_Declare(
  savvy
  GIT_REPOSITORY https://github.com/CreRecombinase/savvy.git
  GIT_TAG vendor_shrinkwrap
  )

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  )

FetchContent_Declare(
  rangev3
  GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
  )

FetchContent_MakeAvailable(fmt rangev3 savvy)



add_executable(make_header src/make_header.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp)
add_executable(tsv2vcf src/tsv2vcf.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp)
add_executable(iter_sav src/iter_sav.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp include/get_variants.hpp)
target_link_libraries(iter_sav PRIVATE savvy fmt::fmt range-v3)
target_include_directories(iter_sav PUBLIC PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)

add_executable(sample_sav src/sample_sav.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp include/get_variants.hpp)
target_link_libraries(sample_sav PRIVATE savvy fmt::fmt range-v3)
target_include_directories(sample_sav PUBLIC PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)



target_link_libraries(make_header PRIVATE savvy fmt::fmt)
target_link_libraries(tsv2vcf PRIVATE savvy fmt::fmt)
#target_include_directories(tsv2vcf ${CMAKE_CURRENT_SOURCE_DIR}/include)
