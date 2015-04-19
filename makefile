#  Author: Aditya Vijayakumar
#  Desc.:  Makefile for building GPU max element
#  Date:   Apr 9, 2011
#  Requirements: OpenCL drivers, gcc

APP_NAME = “app”
SORT_PROJECT_NAME = "GPU Sort”
MAX_PROJECT_NAME = "GPU Max Element"
SORT_C_FILES = "examples/main_sort.c” "floatCL/floatCL.c"
MAX_C_FILES = "examples/main_max.c” "floatCL/floatCL.c"
COPY_FILES = "floatCL/floatCL.cl" “examples/input.txt"
LINKER_FLAGS = -lOpenCL -lm -o3

build_sort: clean
	mkdir -p build/$(SORT_PROJECT_NAME)
	g++ $(LINKER_FLAGS) $(SORT_C_FILES) -o build/$(SORT_PROJECT_NAME)/$(APP_NAME)
	cp $(COPY_FILES) build/$(SORT_PROJECT_NAME)

build_max: clean
	mkdir -p build/$(MAX_PROJECT_NAME)
	g++ $(LINKER_FLAGS) $(MAX_C_FILES) -o build/$(MAX_PROJECT_NAME)/$(APP_NAME)
	cp $(COPY_FILES) build/$(MAX_PROJECT_NAME)

clean: 
	rm -rf build
