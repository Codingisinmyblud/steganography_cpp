cmake_minimum_required(VERSION 3.14)
project(stegkit-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE REQUIRED)

add_library(stegkit_core STATIC
    src/image_codec.cpp
    src/video_codec.cpp
    src/bitstream.cpp
    src/rng.cpp
    src/key_schedule.cpp
    src/error_correction.cpp
    src/checksum.cpp
    src/lsb_embedder.cpp
    src/dct_embedder.cpp
    src/dwt_embedder.cpp
    src/mv_embedder.cpp
    src/container.cpp
    src/pipeline.cpp
)

target_include_directories(stegkit_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/cxxopts
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/doctest
)

target_link_libraries(stegkit_core PUBLIC
    FFmpeg::avcodec FFmpeg::avformat FFmpeg::avutil FFmpeg::swscale
)

# CLI Apps
add_executable(stegimg apps/stegimg.cpp)
target_link_libraries(stegimg PRIVATE stegkit_core)

add_executable(stegvid apps/stegvid.cpp)
target_link_libraries(stegvid PRIVATE stegkit_core)

# Tests
enable_testing()

add_executable(test_lsb tests/test_lsb.cpp)
target_link_libraries(test_lsb PRIVATE stegkit_core)
add_test(NAME test_lsb COMMAND test_lsb)

add_executable(test_dct tests/test_dct.cpp)
target_link_libraries(test_dct PRIVATE stegkit_core)
add_test(NAME test_dct COMMAND test_dct)

add_executable(test_container tests/test_container.cpp)
target_link_libraries(test_container PRIVATE stegkit_core)
add_test(NAME test_container COMMAND test_container)

add_executable(test_bitstream tests/test_bitstream.cpp)
target_link_libraries(test_bitstream PRIVATE stegkit_core)
add_test(NAME test_bitstream COMMAND test_bitstream)

# Tools
add_executable(gen_noise tools/gen_noise.cpp)
target_link_libraries(gen_noise PRIVATE stegkit_core)

add_executable(capacity_scan tools/capacity_scan.cpp)
target_link_libraries(capacity_scan PRIVATE stegkit_core)
