cmake_minimum_required(VERSION 3.22.0)
project(zxshady_optional LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()

set(INCS "")
file(GLOB_RECURSE INCS "tombstone_optional/include/*.hpp")


add_library(zxshady_optional INTERFACE ${INCS})
add_library(ZXShady::Optional ALIAS zxshady_optional)
target_compile_features(zxshady_optional INTERFACE cxx_std_20)

if (MSVC)
    target_compile_options(zxshady_optional INTERFACE /Zc:__cplusplus)
endif()

target_include_directories(zxshady_optional INTERFACE
    tombstone_optional/include
)


option(ZXSHADY_OPTIONAL_BUILD_TESTS "Tests for this `optional` library" OFF)

if(ZXSHADY_OPTIONAL_BUILD_TESTS)
  add_subdirectory(tests)
endif()
