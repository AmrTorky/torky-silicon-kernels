CXX ?= g++
CXXFLAGS = -O3 -std=c++20 -march=native -Wall -Wextra -Iinclude -pthread

TARGET = build/torky-silicon-bench
CUDA_BIN = build/blackwell_potential_kernel
PHOTONIC_BIN = build/photonic_wdm_kernel

all: $(TARGET) $(CUDA_BIN) $(PHOTONIC_BIN)

$(TARGET): src/torky_cli_kernel.cpp include/torky_continuum_core.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) src/torky_cli_kernel.cpp -o $(TARGET)

$(CUDA_BIN): src/cuda/blackwell_potential_kernel.cu include/silicon/hardware_constants.cuh
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -Wno-unused-variable -x c++ src/cuda/blackwell_potential_kernel.cu -o $(CUDA_BIN)

$(PHOTONIC_BIN): src/photonic/photonic_wdm_kernel.cpp include/silicon/hardware_constants.cuh
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -Wno-unused-variable src/photonic/photonic_wdm_kernel.cpp -o $(PHOTONIC_BIN)

test: all
	./$(TARGET)
	./$(CUDA_BIN)
	./$(PHOTONIC_BIN)
	python3 src/nki/neuron_continuum_kernel.py
	./run_audit.sh

clean:
	rm -rf build local_verification_certificate.json
