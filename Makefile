CXX := g++
CXXFLAGS := -O3 -std=c++20 -Wall -Iinclude -pthread
TARGET := build/torky-silicon-bench
all: $(TARGET)
$(TARGET): src/torky_cli_kernel.cpp include/torky_continuum_core.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) src/torky_cli_kernel.cpp -o $(TARGET)
clean:
	rm -rf build
