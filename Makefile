NVC = nvcc
CC = g++

CUDA_PATH = '/opt/linux/centos/7.x/x86_64/pkgs/cuda/10.1/lib64'

main: main.cpp
	$(NVC) -o $@ $^ -L$(CUDA_PATH) -lcudart -lcusolver 

clean:
	rm main
