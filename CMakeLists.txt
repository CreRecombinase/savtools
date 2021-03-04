cmake_minimum_required(VERSION 3.16)
project(bcfab VERSION 0.9.0)
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
FetchContent_Declare(
  savvy
  GIT_REPOSITORY https://github.com/CreRecombinase/savvy.git
  GIT_TAG vendor_shrinkwrap
  )
FetchContent_MakeAvailable(savvy)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  )
FetchContent_MakeAvailable(fmt)

add_executable(make_header src/make_header.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp)
add_executable(tsv2vcf src/tsv2vcf.cpp src/vcf_header.hpp src/seqnames.hpp src/cxxopts.hpp)
target_link_libraries(make_header PRIVATE savvy fmt::fmt)
target_link_libraries(tsv2vcf PRIVATE savvy fmt::fmt)
#target_include_directories(tsv2vcf ${CMAKE_CURRENT_SOURCE_DIR}/include)
