cmake_minimum_required(VERSION 3.0)
project(REST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS -pthread)
# one version with simple map

add_executable(REST main.cpp)
target_compile_features(REST PRIVATE cxx_std_17)

# one version with rocksdb
# add_executable(REST_rocksdb main_rocksdb.cpp)
# target_link_libraries(REST_rocksdb PRIVATE rocksdb)
# target_link_libraries(REST_rocksdb librocksdb)
# target_include_directories(REST_rocksdb PRIVATE ${rocksdb_SOURCE_DIR}/include)
