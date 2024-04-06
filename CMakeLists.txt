cmake_minimum_required(VERSION 3.0)
project(REST)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Release)
#endif()

FetchContent_Declare(
  rocksdb
  GIT_REPOSITORY https://github.com/facebook/rocksdb.git
  GIT_TAG        main # Similarly, lock this to a specific commit
)
FetchContent_MakeAvailable(rocksdb)

# one version with simple map
add_executable(REST main.cpp)

# one version with rocksdb
add_executable(REST_rocksdb main_rocksdb.cpp)
target_link_libraries(REST_rocksdb PRIVATE rocksdb)
target_include_directories(REST_rocksdb PRIVATE ${rocksdb_SOURCE_DIR}/include)
