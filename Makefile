
CXX_FLAGS := -std=c++17 -O2 -pthread
LDFLAGS := -L/home/unal/cpp_rest/rocksdb/ -l:librocksdb.so
INCLUDES := -I./rocksdb/include

all: 
	$(CXX) $(CXX_FLAGS) $(INCLUDES) main_rocksdb.cpp -o ./bench_rocksdb $(LDFLAGS)

clear:
	bench_rocksdb
