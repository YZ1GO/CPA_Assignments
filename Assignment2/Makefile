CXX = g++
CXXFLAGS = -O3 -std=c++17 -fopenmp -Iinclude

SYCL_CXX ?= icpx

SRC_COMMON = \
	src/matrix.cpp \
	src/timer.cpp \
	src/lu_sequential.cpp \
	src/lu_blocked.cpp \
	src/lu_openmp_loop.cpp \
	src/lu_openmp_task.cpp

SRC_MAIN = \
	src/main.cpp

SRC_BENCH = \
	benchmarks/benchmark.cpp

SRC_SYCL = \
	src/lu_sycl.cpp

TARGET_MAIN = build/lu
TARGET_BENCH = build/benchmark
TARGET_SYCL = build/lu_sycl

all: directories main benchmark

directories:
	mkdir -p build
	mkdir -p results

main:
	$(CXX) $(CXXFLAGS) \
	$(SRC_COMMON) $(SRC_MAIN) \
	-o $(TARGET_MAIN)

benchmark:
	$(CXX) $(CXXFLAGS) \
	$(SRC_COMMON) $(SRC_BENCH) \
	-o $(TARGET_BENCH)

sycl:
	$(SYCL_CXX) -O3 -std=c++17 -fsycl -Iinclude \
	$(SRC_COMMON) $(SRC_SYCL) $(SRC_MAIN) \
	-o $(TARGET_SYCL)

clean:
	rm -rf build results/*.csv results/*.png

.PHONY: all directories main benchmark sycl clean